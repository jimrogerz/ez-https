// EZ-HTTPS Copyright 2023 Jim Rogers (jimrogerz@gmail.com).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "https.h"
#include "status_macros.h"
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/detail/fstream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/stream.hpp>

namespace {
constexpr int kMaxBits = 15; // 32K LZ77 window

namespace beast = boost::beast;
namespace asio = boost::asio;
namespace ssl = asio::ssl;
namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

void Decompress(http::response<http::string_body> &response) {
  auto content_encoding = response["Content-Encoding"];
  if (content_encoding != "gzip" && content_encoding != "deflate") {
    return;
  }

  boost::iostreams::filtering_istream is;
  if (content_encoding == "gzip") {
    is.push(boost::iostreams::gzip_decompressor());
  } else {
    is.push(boost::iostreams::zlib_decompressor(-kMaxBits));
  }

  boost::iostreams::array_source src(response.body().data(),
                                     response.body().size());
  is.push(src);
  std::string s;
  boost::iostreams::copy(is, boost::iostreams::back_inserter(s));
  response.body() = s;
}

} // namespace

absl::Status Https::Connect() {
  tcp::socket socket(ctx_);
  tcp::resolver resolver(ctx_);
  auto endpoint = resolver.resolve(hostname_, "443", error_);
  if (error_)
    return Error();
  asio::connect(socket, endpoint, error_);
  if (error_)
    return Error();
  stream_ptr_ =
      boost::make_unique<ssl::stream<tcp::socket>>(std::move(socket), ssl_ctx_);
  boost::certify::set_server_hostname(*stream_ptr_, hostname_);
  boost::certify::sni_hostname(*stream_ptr_, hostname_);
  stream_ptr_->handshake(ssl::stream_base::handshake_type::client, error_);
  if (error_)
    return Error();
  return absl::OkStatus();
}

absl::StatusOr<http::response<http::string_body>> Https::Execute() {
  http::request<http::string_body> request;
  request.method(verb_);
  request.target(path_);
  request.keep_alive(keep_alive_);
  request.set(http::field::accept_encoding, "gzip, deflate");
  request.set(http::field::host, hostname_);
  if (!authorization_.empty()) {
    request.set(http::field::authorization, authorization_);
  }
  if (!content_type_.empty()) {
    request.set(http::field::content_type, content_type_);
  }
  if (!body_.empty()) {
    request.set(http::field::content_length, std::to_string(body_.length()));
    request.body() = body_;
  }
  http::write(*stream_ptr_, request, error_);
  if (error_)
    return Error();
  http::response<http::string_body> response;
  beast::flat_buffer buffer;
  http::read(*stream_ptr_, buffer, response, error_);
  if (error_)
    return Error();
  Decompress(response);
  return response;
}
