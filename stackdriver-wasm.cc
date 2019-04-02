
// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "api/proxy_wasm_intrinsics.h"
#include "stats/measure.h"
#include "stats/stats.h"

const absl::string_view kIstioRequestMeasureName = "istio.io/service/server/request_count_measure";

class ExampleContext : public Context {
public:
    explicit ExampleContext(uint32_t id) : Context(id) {
      absl::string_view descriptor = "number of request received by server";
      absl::string_view units = "1";
      static const wasmsd::stats::MeasureInt64 server_request_count =
          wasmsd::stats::MeasureInt64::Register(kIstioRequestMeasureName, descriptor, units);
    }

    void onStart() override;

    void onCreate() override;
    FilterHeadersStatus onRequestHeaders() override;
    FilterDataStatus onRequestBody(size_t body_buffer_length, bool end_of_stream) override;
    FilterHeadersStatus onResponseHeaders() override;
    void onDone() override;
    void onLog() override;
    void onDelete() override;
private:
};

std::unique_ptr<Context> Context::New(uint32_t id) {
    return std::unique_ptr<Context>(new ExampleContext(id));
}

void ExampleContext::onStart() { logTrace("onStart"); }

void ExampleContext::onCreate() { logWarn(std::string("onCreate " + std::to_string(id()))); }

FilterHeadersStatus ExampleContext::onRequestHeaders() {
    logDebug(std::string("onRequestHeaders ") + std::to_string(id()));
    auto result = getRequestHeaderPairs();
    auto pairs = result->pairs();
    logInfo(std::string("headers: ") + std::to_string(pairs.size()));
    for (auto& p : pairs) {
        logInfo(std::string(p.first) + std::string(" -> ") + std::string(p.second));
    }
    return FilterHeadersStatus::Continue;
}

FilterHeadersStatus ExampleContext::onResponseHeaders() {
    logDebug(std::string("onResponseHeaders ") + std::to_string(id()));
    auto result = getResponseHeaderPairs();
    auto pairs = result->pairs();
    logInfo(std::string("headers: ") + std::to_string(pairs.size()));
    for (auto& p : pairs) {
        logInfo(std::string(p.first) + std::string(" -> ") + std::string(p.second));
    }
    addResponseHeader("newheader", "newheadervalue");
    replaceResponseHeader("location", "envoy-wasm");
    return FilterHeadersStatus::Continue;
}

FilterDataStatus ExampleContext::onRequestBody(size_t body_buffer_length, bool end_of_stream) {
    auto body = getRequestBodyBufferBytes(0, body_buffer_length);
    logError(std::string("onRequestBody ") + std::string(body->view()));
    return FilterDataStatus::Continue;
}

void ExampleContext::onDone() { logWarn(std::string("onDone " + std::to_string(id()))); }

void ExampleContext::onLog() { logWarn(std::string("onLog " + std::to_string(id()))); }

void ExampleContext::onDelete() { logWarn(std::string("onDelete " + std::to_string(id()))); }