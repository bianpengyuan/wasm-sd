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
#include "include/stackdriver.h"
#include "google/protobuf/empty.pb.h"
#include "google/monitoring/v3/metric_service.pb.h"
#include "opencensus/exporters/stats/stackdriver/internal/stackdriver_utils.h"
#include "opencensus/stats/stats.h"

namespace opencensus {
namespace exporters {
namespace stats {

namespace {

constexpr char kGoogleStackdriverStatsAddress[] = "monitoring.googleapis.com";
constexpr char kProjectIdPrefix[] = "projects/";
// Stackdriver limits a single CreateTimeSeries request to 200 series.
constexpr int kTimeSeriesBatchSize = 200;

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
  Context* context_;
//  const std::unique_ptr<google::monitoring::v3::MetricService::Stub> stub_;
  std::unordered_map<std::string, opencensus::stats::ViewDescriptor>
      registered_descriptors_;
};

Handler::Handler(const StackdriverOptions& opts)
    : opts_(opts),
      project_id_(absl::StrCat(kProjectIdPrefix, opts.project_id)) {
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

//  std::vector<grpc::Status> status(num_rpcs);
//  std::vector<grpc::ClientContext> ctx(num_rpcs);
//  // We can safely re-use an empty response--it is never updated.
//  google::protobuf::Empty response;
//  grpc::CompletionQueue cq;

  for (int64_t rpc_index = 0; rpc_index < num_rpcs; ++rpc_index) {
    auto request = google::monitoring::v3::CreateTimeSeriesRequest();
    request.set_name(project_id_);
    const int batch_end = std::min(static_cast<int64_t>(time_series.size()),
                                   (rpc_index + 1) * kTimeSeriesBatchSize);
    for (int i = rpc_index * kTimeSeriesBatchSize; i < batch_end; ++i) {
      *request.add_time_series() = time_series[i];
    }

    std::function<void(google::protobuf::Empty &&)> success_callback =
        [](google::protobuf::Empty&& value) { /* do nothing */; };
    std::function<void(GrpcStatus status, std::string_view error_message)> failure_callback =
        [](GrpcStatus status, std::string_view message) {
          logInfo(std::string("failure ") + std::to_string(static_cast<int>(status)) +
              std::string(message));
        };
    GrpcService grpc_service;
    grpc_service.mutable_envoy_grpc()->set_cluster_name("cluster");
    std::string grpc_service_string;
    grpc_service.SerializeToString(&grpc_service_string);
    context_->grpcSimpleCall(grpc_service_string, "service", "method", request, 1000, success_callback, failure_callback);

//    logInfo("export view data " + request.DebugString());
  }

//  cq.Shutdown();
//  void* tag;
//  bool ok;
//  while (cq.Next(&tag, &ok)) {
//    if (ok) {
//      const auto& s = status[(uintptr_t)tag];
//      if (!s.ok()) {
//        std::cerr << "CreateTimeSeries request failed: "
//                  << opencensus::common::ToString(s) << "\n";
//      }
//    }
//  }
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
