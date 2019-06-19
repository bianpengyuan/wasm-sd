#ifndef INCLUDE_STACKDRIVER_EXPORTER_OPTIONS_H
#define INCLUDE_STACKDRIVER_EXPORTER_OPTIONS_H

#ifndef NULL_PLUGIN
#include "api/wasm/cpp/proxy_wasm_intrinsics.h"
#else

#include "extensions/common/wasm/null/null.h"

namespace Envoy {
namespace Extensions {
namespace Common {
namespace Wasm {
namespace Null {
namespace Plugin {
namespace Stackdriver {
#endif

struct StackdriverOptions {
  // Context which implementes wasm sandbox api. This typically should be root context.
  RootContext* context;

  // type of monitored resource that metrics should attach to, such as k8s_pod, k8s_container.
  std::string monitored_resource_type;

  std::string project_id;

  // lables for monitored resource.
  std::unordered_map<std::string, std::string> monitored_resource_labels;
};

#ifdef NULL_PLUGIN
} // namespace Stackdriver
} // namespace Plugin
} // namespace Null
} // namespace Wasm
} // namespace Common
} // namespace Extensions
} // namespace Envoy
#endif

#endif // INCLUDE_STACKDRIVER_EXPORTER_OPTIONS_H