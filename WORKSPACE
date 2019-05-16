load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
bind(
    name = "boringssl_crypto",
    actual = "//external:ssl",
)

# When updating envoy sha manually please update the sha in istio.deps file also
#
# Determine SHA256 `wget https://github.com/envoyproxy/envoy/archive/COMMIT.tar.gz && sha256sum COMMIT.tar.gz`
ENVOY_SHA = "0f0d43face99d5df65debc1de500a174c4e478cb"

# ENVOY_SHA256 = "e549967e81fb44bba52529bab95db8eed32c57db034af358e0947d8e85c420db"

http_archive(
    name = "envoy",
    # sha256 = ENVOY_SHA256,
    strip_prefix = "envoy-" + ENVOY_SHA,
    url = "https://github.com/jplevyak/envoy/archive/" + ENVOY_SHA + ".tar.gz",
)

load("@envoy//bazel:repositories.bzl", "envoy_dependencies", "GO_VERSION")
envoy_dependencies()

load("@envoy_api//bazel:repositories.bzl", "api_dependencies")
api_dependencies()

load("@io_bazel_rules_go//go:def.bzl", "go_register_toolchains", "go_rules_dependencies")
go_rules_dependencies()
go_register_toolchains(go_version = GO_VERSION)

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")
rules_foreign_cc_dependencies()

load("@envoy//bazel:cc_configure.bzl", "cc_configure")
cc_configure()
