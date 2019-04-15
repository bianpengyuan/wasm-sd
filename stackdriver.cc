// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "include/stackdriver.h"
#include "istio/measure.h"
#include "istio/view.h"
#include "opencensus/exporters/stats/stackdriver/stackdriver_exporter.h"

const std::string kProjectName = "bpy-istio";
const int32_t kFlushIntervalMilliseconds = 5000;

std::unique_ptr<Context> Context::New(uint32_t id) {
  return std::unique_ptr<Context>(new StackdriverContext(id));
}

void StackdriverContext::onStart() {
  opencensus::exporters::stats::StackdriverOptions options;
  options.project_id = kProjectName;
  options.context = this;
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
