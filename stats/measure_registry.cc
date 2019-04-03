#include "stats/measure_registry.h"
#include "stats/measure_registry_impl.h"

namespace stats {

// static
const MeasureDescriptor& MeasureRegistry::GetDescriptorByName(
    absl::string_view name) {
  return MeasureRegistryImpl::Get()->GetDescriptorByName(name);
}

// static
MeasureDouble MeasureRegistry::GetMeasureDoubleByName(absl::string_view name) {
  return MeasureRegistryImpl::Get()->GetMeasureDoubleByName(name);
}

// static
MeasureInt64 MeasureRegistry::GetMeasureInt64ByName(absl::string_view name) {
  return MeasureRegistryImpl::Get()->GetMeasureInt64ByName(name);
}

}  // namespace stats
