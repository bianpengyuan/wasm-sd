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
#include "google/monitoring/v3/metric_service.pb.h"
#include "google/protobuf/empty.pb.h"
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
namespace stackdriver {

namespace {
// Grpc service name and method name.
constexpr char kGoogleStackdriverStatsAddress[] = "monitoring.googleapis.com";
constexpr char kGoogleMonitoringService[] =
    "google.monitoring.v3.MetricService";
constexpr char kGoogleCreateTimeSeriesMethod[] = "CreateTimeSeries";

// Path to CA credentials, which used for TLS between stackdriver client and server.
constexpr char kCACertPath[] = "/etc/ssl/certs/ca-certificates.crt";

// Default time out for create timeseries rpc call.
constexpr int kDefaultTimeoutMillisecond = 10000;

// Default time series batch size limit of CreateTimeSeries method.
constexpr int kTimeSeriesBatchSize = 200;

constexpr char kNamePrefix[] = "projects/";

class Handler : public ::opencensus::stats::StatsExporter::Handler {
 public:
  explicit Handler(const StackdriverOptions& opts);

  void ExportViewData(
      const std::vector<std::pair<opencensus::stats::ViewDescriptor,
                                  opencensus::stats::ViewData>>& data) override;

 private:
  ::google::api::MonitoredResource monitored_resource_;
  std::string timeseries_req_name_;
  std::string grpc_service_string_;
  std::function<void(google::protobuf::Empty &&)> success_callback_;
  std::function<void(GrpcStatus, StringView)> failure_callback_;
  RootContext* context_ = nullptr;
};

Handler::Handler(const StackdriverOptions& opts)
    : timeseries_req_name_(absl::StrCat(kNamePrefix, opts.project_id)),
      context_(opts.context) {
  // GrpcService is to configure the client to stackdriver.
  GrpcService grpc_service;
  grpc_service.mutable_google_grpc()->set_target_uri(
    kGoogleStackdriverStatsAddress);
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
    [](google::protobuf::Empty&& /* value */) {
      logDebug("successfully sent out create timeseries request");
    };
  failure_callback_ = [](GrpcStatus status, StringView message) {
      // TODO(bianpengyuan): add retry logic on failure
      logWarn(std::string("failed to send timeseries ") +
              std::to_string(static_cast<int>(status)) + " " +
              std::string(message));
    };

  // Precreate monitored resource which all time series should be attached to.
  monitored_resource_.set_type(opts.monitored_resource_type);
  for (const auto& label : opts.monitored_resource_labels) {
    (*monitored_resource_.mutable_labels())[label.first] = label.second;
  }
}

void Handler::ExportViewData(
    const std::vector<std::pair<opencensus::stats::ViewDescriptor,
                                opencensus::stats::ViewData>>& data) {
  logInfo("Start export!!!!!!!!!!!!!!!");
  std::vector<google::monitoring::v3::TimeSeries> time_series;
  for (const auto& datum : data) {
    const auto view_time_series =
        MakeTimeSeries(datum.first, datum.second, monitored_resource_);
    time_series.insert(time_series.end(), view_time_series.begin(),
                       view_time_series.end());
  }

  const int64_t num_rpcs =
      ceil(static_cast<double>(time_series.size()) / kTimeSeriesBatchSize);

  for (int64_t rpc_index = 0; rpc_index < num_rpcs; ++rpc_index) {
    auto request = google::monitoring::v3::CreateTimeSeriesRequest();
    request.set_name(timeseries_req_name_);
    const int batch_end = std::min(static_cast<int64_t>(time_series.size()),
                                   (rpc_index + 1) * kTimeSeriesBatchSize);
    for (int i = rpc_index * kTimeSeriesBatchSize; i < batch_end; ++i) {
      *request.add_time_series() = time_series[i];
    }

    if (context_ != nullptr) {
      context_->grpcSimpleCall(grpc_service_string_, kGoogleMonitoringService,
                              kGoogleCreateTimeSeriesMethod, request,
                              kDefaultTimeoutMillisecond, success_callback_,
                              failure_callback_);
    }
  }
  logInfo("Finish export!!!!!!!!!!!!!!!");
}

}  // namespace

// static
void StackdriverExporter::Register(const StackdriverOptions& opts) {
  opencensus::stats::StatsExporter::RegisterPushHandler(
      absl::WrapUnique(new Handler(opts)));
}

}  // namespace stackdriver
}  // namespace stats
}  // namespace exporters
}  // namespace opencensus
