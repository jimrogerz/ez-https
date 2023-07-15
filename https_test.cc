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

namespace {

namespace http = boost::beast::http;

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
};

TEST_F(HttpsTest, Connect) {
  EXPECT_CALL(adapter(), Connect()).Times(1);

  EXPECT_TRUE(https_.Connect().ok());
}

} // namespace
