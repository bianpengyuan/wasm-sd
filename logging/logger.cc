#include "logger.h"
#include "api/proxy_wasm_intrinsics.h"

namespace logging {

constexpr int kMaxRotationBytesSize = 4000000;
constexpr char kServerAccessLogName[] =
    "projects/bpy-istio/logs/server-accesslog-stackdriver";
constexpr char kGoogleStackdriverLoggingAddress[] = "logging.googleapis.com";
constexpr char kGoogleLoggingService[] = "google.logging.v2.LoggingServiceV2";
constexpr char kGoogleWriteLogEntriesMethod[] = "WriteLogEntries";
constexpr int kDefaultTimeoutMillisecond = 10000;


void FillWriteLogEntry(google::logging::v2::WriteLogEntriesRequest* request) {
  request->set_log_name(kServerAccessLogName);
  auto* monitored_resource = request->mutable_resource();
  /** this needs to be changed ***/
  monitored_resource->set_type("k8s_container");
  auto* monitored_resource_labels = monitored_resource->mutable_labels();
  (*monitored_resource_labels)["project_id"] = "bpy-istio";
  (*monitored_resource_labels)["location"] = "us-central1-a";
  (*monitored_resource_labels)["cluster_name"] = "test-cluster";
  (*monitored_resource_labels)["namespace_name"] = "test-namespace";
  (*monitored_resource_labels)["pod_name"] = "test-pod";
  (*monitored_resource_labels)["container_name"] = "test-container";
  /** this needs to be changed ***/
}

Logger* Logger::Get() {
  static Logger* global_logger = new Logger();
  return global_logger;
}

void Logger::Init(Context* context) {
  context_ = context;
}

Logger::Logger() {
  log_entries_request_ =
      std::make_shared<google::logging::v2::WriteLogEntriesRequest>();
  FillWriteLogEntry(log_entries_request_.get());
}

void Logger::AddLogEntry(const std::vector<std::pair<opencensus::tags::TagKey,
                                                     std::string>>& labels) {
  // create a new log entry
  auto* log_entries = log_entries_request_->mutable_entries();
  auto* new_entry = log_entries->Add();

  // Get current time and fill in timestamp
  auto current_time = getCurrentTimeNanoseconds();
  auto* timestamp = new_entry->mutable_timestamp();
  timestamp->set_seconds(current_time / 1000000000);
  timestamp->set_nanos(current_time % 1000000000);

  // Add labels
  auto* log_labels = new_entry->mutable_labels();
  for (const auto& label : labels) {
    (*log_labels)[label.first.name()] = label.second;
  }

  // clean buffer
  size_ += new_entry->ByteSizeLong();
  if (size_ > kMaxRotationBytesSize) {
    logInfo("size triggered flushing");
    Flush();
  }
}

void Logger::Flush() {
  logInfo("size is " + std::to_string(size_));
  std::shared_ptr<google::logging::v2::WriteLogEntriesRequest> cur =
      std::make_shared<google::logging::v2::WriteLogEntriesRequest>();
  FillWriteLogEntry(cur.get());
  log_entries_request_.swap(cur);
  request_queue_.emplace_back(std::move(cur));
  size_ = 0;
}

void Logger::Export() {
  std::function<void(google::protobuf::Empty&&)> success_callback =
      [](google::protobuf::Empty&& value) {
        logDebug("successfully sent out request");
      };
  std::function<void(GrpcStatus status, std::string_view error_message)>
      failure_callback =
      [](GrpcStatus status, std::string_view message) {
        logInfo(
            "logging api call error: "
                + std::to_string(static_cast<int>(status))
                + std::string(message));
      };
  GrpcService grpc_service;
  grpc_service.mutable_google_grpc()->set_target_uri(
      kGoogleStackdriverLoggingAddress);
  grpc_service.mutable_google_grpc()
      ->mutable_channel_credentials()
      ->mutable_ssl_credentials()
      ->mutable_root_certs()
      ->set_filename("/etc/ssl/certs/ca-certificates.crt");
  grpc_service.mutable_google_grpc()
      ->add_call_credentials()
      ->mutable_google_compute_engine();
  std::string grpc_service_string;
  grpc_service.SerializeToString(&grpc_service_string);

  int count = 0;
  for (const auto& req : request_queue_) {
    count += 1;
    logInfo("here is request number " + std::to_string(count));
    context_->grpcSimpleCall(grpc_service_string,
                             kGoogleLoggingService,
                             kGoogleWriteLogEntriesMethod,
                             *req,
                             kDefaultTimeoutMillisecond,
                             success_callback,
                             failure_callback);
  }
  request_queue_.clear();
}

}  // namespace logging