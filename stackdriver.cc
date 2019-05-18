// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "include/stackdriver.h"
#include "istio/measure.h"
#include "istio/view.h"
#include "istio/tag.h"
#include "logging/logger.h"
#include "opencensus/exporters/stats/stackdriver/stackdriver_exporter.h"

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
#endif

std::unique_ptr<Context> NewContext(uint32_t id) {
  return std::unique_ptr<Context>(new StackdriverContext(id));
}

const std::string kProjectName = "bpy-istio";
const int32_t kFlushIntervalMilliseconds = 5000;
const int32_t kExportTickCount = 1; // export every 5s * 6 = 30s

void StackdriverContext::onStart() {
  opencensus::exporters::stats::StackdriverOptions options;
  options.project_id = kProjectName;
  options.context = this;
  opencensus::exporters::stats::StackdriverExporter::Register(options);

  // initialize tags, measures and views
  istio::measure::RegisterMeasures();
  istio::view::InitViews();

  // initialize logger
  logging::Logger::Get()->Init(this);

  // start periodic flush
  proxy_setTickPeriodMilliseconds(kFlushIntervalMilliseconds);
}

void StackdriverContext::onCreate() {
  start_time_ = proxy_getCurrentTimeNanoseconds();
}

FilterHeadersStatus StackdriverContext::onRequestHeaders() {
  auto headers = getRequestHeaderPairs();
  // received_bytes_ += headers->size();
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
  // sent_bytes_ += headers->size();
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
      double_t(proxy_getCurrentTimeNanoseconds() - start_time_) / 1000000;
  opencensus::stats::Record({{istio::measure::ServerRequestCountMeasure(), 1},
                             {istio::measure::ServerRequestBytesMeasure(),
                              received_bytes_},
                             {istio::measure::ServerResponseBytesMeasure(),
                              sent_bytes_},
                             {istio::measure::ServerRequestLatencyMeasure(),
                              total_time_ms}},
                            {});

  logging::Logger::Get()->AddLogEntry({{istio::tag::RequestLatencyKey(),
                                        std::to_string(total_time_ms)}});
}

void StackdriverContext::onTick() {
  tick_counter_ = (tick_counter_ + 1) % kExportTickCount;
  if (opencensus::stats::Flush()) {
    need_flush_ = true;
  }
  if (tick_counter_ == kExportTickCount - 1 && need_flush_) {
    opencensus::stats::StatsExporter::ExportViewData();
    logging::Logger::Get()->Flush();
    logging::Logger::Get()->Export();
    need_flush_ = false;
  }
}

#ifdef NULL_PLUGIN
} // namespace Stackdriver
} // namespace Plugin
} // namespace Null
} // namespace Wasm
} // namespace Common
} // namespace Extensions
} // namespace Envoy
#endif
