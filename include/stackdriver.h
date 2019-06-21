#ifndef INCLUDE_STACKDRIVER_H
#define INCLUDE_STACKDRIVER_H

#include "config/stackdriver_module.pb.h"
#include "include/stackdriver_exporter_options.h"

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
NULL_PLUGIN_ROOT_REGISTRY;
using namespace Plugin;
#endif

class StackdriverRootContext : public RootContext {
 public:
  StackdriverRootContext(uint32_t id, StringView root_id) : RootContext(id, root_id) {}
  ~StackdriverRootContext() {}

  void onConfigure(std::unique_ptr<WasmData> /* configuration */) override;
  void onStart() override;
  void onTick() override;
  void onQueueReady(uint32_t /* token */) override {}

  bool loggingEnabled() { 
    return module_config_.enable_logging();
  }
  bool monitoringEnabled() {
    return module_config_.enable_monitoring();
  }

 private:
  StackdriverOptions getStackdriverOptions();  

  config::StackdriverModule module_config_;

  int64_t current_tick_ms_ = 0;

  bool need_flush_ = false;
};

class StackdriverContext : public Context {
 public:
  StackdriverContext(uint32_t id, RootContext* root) : Context(id, root) {}
  void onCreate() override;
  void onLog() override;

  FilterHeadersStatus onRequestHeaders() override;
  FilterDataStatus
  onRequestBody(size_t body_buffer_length, bool end_of_stream) override;
  FilterHeadersStatus onResponseHeaders() override;
  FilterDataStatus
  onResponseBody(size_t body_buffer_length, bool end_of_stream) override;

  StackdriverRootContext* getStackdriverRoot() { 
    RootContext* root = this->root();
    return static_cast<StackdriverRootContext*>(root);  
  }

 private:
  uint64_t start_time_ = 0;
  uint64_t received_bytes_ = 0;
  uint64_t sent_bytes_ = 0;
  double_t total_time_ms_ = 0;
};

static RegisterContextFactory register_StackdriverContext(CONTEXT_FACTORY(StackdriverContext), ROOT_FACTORY(StackdriverRootContext));

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