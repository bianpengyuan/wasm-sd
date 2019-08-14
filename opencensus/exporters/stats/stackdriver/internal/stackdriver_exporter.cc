// Copyright 2018, OpenCensus Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "opencensus/exporters/stats/stackdriver/stackdriver_exporter.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

#include "absl/memory/memory.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "google/protobuf/empty.pb.h"
#include "google/monitoring/v3/metric_service.pb.h"
#include "opencensus/exporters/stats/stackdriver/internal/stackdriver_utils.h"
#include "opencensus/stats/stats.h"

#ifndef NULL_PLUGIN
#include "api/wasm/cpp/proxy_wasm_intrinsics.h"
#else
#include "extensions/common/wasm/null/null.h"
using namespace Envoy::Extensions::Common::Wasm::Null::Plugin;
using namespace envoy::api::v2::core;
#endif

namespace opencensus {
namespace exporters {
namespace stats {

namespace {

constexpr char kGoogleStackdriverStatsAddress[] = "monitoring.googleapis.com";
constexpr char kProjectIdPrefix[] = "projects/";
// Stackdriver limits a single CreateTimeSeries request to 200 series.
constexpr int kTimeSeriesBatchSize = 200;
constexpr char
    kGoogleMonitoringService[] = "google.monitoring.v3.MetricService";
constexpr char kGoogleCreateTimeSeriesMethod[] = "CreateTimeSeries";
constexpr int kDefaultTimeoutMillisecond = 10000;

class Handler : public ::opencensus::stats::StatsExporter::Handler {
 public:
  explicit Handler(const StackdriverOptions& opts);

  void ExportViewData(
      const std::vector<std::pair<opencensus::stats::ViewDescriptor,
                                  opencensus::stats::ViewData>>& data) override;

 private:
  // Registers 'descriptor' with Stackdriver if no view by that name has been
  // registered by this, and adds it to registered_descriptors_ if successful.
  // Returns true if the view has already been registered or registration is
  // successful, and false if the registration fails or the name has already
  // been registered with different parameters.
  bool MaybeRegisterView(const opencensus::stats::ViewDescriptor& descriptor);

  const StackdriverOptions opts_;
  const std::string project_id_;
  RootContext* context_ = nullptr;
//  const std::unique_ptr<google::monitoring::v3::MetricService::Stub> stub_;
  std::unordered_map<std::string, opencensus::stats::ViewDescriptor>
      registered_descriptors_;
};

Handler::Handler(const StackdriverOptions& opts)
    : opts_(opts),
      project_id_(absl::StrCat(kProjectIdPrefix, opts.project_id)),
      context_(opts.context) {
//      stub_(google::monitoring::v3::MetricService::NewStub(
//          ::grpc::CreateCustomChannel(kGoogleStackdriverStatsAddress,
//                                      ::grpc::GoogleDefaultCredentials(),
//                                      ::opencensus::common::WithUserAgent()))) {
}

void Handler::ExportViewData(
    const std::vector<std::pair<opencensus::stats::ViewDescriptor,
                                opencensus::stats::ViewData>>& data) {
  std::vector<google::monitoring::v3::TimeSeries> time_series;
  for (const auto& datum : data) {
    if (!MaybeRegisterView(datum.first)) {
      continue;
    }
    const auto view_time_series =
        MakeTimeSeries(datum.first, datum.second, opts_.opencensus_task);
    time_series.insert(time_series.end(), view_time_series.begin(),
                       view_time_series.end());
  }

  const int64_t num_rpcs =
      ceil(static_cast<double>(time_series.size()) / kTimeSeriesBatchSize);

  std::function<void(google::protobuf::Empty&&)> success_callback =
      [](google::protobuf::Empty&& /* value */) {
        logDebug("successfully sent out request");
      };
  std::function<void(GrpcStatus status, std::string_view error_message)>
      failure_callback =
      [](GrpcStatus status, std::string_view message) {
        logInfo(
            std::string("failure ") + std::to_string(static_cast<int>(status))
                + std::string(message));
      };
  GrpcService grpc_service;
  grpc_service.mutable_google_grpc()->set_target_uri(
      kGoogleStackdriverStatsAddress);
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

  for (int64_t rpc_index = 0; rpc_index < num_rpcs; ++rpc_index) {
    auto request = google::monitoring::v3::CreateTimeSeriesRequest();
    request.set_name(project_id_);
    const int batch_end = std::min(static_cast<int64_t>(time_series.size()),
                                   (rpc_index + 1) * kTimeSeriesBatchSize);
    for (int i = rpc_index * kTimeSeriesBatchSize; i < batch_end; ++i) {
      *request.add_time_series() = time_series[i];
    }

    context_->grpcSimpleCall(grpc_service_string,
                             kGoogleMonitoringService,
                             kGoogleCreateTimeSeriesMethod,
                             request,
                             kDefaultTimeoutMillisecond,
                             success_callback,
                             failure_callback);
  }
}

bool Handler::MaybeRegisterView(
    const opencensus::stats::ViewDescriptor& descriptor) {
  const auto& it = registered_descriptors_.find(descriptor.name());
  if (it != registered_descriptors_.end()) {
    if (it->second != descriptor) {
//      std::cerr << "Not exporting altered view: " << descriptor.DebugString()
//                << "\nAlready registered as: " << it->second.DebugString()
//                << "\n";
      return false;
    }
    return true;
  }

  auto request = google::monitoring::v3::CreateMetricDescriptorRequest();
  request.set_name(project_id_);
  SetMetricDescriptor(project_id_, descriptor,
                      request.mutable_metric_descriptor());
  registered_descriptors_.emplace_hint(it, descriptor.name(), descriptor);
  return true;
}

}  // namespace

// static
void StackdriverExporter::Register(const StackdriverOptions& opts) {
  opencensus::stats::StatsExporter::RegisterPushHandler(
      absl::WrapUnique(new Handler(opts)));
}

}  // namespace stats
}  // namespace exporters
}  // namespace opencensus
