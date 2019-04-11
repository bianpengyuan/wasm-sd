
// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "api/proxy_wasm_intrinsics.h"
#include "opencensus/stats/measure.h"
#include "opencensus/stats/recording.h"
#include "opencensus/stats/stats.h"
#include "opencensus/exporters/stats/stackdriver/stackdriver_exporter.h"

const absl::string_view kIstioRequestMeasureName = "istio.io/service/server/request_count_measure";
const absl::string_view kIstioRequestViewName = "istio.io/service/server/request_count";
const std::string kProjectName = "bpy-istio";

opencensus::stats::MeasureInt64 RequestCountMeasure() {
    absl::string_view descriptor = "number of request received by server";
    absl::string_view units = "1";
    static const opencensus::stats::MeasureInt64 request_count =
        opencensus::stats::MeasureInt64::Register(
            kIstioRequestMeasureName, descriptor, units);
    return request_count;
}

class ExampleContext : public Context {
 public:
    explicit ExampleContext(uint32_t id) : Context(id) {
      RequestCountMeasure();
      opencensus::exporters::stats::StackdriverOptions options;
      options.project_id = kProjectName;
      opencensus::exporters::stats::StackdriverExporter::Register(options);

      const opencensus::stats::ViewDescriptor server_request_view =
          opencensus::stats::ViewDescriptor()
              .set_name(kIstioRequestViewName)
              .set_measure(kIstioRequestMeasureName)
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
  opencensus::stats::Record({{RequestCountMeasure(), 1}}, {});
}

void ExampleContext::onTick() {
  opencensus::stats::Flush();
  opencensus::stats::StatsExporter::ExportViewData();
}
