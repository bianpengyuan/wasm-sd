#include "stats/measure.h"
#include "stats/measure_descriptor.h"
#include "stats/measure_registry_impl.h"
#include "stats/measure_registry.h"

#include "absl/strings/string_view.h"

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
