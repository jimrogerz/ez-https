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

workspace(name = "root")
 
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

http_archive(
  name = "com_google_absl",
  urls = ["https://github.com/abseil/abseil-cpp/archive/c2435f8342c2d0ed8101cb43adfd605fdc52dca2.zip"],
  strip_prefix = "abseil-cpp-c2435f8342c2d0ed8101cb43adfd605fdc52dca2",
  sha256 = "ea1d31db00eb37e607bfda17ffac09064670ddf05da067944c4766f517876390",
)

http_archive(
  name = "bazel_skylib",
  urls = ["https://github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz"],
  sha256 = "f7be3474d42aae265405a592bb7da8e171919d74c16f082a5457840f06054728",
)

http_archive(
  name = "com_google_googletest",
  urls = ["https://github.com/google/googletest/archive/5ab508a01f9eb089207ee87fd547d290da39d015.zip"],
  strip_prefix = "googletest-5ab508a01f9eb089207ee87fd547d290da39d015",
)

git_repository(
    name = "djarek-certify",
    build_file = "@//:BUILD",
    commit = "97f5eebfd99a5d6e99d07e4820240994e4e59787",
    remote = "https://github.com/djarek/certify.git",
)

# https://github.com/nelhage/rules_boost and https://www.boost.org
http_archive(
    name = "com_github_nelhage_rules_boost",
    url = "https://github.com/nelhage/rules_boost/archive/a6a92d28886fdaba80fb00cee54ee7c72416af20.tar.gz",
    strip_prefix = "rules_boost-a6a92d28886fdaba80fb00cee54ee7c72416af20",
)
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

git_repository(
    name = "status_macros",
    commit = "7ac615a3f354671e9683542eaaa7194991c38105",
    remote = "https://github.com/jimrogerz/status_macros.git",
)
