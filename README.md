# EZ-HTTPS

A [bazel](https://bazel.build/) C++ HTTP TLS library built on top of boost.

## Setup

Add the following to your WORKSPACE file:


```
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "ez-https",
    commit = "ed408505f8939b48da22363e7ae471f4e0c98d1e",
    remote = "https://github.com/jimrogerz/ez-https.git",
)
```

Add `"@ez-https//:https"` to your BUILD deps.

## Usage

Example POST request to OpenAI:

```cpp
#include "https.h"

Https https("api.openai.com");

auto status = https.Connect();
if (!status.ok()) {
  std::cerr << "Unable to connect: " << status << std::endl;
  return 1;
}

absl::string_view body = absl::StrFormat(
    "{"
    "\"prompt\": \"%s\","
    "\"n\": %d,"
    "\"size\": \"%s\","
    "\"response_format\": \"%s\""
    "}",
    prompt, num_images, size, image_count == 1 ? "b64_json" : "url");

auto response = https.SetPath("/v1/images/generations")
                     .SetContentType("application/json")
                     .SetAuthorization(absl::StrCat("Bearer ", kApiKey));
                     .Post(body);

https.Close();

if (!response.ok()) {
  std::cerr << "Unable to post request: " << response.status() << std::endl;
  return 1;
}

std::cout << response->body();
```

## Contributing

Follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html). Update unit tests and confirm passing:

```
bazel test :https_test
```

Format the code:

```
clang-format -i *.cc *.h
```
