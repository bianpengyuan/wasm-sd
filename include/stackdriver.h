#ifndef INCLUDE_STACKDRIVER_H
#define INCLUDE_STACKDRIVER_H

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
using namespace Plugin;
#endif

class StackdriverContext : public Context {
 public:
  explicit StackdriverContext(uint32_t id) : Context(id) {}
  void onStart() override;
  void onCreate() override;
  void onLog() override;
  void onTick() override;

  FilterHeadersStatus onRequestHeaders() override;
  FilterDataStatus
  onRequestBody(size_t body_buffer_length, bool end_of_stream) override;
  FilterHeadersStatus onResponseHeaders() override;
  FilterDataStatus
  onResponseBody(size_t body_buffer_length, bool end_of_stream) override;

 private:
  int32_t tick_counter_ = 0;
  bool need_flush_ = false;
  uint64_t start_time_ = 0;
  uint64_t received_bytes_ = 0;
  uint64_t sent_bytes_ = 0;
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

#endif // INCLUDE_STACKDRIVER_H