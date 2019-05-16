# OpenCensus C++ Trace library.
# See ../../README.md for details.
#
# Copyright 2017, OpenCensus Authors
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
    name = "trace",
    copts = [
        "-DNULL_PLUGIN=1",
        "-DEMSCRIPTEN_PROTOBUF_LITE=1",
    ],
    repository = "@envoy",
    srcs = [
        "internal/annotation.cc",
        "internal/attribute_list.cc",
        "internal/attribute_value.cc",
        "internal/attribute_value_ref.cc",
        "internal/event_with_time.h",
        "internal/link.cc",
        "internal/local_span_store.cc",
        "internal/local_span_store_impl.cc",
        "internal/message_event.cc",
        "internal/running_span_store.cc",
        "internal/running_span_store_impl.cc",
        "internal/sampler.cc",
        "internal/span.cc",
        "internal/span_data.cc",
        "internal/span_exporter.cc",
        "internal/span_exporter_impl.cc",
        "internal/span_impl.cc",
        "internal/status.cc",
        "internal/trace_config.cc",
        "internal/trace_config_impl.cc",
    ],
    hdrs = [
        "attribute_value_ref.h",
        "exporter/annotation.h",
        "exporter/attribute_value.h",
        "exporter/link.h",
        "exporter/message_event.h",
        "exporter/span_data.h",
        "exporter/span_exporter.h",
        "exporter/status.h",
        "internal/attribute_list.h",
        "internal/local_span_store.h",
        "internal/local_span_store_impl.h",
        "internal/running_span_store.h",
        "internal/running_span_store_impl.h",
        "internal/span_exporter_impl.h",
        "internal/span_impl.h",
        "internal/trace_config_impl.h",
        "internal/trace_events.h",
        "internal/trace_params_impl.h",
        "sampler.h",
        "span.h",
        "status_code.h",
        "trace_config.h",
        "trace_params.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":cloud_trace_context",
        ":span_context",
        ":trace_context",
        "//opencensus/common/internal:random_lib",
    ],
)

envoy_cc_library(
    name = "cloud_trace_context",
    copts = [
        "-DNULL_PLUGIN=1",
        "-DEMSCRIPTEN_PROTOBUF_LITE=1",
    ],
    repository = "@envoy",
    srcs = [
        "internal/cloud_trace_context.cc",
    ],
    hdrs = [
        "propagation/cloud_trace_context.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span_context",
    ],
)

envoy_cc_library(
    name = "context_util",
    copts = [
        "-DNULL_PLUGIN=1",
        "-DEMSCRIPTEN_PROTOBUF_LITE=1",
    ],
    repository = "@envoy",
    srcs = ["internal/context_util.cc"],
    hdrs = ["context_util.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":trace",
        "//opencensus/context",
    ],
)

envoy_cc_library(
    name = "grpc_trace_bin",
    copts = [
        "-DNULL_PLUGIN=1",
        "-DEMSCRIPTEN_PROTOBUF_LITE=1",
    ],
    repository = "@envoy",
    srcs = [
        "internal/grpc_trace_bin.cc",
    ],
    hdrs = [
        "propagation/grpc_trace_bin.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span_context",
    ],
)

envoy_cc_library(
    name = "span_context",
    copts = [
        "-DNULL_PLUGIN=1",
        "-DEMSCRIPTEN_PROTOBUF_LITE=1",
    ],
    repository = "@envoy",
    srcs = [
        "internal/span_context.cc",
        "internal/span_id.cc",
        "internal/trace_id.cc",
        "internal/trace_options.cc",
    ],
    hdrs = [
        "span_context.h",
        "span_id.h",
        "trace_id.h",
        "trace_options.h",
    ],
    visibility = ["//visibility:public"],
)

envoy_cc_library(
    name = "trace_context",
    srcs = [
        "internal/trace_context.cc",
    ],
    hdrs = [
        "propagation/trace_context.h",
    ],
    copts = [
        "-DNULL_PLUGIN=1",
        "-DEMSCRIPTEN_PROTOBUF_LITE=1",
    ],
    repository = "@envoy",
    visibility = ["//visibility:public"],
    deps = [
        ":span_context",
    ],
)

envoy_cc_library(
    name = "with_span",
    copts = [
        "-DNULL_PLUGIN=1",
        "-DEMSCRIPTEN_PROTOBUF_LITE=1",
    ],
    repository = "@envoy",
    srcs = ["internal/with_span.cc"],
    hdrs = ["with_span.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":trace",
        "//opencensus/context",
    ],
)