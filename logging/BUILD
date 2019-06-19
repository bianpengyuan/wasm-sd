load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
)

envoy_cc_library(
    name = "stackdriver_logger",
    copts = [
        "-DNULL_PLUGIN=1",
        "-DEMSCRIPTEN_PROTOBUF_LITE=1",
    ],
    repository = "@envoy",
    hdrs = [
        "logger.h",
    ],
    srcs = [
        "logger.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//:stackdriver_exporter_options_hdr",
        "//google/logging/v2:log_entry_proto_cc",
        "//google/logging/v2:logging_proto_cc",
        "//opencensus/tags:tags",
        "@envoy//source/extensions/common/wasm/null:null_lib",
    ],
)