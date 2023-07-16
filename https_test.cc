// EZ-HTTPS Copyright 2023 Jim Rogers.
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
#include "gmock/gmock.h"
#include <gtest/gtest.h>

namespace ez {
namespace smtp {
namespace {

const std::string kTestHostName = "test_hostname";
const std::string kTestBody = "This is the message body.";
const std::string kEmptyTestBody = "";
const std::string kTestPath = "/some/path?and=query";
const std::string kTestContentType = "ContentType";
const std::string kTestAuthorization = "authorization header";
const std::string kTestEncoding = "gzip, deflate";

namespace http = boost::beast::http;

using testing::Eq;
using testing::Return;

MATCHER_P2(RequestEq, expected_method, expected_body, "") {
  if (expected_body.length() > 0) {
    if (arg.at(http::field::content_length) !=
        std::to_string(expected_body.length())) {
      return false;
    }
  }
  return arg.method() == expected_method && arg.target() == kTestPath &&
         arg.keep_alive() == true && arg.body() == expected_body &&
         arg.at(http::field::accept_encoding) == kTestEncoding &&
         arg.at(http::field::host) == kTestHostName &&
         arg.at(http::field::authorization) == kTestAuthorization &&
         arg.at(http::field::content_type) == kTestContentType;
}

class MockHttpAdapter : public HttpAdapter {
public:
  MOCK_METHOD(absl::Status, Connect, (), (override));
  MOCK_METHOD(absl::StatusOr<http::response<http::string_body>>, Execute,
              (const http::request<http::string_body> &request), (override));
  MOCK_METHOD(std::string, Hostname, (), (override));
  MOCK_METHOD(void, Disconnect, (), (override));
};

class HttpsTest : public ::testing::Test {
public:
  HttpsTest()
      : mock_adapter_(std::make_shared<MockHttpAdapter>()),
        https_(mock_adapter_) {}

protected:
  MockHttpAdapter &adapter() { return *mock_adapter_; }
  std::shared_ptr<MockHttpAdapter> mock_adapter_;
  Https https_;
  absl::StatusOr<http::response<http::string_body>> SuccessResponse() {
    http::response<http::string_body> response;
    return response;
  }
};

TEST_F(HttpsTest, Connect) {
  EXPECT_CALL(adapter(), Connect()).Times(1);

  EXPECT_TRUE(https_.Connect().ok());
}

TEST_F(HttpsTest, Disconnect) {
  EXPECT_CALL(adapter(), Disconnect()).Times(1);

  https_.Close();
}

TEST_F(HttpsTest, Get) {
  EXPECT_CALL(adapter(), Hostname()).Times(1).WillOnce(Return(kTestHostName));
  EXPECT_CALL(adapter(),
              Execute(RequestEq(boost::beast::http::verb::get, kEmptyTestBody)))
      .Times(1)
      .WillOnce(Return(SuccessResponse()));

  auto response = https_.SetPath(kTestPath)
                      .SetContentType(kTestContentType)
                      .SetAuthorization(kTestAuthorization)
                      .KeepAlive()
                      .Get();

  EXPECT_TRUE(response.ok());
}

TEST_F(HttpsTest, Put) {
  EXPECT_CALL(adapter(), Hostname()).Times(1).WillOnce(Return(kTestHostName));
  EXPECT_CALL(adapter(),
              Execute(RequestEq(boost::beast::http::verb::put, kTestBody)))
      .Times(1)
      .WillOnce(Return(SuccessResponse()));

  auto response = https_.SetPath(kTestPath)
                      .SetContentType(kTestContentType)
                      .SetAuthorization(kTestAuthorization)
                      .KeepAlive()
                      .Put(kTestBody);

  EXPECT_TRUE(response.ok());
}

TEST_F(HttpsTest, Post) {
  EXPECT_CALL(adapter(), Hostname()).Times(1).WillOnce(Return(kTestHostName));
  EXPECT_CALL(adapter(),
              Execute(RequestEq(boost::beast::http::verb::post, kTestBody)))
      .Times(1)
      .WillOnce(Return(SuccessResponse()));

  auto response = https_.SetPath(kTestPath)
                      .SetContentType(kTestContentType)
                      .SetAuthorization(kTestAuthorization)
                      .KeepAlive()
                      .Post(kTestBody);

  EXPECT_TRUE(response.ok());
}

TEST_F(HttpsTest, Delete) {
  EXPECT_CALL(adapter(), Hostname()).Times(1).WillOnce(Return(kTestHostName));
  EXPECT_CALL(adapter(), Execute(RequestEq(boost::beast::http::verb::delete_,
                                           kEmptyTestBody)))
      .Times(1)
      .WillOnce(Return(SuccessResponse()));

  auto response = https_.SetPath(kTestPath)
                      .SetContentType(kTestContentType)
                      .SetAuthorization(kTestAuthorization)
                      .KeepAlive()
                      .Delete();

  EXPECT_TRUE(response.ok());
}

} // namespace
} // namespace smtp
} // namespace ez
