#ifndef WASMSD_STATS_MEASURE_REGISTRY_H_
#define WASMSD_STATS_MEASURE_REGISTRY_H_

#include "absl/strings/string_view.h"
#include "measure.h"
#include "measure_descriptor.h"

namespace wasmsd {
namespace stats {

// The MeasureRegistry keeps a record of all MeasureDescriptors registered,
// providing functions for querying their metadata by name or handle. Use
// Measure<MeasureT>::Register() to register a measure with the registry.
// MeasureRegistry is thread-safe.
class MeasureRegistry final {
 public:
  // Returns the descriptor of the measure registered under 'name' if one is
  // registered, and a descriptor with an empty name otherwise.
  static const MeasureDescriptor& GetDescriptorByName(absl::string_view name);

  // Returns a measure for the registered MeasureDescriptor with the
  // provided name, if one exists, and an invalid Measure otherwise.
  static MeasureDouble GetMeasureDoubleByName(absl::string_view name);
  static MeasureInt64 GetMeasureInt64ByName(absl::string_view name);
};

}  // namespace stats
}  // namespace wasmsd

#endif  // WASMSD_STATS_MEASURE_REGISTRY_H_
