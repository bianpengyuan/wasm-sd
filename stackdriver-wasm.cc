
// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "api/proxy_wasm_intrinsics.h"
#include "stats/measure.h"
#include "stats/stats.h"

const absl::string_view kIstioRequestMeasureName = "istio.io/service/server/request_count_measure";
stats::MeasureInt64 RequestCountMeasure() {
    absl::string_view descriptor = "number of request received by server";
    absl::string_view units = "1";
    static const stats::MeasureInt64 request_count =
        stats::MeasureInt64::Register(
            kIstioRequestMeasureName, descriptor, units);
    return request_count;
}

class ExampleContext : public Context {
public:
    explicit ExampleContext(uint32_t id) : Context(id) {
      RequestCountMeasure();
    }
    void onStart() override;
    void onLog() override;
    void onTick() override;
private:
};

std::unique_ptr<Context> Context::New(uint32_t id) {
    return std::unique_ptr<Context>(new ExampleContext(id));
}

void ExampleContext::onStart() {
  proxy_setTickPeriodMilliseconds(5000);
}

void ExampleContext::onLog() {
  stats::Record({{RequestCountMeasure(), 1}}, {});
}

void ExampleContext::onTick() {
  stats::Flush();
}
