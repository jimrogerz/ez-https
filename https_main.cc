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

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "https.h"

ABSL_FLAG(std::string, server, "", "Server to connect to");
ABSL_FLAG(std::string, path_and_query, "", "Path and query to request");

/**
 * Performs a GET request.
 *  
 * Example usage:
 *
 *   bazel run :https_main -- --server="google.com" --path_and_query="/search?q=ez-https"
 */
int main(int argc, char **argv) {
  absl::ParseCommandLine(argc, argv);

  Https https(absl::GetFlag(FLAGS_server));
  auto status = https.Connect();
  if (!status.ok()) {
    std::cerr << status << std::endl;
    return 1;
  }

  auto response = https.SetPath(absl::GetFlag(FLAGS_path_and_query)).Get();
  if (!response.ok()) {
    std::cerr << response.status() << std::endl;
    return 1;
  }

  https.Close();
  
  std::cout << response->body() << std::endl;

  return 0;
}
