# EZ-HTTPS Copyright 2023 Jim Rogers (jimrogerz@gmail.com).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.load("@rules_cc//cc:defs.bzl", "cc_library")

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

package(default_visibility = ["//visibility:public"])

# https://github.com/djarek/certify
cc_library(
    name = "certify",
    hdrs = glob(["**/*.hpp", "**/*.ipp"]),
    includes = ["include"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "https",
    srcs = ["https.cc"],
    hdrs = ["https.h"],
    deps = [
        "@status_macros//:status_macros",
        "@djarek-certify//:certify",
        "@boost//:beast",
        "@boost//:iostreams",
        "@boost//:asio",
        "@boost//:asio_ssl",
        "@boost//:serialization",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
   ],
)

cc_test(
  name = "https_test",
  srcs = ["https_test.cc"],
  deps = [
    ":https",
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/status:statusor",
    "@com_google_googletest//:gtest_main"
  ],
)

cc_binary(
    name = "https_main",
    srcs = ["https_main.cc"],
    deps = [
        ":https",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
    ],
)
