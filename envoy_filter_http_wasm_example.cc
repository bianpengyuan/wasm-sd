
// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"
#include "opencensus/exporters/stats/stackdriver/stackdriver_exporter.h"
#include "opencensus/stats/view_descriptor.h"
#include "opencensus/stats/stats.h"

ABSL_CONST_INIT const char kLettersMeasureName[] = "example.org/measure/letters";

class ExampleContext : public Context {
public:
    explicit ExampleContext(uint32_t id) : Context(id) {
        LettersMeasure();
        const opencensus::stats::ViewDescriptor letters_view =
                opencensus::stats::ViewDescriptor()
                        .set_name("example.org/view/letters_view")
                        .set_description("number of letters in names greeted over time")
                        .set_measure(kLettersMeasureName)
                        .set_aggregation(opencensus::stats::Aggregation::Sum())
                        .add_column(CaseKey());
        opencensus::stats::View view(letters_view);
        assert(view.IsValid());
        letters_view.RegisterForExport();
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
    opencensus::stats::MeasureInt64 LettersMeasure() {
        static const opencensus::stats::MeasureInt64 measure =
                opencensus::stats::MeasureInt64::Register(
                        kLettersMeasureName, "Number of letters in processed names.", "By");
        return measure;
    }

    opencensus::tags::TagKey CaseKey() {
        static const opencensus::tags::TagKey key =
                opencensus::tags::TagKey::Register("example_uppercased");
        return key;
    }
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

//

//
//class ExampleContext : public Context {
//  public:
//    explicit ExampleContext(uint32_t id) : Context(id) {
//        LettersMeasure();
//        RegisterStackdriverExporters();
//        const opencensus::stats::ViewDescriptor letters_view =
//                opencensus::stats::ViewDescriptor()
//                        .set_name("example.org/view/letters_view")
//                        .set_description("number of letters in names greeted over time")
//                        .set_measure(kLettersMeasureName)
//                        .set_aggregation(opencensus::stats::Aggregation::Sum())
//                        .add_column(CaseKey());
//        opencensus::stats::View view(letters_view);
//        assert(view.IsValid());
//        letters_view.RegisterForExport();
//    }
//    void onStart() override;
//    FilterHeadersStatus onRequestHeaders() override;
//    FilterDataStatus onRequestBody(size_t body_buffer_length, bool end_of_stream) override;
//    FilterHeadersStatus onResponseHeaders() override;
//    void onDestroy() override;
//  private:
//    opencensus::stats::MeasureInt64 LettersMeasure() {
//        static const opencensus::stats::MeasureInt64 measure =
//                opencensus::stats::MeasureInt64::Register(
//                        kLettersMeasureName, "Number of letters in processed names.", "By");
//        return measure;
//    }
//
//    opencensus::tags::TagKey CaseKey() {
//        static const opencensus::tags::TagKey key =
//                opencensus::tags::TagKey::Register("example_uppercased");
//        return key;
//    }
//
//    void RegisterStackdriverExporters() {
//        const char *project_id = "bpy-istio";
//        if (project_id == nullptr) {
//            std::cerr << "The STACKDRIVER_PROJECT_ID environment variable is not set: "
//                    "not exporting to Stackdriver.\n";
//            return;
//        }
//
//        opencensus::exporters::stats::StackdriverOptions stats_opts;
//        stats_opts.project_id = project_id;
//        stats_opts.opencensus_task = absl::StrCat("cpp-", getpid(), "@host");
//
//        std::cout << "RegisterStackdriverExporters:\n";
//        std::cout << "  project_id = \"" << stats_opts.project_id << "\"\n";
//        std::cout << "  opencensus_task = \"" << stats_opts.opencensus_task << "\"\n";
//        opencensus::exporters::stats::StackdriverExporter::Register(stats_opts);
//    }
//};
//
//std::unique_ptr<Context> Context::New(uint32_t id) {
//  return std::unique_ptr<Context>(new ExampleContext(id));
//}
//
//void ExampleContext::onStart() {
//  logTrace("main");
//}
//
//FilterHeadersStatus ExampleContext::onRequestHeaders() {
//  logDebug(std::string("onRequestHaders ") + std::to_string(id()));
//  auto result = getRequestHeaderPairs();
//  auto pairs = result->pairs();
//  logInfo(std::string("headers: ") + std::to_string(pairs.size()));
//  for (auto& p : pairs) {
//    logInfo(std::string(p.first) + std::string(" -> ") + std::string(p.second));
//  }
//  return FilterHeadersStatus::Continue;
//}
//
//FilterHeadersStatus ExampleContext::onResponseHeaders() {
//  logDebug(std::string("onResponseHaders ") + std::to_string(id()));
//  auto result = getResponseHeaderPairs();
//  auto pairs = result->pairs();
//  logInfo(std::string("headers: ") + std::to_string(pairs.size()));
//  for (auto& p : pairs) {
//    logInfo(std::string(p.first) + std::string(" -> ") + std::string(p.second));
//  }
//  addResponseHeader("newheader", "newheadervalue");
//  replaceResponseHeader("location", "envoy-wasm");
//
//  api::HandleMetricRequest req;
//
//  return FilterHeadersStatus::Continue;
//}
//
//FilterDataStatus ExampleContext::onRequestBody(size_t body_buffer_length, bool end_of_stream) {
//  auto body = getRequestBodyBufferBytes(0, body_buffer_length);
//  logError(std::string("onRequestBody ") + std::string(body->view()));
//  return FilterDataStatus::Continue;
//}
//
//void ExampleContext::onDestroy() {
//  logWarn(std::string("onDestroy " + std::to_string(id())));
//}
