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

#ifndef OPENCENSUS_EXPORTERS_STATS_STACKDRIVER_STACKDRIVER_EXPORTER_H_
#define OPENCENSUS_EXPORTERS_STATS_STACKDRIVER_STACKDRIVER_EXPORTER_H_

#include <string>

#include "absl/base/macros.h"
#include "absl/strings/string_view.h"
#include "include/stackdriver_exporter_options.h"

#ifndef NULL_PLUGIN
#include "api/wasm/cpp/proxy_wasm_intrinsics.h"
#else
#include "extensions/common/wasm/null/null.h"
using namespace Envoy::Extensions::Common::Wasm::Null::Plugin;
using namespace Envoy::Extensions::Common::Wasm::Null::Plugin::Stackdriver;
#endif

namespace opencensus {
namespace exporters {
namespace stats {
namespace stackdriver {

// Exports stats for registered views (see opencensus/stats/stats_exporter.h) to
// Stackdriver. StackdriverExporter is thread-safe.
class StackdriverExporter {
 public:
  // Registers the exporter.
  static void Register(const StackdriverOptions& opts);

 private:
  StackdriverExporter() = delete;
};

}  // namespace stackdriver
}  // namespace stats
}  // namespace exporters
}  // namespace opencensus

#endif  // OPENCENSUS_EXPORTERS_STATS_STACKDRIVER_STACKDRIVER_EXPORTER_H_
