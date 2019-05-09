#ifndef WASM_SD_LOGGING_LOGGING_H
#define WASM_SD_LOGGING_LOGGING_H

#include <string>
#include <vector>

#include "google/logging/v2/log_entry.pb.h"
#include "google/logging/v2/logging.pb.h"
#include "api/proxy_wasm_intrinsics.h"
#include "opencensus/tags/tag_key.h"

namespace logging {

class Logger {
 public:
  static Logger* Get();
  void Init(Context* context);

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

  Context* context_;
};

}  // namespace logging

#endif //WASM_SD_LOGGING_LOGGING_H
