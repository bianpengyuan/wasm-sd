// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "include/stackdriver.h"
#include "istio/measure.h"
#include "istio/view.h"
#include "opencensus/exporters/stats/stackdriver/stackdriver_exporter.h"

const std::string kProjectName = "bpy-istio";
const int32_t kFlushIntervalMilliseconds = 5000;
const int32_t kExportTickCount = 12; // export every 5s * 12 = 60s

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

void StackdriverContext::onCreate() {
  start_time_ = getCurrentTimeNanoseconds();
}

FilterHeadersStatus StackdriverContext::onRequestHeaders() {
  auto headers = getRequestHeaderPairs();
  received_bytes_ += headers->size();
  return FilterHeadersStatus::Continue;
}

FilterDataStatus StackdriverContext::onRequestBody(size_t body_buffer_length,
                                                   bool end_of_stream) {
  if (end_of_stream) {
    received_bytes_ += body_buffer_length;
  }
  return FilterDataStatus::Continue;
}

FilterHeadersStatus StackdriverContext::onResponseHeaders() {
  auto headers = getRequestHeaderPairs();
  sent_bytes_ += headers->size();
  return FilterHeadersStatus::Continue;
}

FilterDataStatus StackdriverContext::onResponseBody(size_t body_buffer_length,
                                                    bool end_of_stream) {
  if (end_of_stream) {
    sent_bytes_ += body_buffer_length;
  }
  return FilterDataStatus::Continue;
}

void StackdriverContext::onLog() {
  double_t
      total_time_ms =
      double_t(getCurrentTimeNanoseconds() - start_time_) / 1000000;
  opencensus::stats::Record({{istio::measure::ServerRequestCountMeasure(), 1},
                             {istio::measure::ServerRequestBytesMeasure(),
                              received_bytes_},
                             {istio::measure::ServerResponseBytesMeasure(),
                              sent_bytes_},
                             {istio::measure::ServerRequestLatencyMeasure(),
                              total_time_ms}},
                            {});
}

void StackdriverContext::onTick() {
  tick_counter_ = (tick_counter_ + 1) % kExportTickCount;
  if (opencensus::stats::Flush()) {
    need_flush_ = true;
  }
  if (tick_counter_ == kExportTickCount - 1 && need_flush_) {
    opencensus::stats::StatsExporter::ExportViewData();
    need_flush_ = false;
  }
}
