#include "extensions/common/wasm/null/null.h"
#include "include/stackdriver.h"

namespace Envoy {
namespace Extensions {
namespace Common {
namespace Wasm {
namespace Null {
namespace Plugin {
namespace Stackdriver {

std::unique_ptr<Plugin::Context> NewContext(uint32_t id);

/**
 * Config registration for a Wasm filter plugin. @see NamedHttpFilterConfigFactory.
 */
class StackdriverPluginFactory : public NullVmPluginFactory {
public:
  StackdriverPluginFactory() {}

  const std::string name() const override { return "stackdriver_plugin"; }
  std::unique_ptr<NullVmPlugin> create() const override {
    return std::make_unique<NullVmPlugin>(
        &Envoy::Extensions::Common::Wasm::Null::Plugin::Stackdriver::NewContext);
  }

private:
  std::string name_;
};

/**
 * Static registration for the null Wasm filter. @see RegisterFactory.
 */
static Registry::RegisterFactory<StackdriverPluginFactory, NullVmPluginFactory> register_;

} // namespace Stackdriver
} // namespace Plugin
} // namespace Null
} // namespace Wasm
} // namespace Common
} // namespace Extensions
} // namespace Envoy
