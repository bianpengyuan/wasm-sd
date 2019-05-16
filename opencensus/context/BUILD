# OpenCensus C++ Context library.
# See context.h for details.
#
# Copyright 2018, OpenCensus Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
)

envoy_cc_library(
    name = "context",
    copts = [
        "-DNULL_PLUGIN=1",
        "-DEMSCRIPTEN_PROTOBUF_LITE=1",
    ],
    repository = "@envoy",
    srcs = [
        "internal/context.cc",
        "internal/with_context.cc",
    ],
    hdrs = [
        "context.h",
        "with_context.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//opencensus/tags",
        "//opencensus/trace",
    ],
)