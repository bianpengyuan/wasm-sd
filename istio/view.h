#ifndef ISTIO_VIEW_H_
#define ISTIO_VIEW_H_

#include "istio/measure.h"
#include "opencensus/stats/stats.h"

namespace istio {
namespace view {

const absl::string_view
    kIstioServerRequestCount = "istio.io/service/server/request_count";
const absl::string_view
    kIstioServerRequestBytes = "istio.io/service/server/request_bytes";
const absl::string_view kIstioServerResponseBytes =
    "istio.io/service/server/response_bytes";
const absl::string_view kIstioServerResponseLatencies =
    "istio.io/service/server/response_latencies";

void InitServerRequestCountView() {
  const opencensus::stats::ViewDescriptor view_descriptor =
      opencensus::stats::ViewDescriptor()
          .set_name(kIstioServerRequestCount)
          .set_measure(istio::measure::kIstioServerRequestCount)
          .set_aggregation(opencensus::stats::Aggregation::Count());
  opencensus::stats::View view(view_descriptor);
  view_descriptor.RegisterForExport();
}

void InitServerRequestBytesView() {
  const auto view_descriptor =
      opencensus::stats::ViewDescriptor()
          .set_name(kIstioServerRequestBytes)
          .set_measure(istio::measure::kIstioServerRequestBytes)
          .set_aggregation(opencensus::stats::Aggregation::Distribution(
              opencensus::stats::BucketBoundaries::Exponential(20, 1, 2)));
  opencensus::stats::View view(view_descriptor);
  view_descriptor.RegisterForExport();
}

void InitServerResponseBytesView() {
  const opencensus::stats::ViewDescriptor view_descriptor =
      opencensus::stats::ViewDescriptor()
          .set_name(kIstioServerResponseBytes)
          .set_measure(istio::measure::kIstioServerResponseBytes)
          .set_aggregation(opencensus::stats::Aggregation::Distribution(
              opencensus::stats::BucketBoundaries::Exponential(20, 1, 2)));
  opencensus::stats::View view(view_descriptor);
  view_descriptor.RegisterForExport();
}

void InitServerResponseLatenciesView() {
  const opencensus::stats::ViewDescriptor view_descriptor =
      opencensus::stats::ViewDescriptor()
          .set_name(kIstioServerResponseLatencies)
          .set_measure(istio::measure::kIstioServerResponseLatencies)
          .set_aggregation(opencensus::stats::Aggregation::Distribution(
              opencensus::stats::BucketBoundaries::Exponential(20, 1, 2)));
  opencensus::stats::View view(view_descriptor);
  view_descriptor.RegisterForExport();
}


void InitViews() {
  InitServerRequestCountView();
  InitServerRequestBytesView();
  InitServerResponseBytesView();
  InitServerResponseLatenciesView();
}

}  // namespace view
}  // measure istio


#endif // ISTIO_VIEW_H_