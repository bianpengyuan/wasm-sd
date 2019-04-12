// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "api/proxy_wasm_intrinsics.h"
#include "istio/measure.h"
#include "istio/view.h"
#include "opencensus/stats/recording.h"
#include "opencensus/stats/stats.h"
#include "opencensus/exporters/stats/stackdriver/stackdriver_exporter.h"

const std::string kProjectName = "bpy-istio";
const int32_t kFlushIntervalMilliseconds = 5000;

class StackdriverContext : public Context {
 public:
  explicit StackdriverContext(uint32_t id) : Context(id) {}
  void onStart() override;
  void onLog() override;
  void onTick() override;
 private:
};

std::unique_ptr<Context> Context::New(uint32_t id) {
  return std::unique_ptr<Context>(new StackdriverContext(id));
}

void StackdriverContext::onStart() {
  opencensus::exporters::stats::StackdriverOptions options;
  options.project_id = kProjectName;
  opencensus::exporters::stats::StackdriverExporter::Register(options);

  // initialize tags, measures and views
  istio::measure::RegisterMeasures();
  istio::view::InitViews();

  // start periodic flush
  proxy_setTickPeriodMilliseconds(kFlushIntervalMilliseconds);
}

void StackdriverContext::onLog() {
  opencensus::stats::Record({{istio::measure::ServerRequestCountMeasure(), 1}},
                            {});
}

void StackdriverContext::onTick() {
  opencensus::stats::Flush();
  opencensus::stats::StatsExporter::ExportViewData();
}
