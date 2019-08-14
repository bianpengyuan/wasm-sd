#ifndef WASM_SD_LOGGING_LOGGING_H
#define WASM_SD_LOGGING_LOGGING_H

#include <string>
#include <vector>

#include "google/logging/v2/log_entry.pb.h"
#include "google/logging/v2/logging.pb.h"
#include "opencensus/tags/tag_key.h"

#ifndef NULL_PLUGIN
#include "api/wasm/cpp/proxy_wasm_intrinsics.h"
#else
#include "extensions/common/wasm/null/null.h"
using namespace Envoy::Extensions::Common::Wasm::Null::Plugin;

namespace Envoy {
namespace Extensions {
namespace Common {
namespace Wasm {
namespace Null {
namespace Plugin {
namespace Stackdriver {
#endif

namespace logging {


}  // namespace logging

#ifdef NULL_PLUGIN
} // namespace Stackdriver
} // namespace Plugin
} // namespace Null
} // namespace Wasm
} // namespace Common
} // namespace Extensions
} // namespace Envoy
#endif

#endif //WASM_SD_LOGGING_LOGGING_H
