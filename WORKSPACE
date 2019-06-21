load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
bind(
    name = "boringssl_crypto",
    actual = "//external:ssl",
)

# When updating envoy sha manually please update the sha in istio.deps file also
#
# Determine SHA256 `wget https://github.com/envoyproxy/envoy/archive/COMMIT.tar.gz && sha256sum COMMIT.tar.gz`
ENVOY_SHA = "674411c3b9d363cef8e88c5604de09b95f0f965c"

# ENVOY_SHA256 = "e549967e81fb44bba52529bab95db8eed32c57db034af358e0947d8e85c420db"

http_archive(
    name = "envoy",
    # sha256 = ENVOY_SHA256,
    strip_prefix = "envoy-wasm-" + ENVOY_SHA,
    url = "https://github.com/envoyproxy/envoy-wasm/archive/" + ENVOY_SHA + ".tar.gz",
)

# load("@envoy//bazel:api_repositories.bzl", "envoy_api_dependencies")
# envoy_api_dependencies()

# load("@io_bazel_rules_go//go:def.bzl", "go_register_toolchains", "go_rules_dependencies")
# load("@envoy//bazel:repositories.bzl", "envoy_dependencies", "GO_VERSION")

# go_rules_dependencies()
# go_register_toolchains(go_version = GO_VERSION)
# envoy_dependencies()

# load("@rules_foreign_cc//:wokspace_definitions.bzl", "rules_foreign_cc_dependencies")
# rules_foreign_cc_dependencies()

# load("@envoy//bazel:cc_configure.bzl", "cc_configure")
# cc_configure()

load("@envoy//bazel:api_repositories.bzl", "envoy_api_dependencies")

envoy_api_dependencies()

load("@envoy//bazel:repositories.bzl", "GO_VERSION", "envoy_dependencies")
load("@envoy//bazel:cc_configure.bzl", "cc_configure")

envoy_dependencies()

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

cc_configure()

load("@io_bazel_rules_go//go:deps.bzl", "go_register_toolchains", "go_rules_dependencies")

go_rules_dependencies()

go_register_toolchains(go_version = GO_VERSION)

TELEMETRY_GOOGLEAPIS_SHA="c39b7e880e6db2ce61704da2a55083ea17fdb14b"
TELEMETRY_GOOGLEAPIS_SHA256="ee05b85961aa721671d85c111c6287e9667e69b616d97959588b1a991ef44a2d"
TELEMETRY_GOOGLEAPIS_URLS=["https://github.com/googleapis/googleapis/archive/" + TELEMETRY_GOOGLEAPIS_SHA + ".tar.gz"]

http_archive(
    name = "telemetry_googleapis",
    urls = TELEMETRY_GOOGLEAPIS_URLS,
    sha256 = TELEMETRY_GOOGLEAPIS_SHA256,
    strip_prefix = "googleapis-" + TELEMETRY_GOOGLEAPIS_SHA,
)

load("@telemetry_googleapis//:repository_rules.bzl", "switched_rules_by_language")

switched_rules_by_language(
    name = "com_google_googleapis_imports",
    cc = True,
)
