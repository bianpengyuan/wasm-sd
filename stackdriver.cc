
// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "api/proxy_wasm_intrinsics.h"
#include "istio/measure.h"
#include "opencensus/stats/recording.h"
#include "opencensus/stats/stats.h"
#include "opencensus/exporters/stats/stackdriver/stackdriver_exporter.h"

const absl::string_view kIstioRequestViewName = "istio.io/service/server/request_count";
const std::string kProjectName = "bpy-istio";

class ExampleContext : public Context {
 public:
    explicit ExampleContext(uint32_t id) : Context(id) {
      istio::measure::ServerRequestCountMeasure();
      opencensus::exporters::stats::StackdriverOptions options;
      options.project_id = kProjectName;
      opencensus::exporters::stats::StackdriverExporter::Register(options);

      const opencensus::stats::ViewDescriptor server_request_view =
          opencensus::stats::ViewDescriptor()
              .set_name(kIstioRequestViewName)
              .set_measure(istio::measure::kIstioServerRequestCount)
              .set_aggregation(opencensus::stats::Aggregation::Count());
      opencensus::stats::View view(server_request_view);
      server_request_view.RegisterForExport();
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
  opencensus::stats::Record({{istio::measure::ServerRequestCountMeasure(), 1}}, {});
}

void ExampleContext::onTick() {
  opencensus::stats::Flush();
  opencensus::stats::StatsExporter::ExportViewData();
}
