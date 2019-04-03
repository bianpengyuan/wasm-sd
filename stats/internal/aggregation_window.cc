// Copyright 2017, OpenCensus Authors
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

#include "aggregation_window.h"

//#include <cassert>

#include "absl/strings/str_cat.h"

namespace stats {

std::string AggregationWindow::DebugString() const {
  switch (type_) {
    case Type::kCumulative:
      return "Cumulative";
    case Type::kDelta:
      return "Delta";
    case Type::kInterval:
      return absl::StrCat("Interval (", absl::ToDoubleSeconds(duration_),
                          "s window)");
  }
//  assert(false && "Bad AggregationWindow type.");
  return "BAD TYPE";
}

}  // namespace stats
