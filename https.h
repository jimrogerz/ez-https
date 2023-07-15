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

#ifndef LIB_HTTPS_H
#define LIB_HTTPS_H

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/certify/extensions.hpp>
#include <boost/certify/https_verification.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

/** Network adapter for HTTP. */
class HttpAdapter {
public:
  // Connects to the HTTP server over TLS.
  virtual absl::Status Connect() = 0;

  // Executes a request.
  virtual absl::StatusOr<
      boost::beast::http::response<boost::beast::http::string_body>>
  Execute(const boost::beast::http::request<boost::beast::http::string_body>
              &request) = 0;

  // Returns the hostname.
  virtual std::string Hostname() = 0;

  // Disconnects from the HTTP server.
  virtual void Disconnect() = 0;
};

/** Concrete implementation of HttpAdapter. */
class HttpAdapterImpl : public HttpAdapter {
public:
  HttpAdapterImpl(absl::string_view hostname)
      : hostname_(std::string(hostname)),
        ssl_ctx_(
            boost::asio::ssl::context(boost::asio::ssl::context::tls_client)) {
    ssl_ctx_.set_verify_mode(
        boost::asio::ssl::context::verify_peer |
        boost::asio::ssl::context::verify_fail_if_no_peer_cert);
    ssl_ctx_.set_default_verify_paths();
    boost::certify::enable_native_https_server_verification(ssl_ctx_);
  }

  absl::StatusOr<boost::beast::http::response<boost::beast::http::string_body>>
  Execute(const boost::beast::http::request<boost::beast::http::string_body>
              &request) override;

  absl::Status Connect() override;

  std::string Hostname() override { return hostname_; }

  void Disconnect() override { stream_ptr_->next_layer().close(error_); }

private:
  std::string hostname_;
  boost::asio::io_context ctx_;
  boost::asio::ssl::context ssl_ctx_;
  boost::system::error_code error_;
  std::unique_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
      stream_ptr_;
  absl::Status Error() {
    return this->error_ ? absl::InternalError(this->error_.message())
                        : absl::OkStatus();
  }
};

/**
 * Performs GET, PUT, POST, and DELETE requests over TLS and
 * decompresses responses.
 *
 * TODO (jimrogerz@gmail.com): compress PUT/POST bodies.
 *
 * Example usage:
 *
 * #include "https.h"
 *
 * Https https("api.openai.com");
 *
 * auto status = https.SetPath("/v1/images/generations")
 *                  .SetContentType("application/json")
 *                  .SetAuthorization(absl::StrCat("Bearer ", kApiKey));
 *                  .KeepAlive();
 *                  .Connect();
 * if (!status.ok()) {
 *   std::cerr << "Unable to connect: " << connection_status << std::endl;
 *   return 1;
 * }
 *
 * auto response = https.Post(absl::StrFormat(
 *     "{"
 *     "\"prompt\": \"%s\","
 *     "\"n\": %d,"
 *     "\"size\": \"%s\","
 *     "\"response_format\": \"%s\""
 *     "}",
 *     prompt, num_images, size, image_count == 1 ? "b64_json" : "url"));
 * if (!response.ok()) {
 *   std::cerr << "Unable to post request: " << response.status() << std::endl;
 *   return 1;
 * }
 *
 * https.Close();
 *
 * std::cout << response->body() << std::endl;
 */
class Https {

public:
  Https(std::shared_ptr<HttpAdapter> adapter)
      : path_("/"), content_type_(""), authorization_(""),
        verb_(boost::beast::http::verb::get), adapter_(adapter) {}

  Https(absl::string_view hostname)
      : Https(std::make_shared<HttpAdapterImpl>(hostname)) {}

  // The path and query of the request.
  Https &SetPath(absl::string_view path) {
    path_ = std::string(path);
    return *this;
  }

  // The content type.
  Https &SetContentType(absl::string_view content_type) {
    content_type_ = content_type;
    return *this;
  }

  // The authorization header.
  Https &SetAuthorization(absl::string_view authorization) {
    authorization_ = authorization;
    return *this;
  }

  // Option to keep the connection alive.
  Https &KeepAlive() {
    keep_alive_ = true;
    return *this;
  }

  // Connects to the remote server over TLS.
  absl::Status Connect() { return adapter_->Connect(); }

  // Gets the resource.
  absl::StatusOr<boost::beast::http::response<boost::beast::http::string_body>>
  Get() {
    verb_ = boost::beast::http::verb::get;
    return this->Execute();
  }

  // Posts to a resource.
  absl::StatusOr<boost::beast::http::response<boost::beast::http::string_body>>
  Post(absl::string_view body) {
    body_ = body;
    verb_ = boost::beast::http::verb::post;
    return this->Execute();
  }

  // Puts to a resource.
  absl::StatusOr<boost::beast::http::response<boost::beast::http::string_body>>
  Put(absl::string_view body) {
    body_ = body;
    verb_ = boost::beast::http::verb::put;
    return this->Execute();
  }

  // Closes the connection.
  void Close() { adapter_->Disconnect(); }

private:
  std::string path_;
  std::string content_type_;
  std::string authorization_;
  std::string body_;
  boost::beast::http::verb verb_;
  std::shared_ptr<HttpAdapter> adapter_;
  bool keep_alive_;
  absl::StatusOr<boost::beast::http::response<boost::beast::http::string_body>>
  Execute();
};

#endif // LIB_HTTPS_H
