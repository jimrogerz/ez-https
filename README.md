# EZ-HTTPS

A C++ HTTP over TLS library built with [bazel](https://bazel.build/).

## Setup

Add the following to your WORKSPACE file:


```
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "ez-https",
    commit = "3b3812d37c0562ece3234129449ea53cba47bc70",
    remote = "https://github.com/jimrogerz/ez-https.git",
)
```

Add `"@ez-https//:https"` to your BUILD deps.

## Usage

Example request to the OpenAI API:

```cpp
#include "https.h"

Https https("api.openai.com");

auto status = https.SetPath("/v1/images/generations")
                 .SetContentType("application/json")
                 .SetAuthorization(absl::StrCat("Bearer ", kApiKey));
                 .KeepAlive();
                 .Connect();
if (!status.ok()) {
  std::cerr << "Unable to connect: " << connection_status << std::endl;
  return 1;
}

auto response = https.Post(absl::StrFormat(
    "{"
    "\"prompt\": \"%s\","
    "\"n\": %d,"
    "\"size\": \"%s\","
    "\"response_format\": \"%s\""
    "}",
    prompt, num_images, size, image_count == 1 ? "b64_json" : "url"));
if (!response.ok()) {
  std::cerr << "Unable to post request: " << response.status() << std::endl;
  return 1;
}

https.Close();

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
