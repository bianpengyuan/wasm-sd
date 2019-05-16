load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_proto_library",
)

envoy_proto_library(
    name = "annotations_proto",
    srcs = ["annotations.proto"],
)

envoy_proto_library(
    name = "distribution_proto",
    srcs = ["distribution.proto"],
)

envoy_proto_library(
    name = "http_proto",
    srcs = ["http.proto"],
)

envoy_proto_library(
    name = "label_proto",
    srcs = ["label.proto"],
)

envoy_proto_library(
    name = "launch_stage_proto",
    srcs = ["launch_stage.proto"],
)

envoy_proto_library(
    name = "metric_proto",
    srcs = ["metric.proto"],
    deps = [
        ":label_proto",
        ":launch_stage_proto",
    ]
)

envoy_proto_library(
    name = "monitored_resource_proto",
    srcs = ["monitored_resource.proto"],
    deps = [
        ":label_proto",
    ]
)
