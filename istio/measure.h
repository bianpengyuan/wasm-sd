#ifndef ISTIO_MEASURE_H_
#define ISTIO_MEASURE_H_

#include "opencensus/stats/measure.h"

namespace istio {
namespace measure {

const absl::string_view kIstioServerRequestCount = "istio.io/service/server/request_count_measure";
const absl::string_view kIstioServerRequestBytes = "istio.io/service/server/request_bytes_measure";
const absl::string_view kIstioServerResponseBytes = "istio.io/service/server/response_bytes_measure";
const absl::string_view kIstioServerResponseLatencies = "istio.io/service/server/response_latencies_measure";

opencensus::stats::MeasureInt64 ServerRequestCountMeasure() {
  absl::string_view descriptor = "number of request received by server";
  absl::string_view units = "1";
  static const opencensus::stats::MeasureInt64 request_count =
      opencensus::stats::MeasureInt64::Register(
          kIstioServerRequestCount, descriptor, units);
  return request_count;
}

opencensus::stats::MeasureInt64 ServerRequestBytesMeasure() {
  absl::string_view descriptor = "number of bytes received by server";
  absl::string_view units = "1";
  static const opencensus::stats::MeasureInt64 request_bytes =
      opencensus::stats::MeasureInt64::Register(
          kIstioServerRequestBytes, descriptor, units);
  return request_bytes;
}

opencensus::stats::MeasureInt64 ServerResponseBytesMeasure() {
  absl::string_view descriptor = "number of bytes sent by server";
  absl::string_view units = "1";
  static const opencensus::stats::MeasureInt64 response_bytes =
      opencensus::stats::MeasureInt64::Register(
          kIstioServerResponseBytes, descriptor, units);
  return response_bytes;
}

opencensus::stats::MeasureInt64 RequestCountMeasure() {
  absl::string_view descriptor = "";
  absl::string_view units = "ms";
  static const opencensus::stats::MeasureInt64 request_count =
      opencensus::stats::MeasureInt64::Register(
          kIstioServerResponseLatencies, descriptor, units);
  return request_count;
}

}  // namespace measure
}  // measure istio


#define ISTIO_MEASURE_H_