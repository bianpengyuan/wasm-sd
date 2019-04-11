
// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "api/proxy_wasm_intrinsics.h"
#include "opencensus/stats/measure.h"
#include "google/monitoring/v3/metric.pb.h"
#include "opencensus/stats/recording.h"
#include "opencensus/stats/stats.h"
#include "opencensus/exporters/stats/stackdriver/stackdriver_exporter.h"

class ExampleContext : public Context {
 public:
    explicit ExampleContext(uint32_t id) : Context(id) {
    }
    void onStart() override;
    void onLog() override;
 private:
};

std::unique_ptr<Context> Context::New(uint32_t id) {
    return std::unique_ptr<Context>(new ExampleContext(id));
}

void ExampleContext::onStart() {
}


void ExampleContext::onLog() {
  std::vector<google::monitoring::v3::TimeSeries> time_series;
  logInfo("length: " + std::to_string(time_series.size()));

  auto base_time_series = google::monitoring::v3::TimeSeries();

  /* comment the following line, things are working fine */
  base_time_series.mutable_metric()->set_type("abc");

  // even I left the upper line uncommented, and comment the following line, so
  // that base_time_series is not pushed crash still happens in the last size()
  // line.
  time_series.push_back(base_time_series);
  logInfo("length: " + std::to_string(time_series.size()));
}
