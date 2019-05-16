# OpenCensus C++ Tags library.
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
    name = "exporter",
    copts = [
        "-DNULL_PLUGIN=1",
        "-DEMSCRIPTEN_PROTOBUF_LITE=1",
    ],
    repository = "@envoy",
    srcs = [
        "internal/stackdriver_utils.cc",
        "internal/stackdriver_exporter.cc",
    ],
    hdrs = [
        "internal/stackdriver_utils.h",
        "stackdriver_exporter.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//google/api:metric_proto_cc",
        "//google/monitoring/v3:metric_proto_cc",
        "//google/monitoring/v3:metric_service_proto_cc",
        "//opencensus/common/internal:hash_mix",
        "//opencensus/stats:stats",
    ],
)
