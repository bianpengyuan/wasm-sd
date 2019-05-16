load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
)

envoy_cc_library(
    name = "opencensus_registry",
    hdrs = [
        "measure.h",
        "tag.h",
        "view.h",
    ],
    repository = "@envoy",
    visibility = ["//visibility:public"],
    deps = [
        "@envoy//source/extensions/common/wasm/null:null_lib",
        "@envoy_api//envoy/config/wasm/v2:wasm_cc",
    ],
)