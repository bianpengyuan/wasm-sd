#include "measure.h"
#include "measure_descriptor.h"
#include "measure_registry_impl.h"
#include "measure_registry.h"

#include "absl/strings/string_view.h"

namespace wasmsd {
namespace stats {

// static
template <typename MeasureT>
Measure<MeasureT> Measure<MeasureT>::Register(absl::string_view name,
                                    absl::string_view description,
                                    absl::string_view units) {
  return MeasureRegistryImpl::Get()->Register<MeasureT>(name, description,
                                                        units);
}


template <typename MeasureT>
Measure<MeasureT>::Measure(uint64_t id) : id_(id) {}


template class Measure<double>;
template class Measure<int64_t>;

}  // namespace stats
}  // namespace wasmsd