# EZ-HTTPS

A C++ HTTP TLS library built with [boost](https://www.boost.org/) and [abseil](https://abseil.io/).

## Setup

Add the http_archive rule:

```
http_archive = use_repo_rule("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
```

Add the http_archive using the latest commit from https://github.com/jimrogerz/ez-https/commits/main/:

```
EZ_HTTPS_COMMIT = "11955c784b54e5517073d2915edb563dff1e0ca2"
http_archive(
    name = "ez-https",
    strip_prefix = "ez-https-" + EZ_HTTPS_COMMIT,
    url = "https://github.com/jimrogerz/ez-https/archive/%s.zip" % EZ_HTTPS_COMMIT,
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
