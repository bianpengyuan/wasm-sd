#include "logger.h"

#ifdef NULL_PLUGIN
using namespace Envoy::Extensions::Common::Wasm::Null::Plugin;
using namespace envoy::api::v2::core;

namespace Envoy {
namespace Extensions {
namespace Common {
namespace Wasm {
namespace Null {
namespace Plugin {
namespace Stackdriver {
#endif

namespace logging {

constexpr int kMaxRotationBytesSize = 4000000;
// Path to CA credentials, which used for TLS between stackdriver client and server.
constexpr char kCACertPath[] = "/etc/ssl/certs/ca-certificates.crt";
constexpr char kGoogleStackdriverLoggingAddress[] = "logging.googleapis.com";
constexpr char kGoogleLoggingService[] = "google.logging.v2.LoggingServiceV2";
constexpr char kGoogleWriteLogEntriesMethod[] = "WriteLogEntries";
constexpr int kDefaultTimeoutMillisecond = 10000;

Logger* Logger::Get() {
  static Logger* global_logger = new Logger();
  return global_logger;
}

void Logger::Init(const StackdriverOptions& opts) {
  GrpcService grpc_service;
  grpc_service.mutable_google_grpc()->set_target_uri(
    kGoogleStackdriverLoggingAddress);
  grpc_service.mutable_google_grpc()
      ->mutable_channel_credentials()
      ->mutable_ssl_credentials()
      ->mutable_root_certs()
      ->set_filename(kCACertPath);
  grpc_service.mutable_google_grpc()
      ->add_call_credentials()
      ->mutable_google_compute_engine();
  grpc_service.SerializeToString(&grpc_service_string_);

  success_callback_ =
      [](google::protobuf::Empty&&) {
        logDebug("successfully sent out request");
      };
  failure_callback_ =
      [](GrpcStatus status, StringView message) {
        logInfo(
            "logging api call error: "
                + std::to_string(static_cast<int>(status))
                + std::string(message));
      };

  // Precreate monitored resource which all time series should be attached to.
  monitored_resource_.set_type(opts.monitored_resource_type);
  for (const auto& label : opts.monitored_resource_labels) {
    (*monitored_resource_.mutable_labels())[label.first] = label.second;
  }

  context_ = opts.context;

  log_entries_request_ =
      std::make_unique<google::logging::v2::WriteLogEntriesRequest>();
  server_access_log_name_ = "projects/" + opts.project_id + "/logs/server-accesslog-stackdriver";
  log_entries_request_->set_log_name(server_access_log_name_);
  log_entries_request_->mutable_resource()->CopyFrom(monitored_resource_);
}

void Logger::AddLogEntry(const std::vector<std::pair<opencensus::tags::TagKey,
                                                     StringView>>& labels) {
  // create a new log entry
  auto* log_entries = log_entries_request_->mutable_entries();
  auto* new_entry = log_entries->Add();

  // Get current time and fill in timestamp
  auto current_time = proxy_getCurrentTimeNanoseconds();
  auto* timestamp = new_entry->mutable_timestamp();
  timestamp->set_seconds(current_time / 1000000000);
  timestamp->set_nanos(current_time % 1000000000);

  // Add labels
  auto* log_labels = new_entry->mutable_labels();
  for (const auto& label : labels) {
    (*log_labels)[label.first.name()] = std::string(label.second);
  }

  // clean buffer
  size_ += new_entry->ByteSizeLong();
  if (size_ > kMaxRotationBytesSize) {
    logDebug("size triggered flushing");
    Flush();
  }
}

void Logger::Flush() {
  if (size_ == 0) {
    return;
  }
  std::unique_ptr<google::logging::v2::WriteLogEntriesRequest> cur =
      std::make_unique<google::logging::v2::WriteLogEntriesRequest>();
  cur->set_log_name(server_access_log_name_);
  cur->mutable_resource()->CopyFrom(monitored_resource_);
  log_entries_request_.swap(cur);
  request_queue_.emplace_back(std::move(cur));
  size_ = 0;
}

void Logger::Export() {
  for (const auto& req : request_queue_) {
    context_->grpcSimpleCall(grpc_service_string_,
                             kGoogleLoggingService,
                             kGoogleWriteLogEntriesMethod,
                             *req,
                             kDefaultTimeoutMillisecond,
                             success_callback_,
                             failure_callback_);
  }
  request_queue_.clear();
}

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
