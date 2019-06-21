#ifndef WASM_SD_LOGGING_LOGGING_H
#define WASM_SD_LOGGING_LOGGING_H

#include <string>
#include <vector>

#include "google/logging/v2/log_entry.pb.h"
#include "google/logging/v2/logging.pb.h"
#include "opencensus/tags/tag_key.h"
#include "include/stackdriver_exporter_options.h"

#ifndef NULL_PLUGIN
#include "api/wasm/cpp/proxy_wasm_intrinsics.h"
#else
#include "extensions/common/wasm/null/null.h"
using namespace Envoy::Extensions::Common::Wasm::Null::Plugin;

namespace Envoy {
namespace Extensions {
namespace Common {
namespace Wasm {
namespace Null {
namespace Plugin {
namespace Stackdriver {
#endif

namespace logging {

class Logger {
 public:
  static Logger* Get();
  void Init(const StackdriverOptions& options);

  void AddLogEntry(const std::vector<std::pair<opencensus::tags::TagKey,
                                               StringView>>& labels);

  void Flush();
  void Export();

 private:
  Logger() {}

  std::unique_ptr<google::logging::v2::WriteLogEntriesRequest>
      log_entries_request_;

  int size_ = 0;

  std::vector<std::shared_ptr<google::logging::v2::WriteLogEntriesRequest>>
      request_queue_;

  std::string server_access_log_name_;

  ::google::api::MonitoredResource monitored_resource_;
  std::string grpc_service_string_;
  std::function<void(google::protobuf::Empty &&)> success_callback_;
  std::function<void(GrpcStatus, StringView)> failure_callback_;
  RootContext* context_ = nullptr;
};

}  // namespace logging

#ifdef NULL_PLUGIN
} // namespace Stackdriver
} // namespace Plugin
} // namespace Null
} // namespace Wasm
} // namespace Common
} // namespace Extensions
} // namespace Envoy
#endif

#endif //WASM_SD_LOGGING_LOGGING_H
