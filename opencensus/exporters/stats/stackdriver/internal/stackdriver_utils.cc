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

#include "opencensus/exporters/stats/stackdriver/internal/stackdriver_utils.h"

#include <string>

#include "absl/base/macros.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "google/api/distribution.pb.h"
#include "google/api/label.pb.h"
#include "google/api/metric.pb.h"
#include "google/api/monitored_resource.pb.h"
#include "google/monitoring/v3/common.pb.h"
#include "google/monitoring/v3/metric.pb.h"
#include "google/protobuf/timestamp.pb.h"
#include "opencensus/stats/stats.h"

namespace opencensus {
namespace exporters {
namespace stats {
namespace stackdriver {

namespace {

google::api::MetricDescriptor::ValueType GetValueType(
    const opencensus::stats::ViewDescriptor& descriptor) {
  switch (descriptor.aggregation().type()) {
    case opencensus::stats::Aggregation::Type::kCount:
      return google::api::MetricDescriptor::INT64;
    case opencensus::stats::Aggregation::Type::kSum:
    case opencensus::stats::Aggregation::Type::kLastValue:
      switch (descriptor.measure_descriptor().type()) {
        case opencensus::stats::MeasureDescriptor::Type::kDouble:
          return google::api::MetricDescriptor::DOUBLE;
        case opencensus::stats::MeasureDescriptor::Type::kInt64:
          return google::api::MetricDescriptor::INT64;
      }
    case opencensus::stats::Aggregation::Type::kDistribution:
      return google::api::MetricDescriptor::DISTRIBUTION;
  }
  return google::api::MetricDescriptor::DOUBLE;
}

// Overloaded function for converting ViewData value types to Points. The
// ValueType is needed because Sum aggregation with an int64 measure returns
// doubles but we want to export int64s for future compatibility.
void SetTypedValue(double value,
                   google::api::MetricDescriptor::ValueType type,
                   google::monitoring::v3::TypedValue* proto) {
  if (type == google::api::MetricDescriptor::DOUBLE) {
    proto->set_double_value(value);
  } else {
    proto->set_int64_value(static_cast<int64_t>(value));
  }
}
void SetTypedValue(int64_t value,
                   google::api::MetricDescriptor::ValueType /* type */,
                   google::monitoring::v3::TypedValue* proto) {
  proto->set_int64_value(value);
}
void SetTypedValue(const opencensus::stats::Distribution& value,
                   google::api::MetricDescriptor::ValueType /* type */,
                   google::monitoring::v3::TypedValue* proto) {
  auto* distribution_proto = proto->mutable_distribution_value();
  distribution_proto->set_count(value.count());
  distribution_proto->set_mean(value.mean());
  distribution_proto->set_sum_of_squared_deviation(
      value.sum_of_squared_deviation());
  // TODO: Set range when Stackdriver supports it.
  if (value.bucket_boundaries().num_buckets() > 1) {
    auto* buckets = distribution_proto->mutable_bucket_options()
                        ->mutable_explicit_buckets();
    for (const auto boundary : value.bucket_boundaries().lower_boundaries()) {
      buckets->add_bounds(boundary);
    }
    for (const auto bucket_count : value.bucket_counts()) {
      distribution_proto->add_bucket_counts(bucket_count);
    }
  }
}

template <typename DataValueT>
std::vector<google::monitoring::v3::TimeSeries> DataToTimeSeries(
    const opencensus::stats::ViewDescriptor& view_descriptor,
    const opencensus::stats::ViewData::DataMap<DataValueT>& data,
    const google::monitoring::v3::TimeSeries& base_time_series) {
  const google::api::MetricDescriptor::ValueType type =
      GetValueType(view_descriptor);
  std::vector<google::monitoring::v3::TimeSeries> vector;
  for (const auto& row : data) {
    vector.push_back(base_time_series);
    auto& time_series = vector.back();
    for (uint32_t i = 0; i < view_descriptor.columns().size(); ++i) {
      (*time_series.mutable_metric()
            ->mutable_labels())[view_descriptor.columns()[i].name()] =
          row.first[i];
    }
    // The point is already created in the base_time_series to set the times.
    SetTypedValue(row.second, type,
                  time_series.mutable_points(0)->mutable_value());
  }
  return vector;
}

}  // namespace

std::vector<google::monitoring::v3::TimeSeries> MakeTimeSeries(
    const opencensus::stats::ViewDescriptor& view_descriptor,
    const opencensus::stats::ViewData& data,
    const ::google::api::MonitoredResource& monitored_resource) {
  // Set values that are common across all the rows.
  auto base_time_series = google::monitoring::v3::TimeSeries();
  base_time_series.mutable_metric()->set_type(view_descriptor.name());
  (*base_time_series.mutable_resource()) = monitored_resource;
  auto* interval = base_time_series.add_points()->mutable_interval();
  SetTimestamp(data.start_time(), interval->mutable_start_time());
  SetTimestamp(data.end_time(), interval->mutable_end_time());

  switch (data.type()) {
    case opencensus::stats::ViewData::Type::kDouble:
      return DataToTimeSeries(view_descriptor, data.double_data(),
                              base_time_series);
    case opencensus::stats::ViewData::Type::kInt64:
      return DataToTimeSeries(view_descriptor, data.int_data(),
                              base_time_series);
    case opencensus::stats::ViewData::Type::kDistribution:
      return DataToTimeSeries(view_descriptor, data.distribution_data(),
                              base_time_series);
  }
  return {};
}

void SetTimestamp(absl::Time time, google::protobuf::Timestamp* proto) {
  const int64_t seconds = absl::ToUnixSeconds(time);
  proto->set_seconds(seconds);
  proto->set_nanos(
      absl::ToInt64Nanoseconds(time - absl::FromUnixSeconds(seconds)));
}

}  // namespace stackdriver
}  // namespace stats
}  // namespace exporters
}  // namespace opencensus
