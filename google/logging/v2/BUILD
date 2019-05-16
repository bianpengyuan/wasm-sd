load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_proto_library",
)

envoy_proto_library(
    name = "log_entry_proto",
    srcs = ["log_entry.proto"],
    deps = [
        "//google/api:monitored_resource_proto",
        "//google/logging/type:http_request_proto",
        "//google/logging/type:log_severity_proto",
    ],
)

envoy_proto_library(
    name = "logging_proto",
    srcs = ["logging.proto"],
    deps = [
        ":log_entry_proto",
        "//google/api:monitored_resource_proto",
    ],
)