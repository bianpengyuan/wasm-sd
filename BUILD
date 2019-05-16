load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
    "envoy_cc_binary",
)

envoy_cc_library(
    name = "stackdriver_plugin",
    repository = "@envoy",
    copts = [
        "-DNULL_PLUGIN=1",
        "-DEMSCRIPTEN_PROTOBUF_LITE=1",
    ],
    hdrs = [
        "include/stackdriver.h",
    ],
    srcs = [
        "null_plugin_wrapper.cc",
        "stackdriver.cc",
    ],
    deps = [
        "//logging:stackdriver_logger",
        "//istio:opencensus_registry",
        "//opencensus/exporters/stats/stackdriver:exporter",
        "@envoy//source/extensions/common/wasm/null:null_lib",
        "@envoy_api//envoy/config/wasm/v2:wasm_cc",
    ],
)

envoy_cc_binary(
    name = "envoy",
    repository = "@envoy",
    visibility = ["//visibility:public"],
    deps = [
        ":stackdriver_plugin",
        "@envoy//source/exe:envoy_main_entry_lib",
    ],
)