#ifndef WASM_SD_LOGGING_LOGGING_H
#define WASM_SD_LOGGING_LOGGING_H

#include <string>
#include <vector>

#include "google/logging/v2/log_entry.pb.h"
#include "google/logging/v2/logging.pb.h"
#include "opencensus/tags/tag_key.h"

#ifndef NULL_PLUGIN
#include "api/wasm/cpp/proxy_wasm_intrinsics.h"
#else
#include "extensions/common/wasm/null/null.h"
using namespace Envoy::Extensions::Common::Wasm::Null::Plugin;
#endif

namespace logging {

class Logger {
 public:
  static Logger* Get();
  void Init(RootContext* context);

  void AddLogEntry(const std::vector<std::pair<opencensus::tags::TagKey,
                                               std::string>>& labels);

  void Flush();
  void Export();

 private:
  Logger();

  std::shared_ptr<google::logging::v2::WriteLogEntriesRequest>
      log_entries_request_;

  int size_ = 0;

  std::vector<std::shared_ptr<google::logging::v2::WriteLogEntriesRequest>>
      request_queue_;

  RootContext* context_;
};

}  // namespace logging

#endif //WASM_SD_LOGGING_LOGGING_H
