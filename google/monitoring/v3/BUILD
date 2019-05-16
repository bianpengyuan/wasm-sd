load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_proto_library",
)

envoy_proto_library(
    name = "common_proto",
    srcs = ["common.proto"],
    deps = [
        "//google/api:distribution_proto",
    ],
)

envoy_proto_library(
    name = "metric_service_proto",
    srcs = ["metric_service.proto"],
    deps = [
        ":common_proto",
        ":metric_proto",
        "//google/api:monitored_resource_proto",
        "//google/api:metric_proto",
    ],
)

envoy_proto_library(
    name = "metric_proto",
    srcs = ["metric.proto"],
    deps = [
        "//google/api:label_proto",
        "//google/api:metric_proto",
        "//google/api:monitored_resource_proto",
        ":common_proto",
    ],
)