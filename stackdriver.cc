// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>

#include "include/stackdriver.h"
#include "istio/measure.h"
#include "istio/view.h"
#include "istio/tag.h"
#include "logging/logger.h"
#include "opencensus/exporters/stats/stackdriver/stackdriver_exporter.h"

#ifndef NULL_PLUGIN
#include "api/wasm/cpp/proxy_wasm_intrinsics.h"
#else

#include "extensions/common/wasm/null/null.h"

namespace Envoy {
namespace Extensions {
namespace Common {
namespace Wasm {
namespace Null {
namespace Plugin {
namespace Stackdriver {
#endif

const int32_t kFlushIntervalMilliseconds = 5000;
const int32_t kExportTickCount = 6; // export every 5s * 6 = 30s

void StackdriverRootContext::onConfigure(std::unique_ptr<WasmData> configuration) {
  module_config_ = configuration->proto<config::StackdriverModule>();
}

void SetSharedData() {
  // set shared data to provide label values for telemetry data
  // Note this is only used for simulation and performance measurement.
  // request_protocol: Protocol of the request or connection (e.g. HTTP, gRPC, TCP). 
  // service_authentication_policy: Determines if Istio was used to secure communications between services and how. Currently supported values: `NONE`, `MUTUAL_TLS`. 
  // mesh_uid: Unique identifier for the mesh that is being monitored. 
  setSharedData("destination_service_name", "some-destination-service-name");
  setSharedData("destination_service_namespace", "some-destination-service-namespace");
  setSharedData("destination_port", "12345");
  setSharedData("source_principal", "some-source-principal");
  setSharedData("source_workload_name", "some-source-workload-name");
  setSharedData("source_workload_namespace", "some-workload-namespace");
  setSharedData("source_owner", "k8s://v1alpha1/app/v1/some-source-owner");
  setSharedData("destination_workload_name", "some-destination-workload-name");
  setSharedData("destination_workload_namespace", "some-destination-workload-namespace");
  setSharedData("destination_owner", "k8s://v1alpha1/app/v1/some-desination-owner");
}

void StackdriverRootContext::onStart() {
  /***Simulation Only***/
  SetSharedData();
  /***Simulation Only***/

  auto options = getStackdriverOptions();
  if (monitoringEnabled()) {
    opencensus::exporters::stats::stackdriver::StackdriverExporter::Register(options);

    // initialize tags, measures and views
    istio::measure::RegisterMeasures();
    istio::view::InitViews();
  }
  if (loggingEnabled()) {
    // initialize logger
    logging::Logger::Get()->Init(options);
  }

  if (monitoringEnabled() || loggingEnabled()) {
    // start periodic flush
    proxy_setTickPeriodMilliseconds(kFlushIntervalMilliseconds);
  }
}

void StackdriverContext::onCreate() {
  if (getStackdriverRoot()->monitoringEnabled() || getStackdriverRoot()->loggingEnabled()) {
    start_time_ = proxy_getCurrentTimeNanoseconds();
  }
}

FilterHeadersStatus StackdriverContext::onRequestHeaders() {
  if (getStackdriverRoot()->monitoringEnabled() || getStackdriverRoot()->loggingEnabled()) {
    auto headers = getRequestHeaderPairs();
    // received_bytes_ += headers->size();
  }
  return FilterHeadersStatus::Continue;
}

FilterDataStatus StackdriverContext::onRequestBody(size_t body_buffer_length,
                                                   bool end_of_stream) {
  if (getStackdriverRoot()->monitoringEnabled() || getStackdriverRoot()->loggingEnabled()) {
    if (end_of_stream) {
      received_bytes_ += body_buffer_length;
    }
  }
  return FilterDataStatus::Continue;
}

FilterHeadersStatus StackdriverContext::onResponseHeaders() {
  if (getStackdriverRoot()->monitoringEnabled() || getStackdriverRoot()->loggingEnabled()) {
    auto headers = getRequestHeaderPairs();
    // sent_bytes_ += headers->size();
  }
  return FilterHeadersStatus::Continue;
}

FilterDataStatus StackdriverContext::onResponseBody(size_t body_buffer_length,
                                                    bool end_of_stream) {
  if (getStackdriverRoot()->monitoringEnabled() || getStackdriverRoot()->loggingEnabled()) {
    if (end_of_stream) {
      sent_bytes_ += body_buffer_length;
    }
  }
  return FilterDataStatus::Continue;
}

void StackdriverContext::onLog() {
  if (!getStackdriverRoot()->monitoringEnabled() && !getStackdriverRoot()->loggingEnabled()) {
    return;
  }

  total_time_ms_ =
      double_t(proxy_getCurrentTimeNanoseconds() - start_time_) / 1000000;
  auto destination_service_name = getSharedData("destination_service_name");
  auto destination_service_namespace = getSharedData("destination_service_namespace");
  auto destination_port = getSharedData("destination_port");
  auto source_principal = getSharedData("source_principal");
  auto source_workload_name = getSharedData("source_workload_name");
  auto source_workload_namespace = getSharedData("source_workload_namespace");
  auto source_owner = getSharedData("source_owner");
  auto destination_workload_name = getSharedData("destination_workload_name");
  auto destination_workload_namespace = getSharedData("destination_workload_namespace");
  auto destination_owner = getSharedData("destination_owner");

  if (getStackdriverRoot()->monitoringEnabled()) {
    opencensus::stats::Record({{istio::measure::ServerRequestCountMeasure(), 1},
                              {istio::measure::ServerRequestBytesMeasure(),
                                received_bytes_},
                              {istio::measure::ServerResponseBytesMeasure(),
                                sent_bytes_},
                              {istio::measure::ServerRequestLatencyMeasure(),
                                total_time_ms_}},
                              {{istio::tag::RequestProtocolKey(), "http"},
                               {istio::tag::MeshUIDKey(), "thisisaservicemesh"},
                               {istio::tag::DestinationServiceNameKey(), destination_service_name->view()},
                               {istio::tag::DestinationServiceNamespaceKey(), destination_service_namespace->view()},
                               {istio::tag::DestinationPortKey(), destination_port->view()},
                               {istio::tag::SourcePrincipalKey(), source_principal->view()},
                               {istio::tag::SourceWorkloadNameKey(), source_workload_name->view()},
                               {istio::tag::SourceWorkloadNamespaceKey(), source_workload_namespace->view()},
                               {istio::tag::SourceOwnerKey(), source_owner->view()},
                               {istio::tag::DestinationWorkloadNameKey(), destination_workload_name->view()},
                               {istio::tag::DestinationWorkloadNamespaceKey(), destination_workload_namespace->view()},
                               {istio::tag::DestinationOwnerKey(), destination_owner->view()}});
  }

  if (getStackdriverRoot()->loggingEnabled()) {
    logging::Logger::Get()->AddLogEntry({{istio::tag::RequestProtocolKey(), "http"},
                                         {istio::tag::MeshUIDKey(), "thisisaservicemesh"},
                                         {istio::tag::DestinationServiceNameKey(), destination_service_name->view()},
                                         {istio::tag::DestinationServiceNamespaceKey(), destination_service_namespace->view()},
                                         {istio::tag::DestinationPortKey(), destination_port->view()},
                                         {istio::tag::SourcePrincipalKey(), source_principal->view()},
                                         {istio::tag::SourceWorkloadNameKey(), source_workload_name->view()},
                                         {istio::tag::SourceWorkloadNamespaceKey(), source_workload_namespace->view()},
                                         {istio::tag::SourceOwnerKey(), source_owner->view()},
                                         {istio::tag::DestinationWorkloadNameKey(), destination_workload_name->view()},
                                         {istio::tag::DestinationWorkloadNamespaceKey(), destination_workload_namespace->view()},
                                         {istio::tag::DestinationOwnerKey(), destination_owner->view()}});
  }
}

void StackdriverRootContext::onTick() {
  tick_counter_ = (tick_counter_ + 1) % kExportTickCount;

  if (monitoringEnabled()) {
    if (opencensus::stats::Flush()) {
      need_flush_ = true;
    }
    if (tick_counter_ == kExportTickCount - 1 && need_flush_) {
      opencensus::stats::StatsExporter::ExportViewData();
      need_flush_ = false;
    }
  }

  if (loggingEnabled()) {
    logging::Logger::Get()->Flush();
    logging::Logger::Get()->Export();
  }
}

StackdriverOptions StackdriverRootContext::getStackdriverOptions() {
  StackdriverOptions options;
  options.context = asRoot();
  options.project_id = module_config_.project_id();
  options.monitored_resource_type = module_config_.monitored_resource_type();
  for (auto& p : options.monitored_resource_labels) {
    options.monitored_resource_labels[p.first] = p.second;
  }
  return options;
}

#ifdef NULL_PLUGIN
} // namespace Stackdriver
} // namespace Plugin
} // namespace Null
} // namespace Wasm
} // namespace Common
} // namespace Extensions
} // namespace Envoy
#endif
