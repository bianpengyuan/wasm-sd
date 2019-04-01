#include "measure_descriptor.h"

#ifndef WASMSD_STATS_MEASURE_H_
#define WASMSD_STATS_MEASURE_H_

namespace wasmsd {
namespace stats {

template <typename MeasureT>
class Measure {
 public:
  static Measure<MeasureT> Register(absl::string_view name,
                                    absl::string_view description,
                                    absl::string_view units);

 private:
  friend class MeasureRegistryImpl;
  const uint64_t id_;
  explicit Measure(uint64_t id);
};

typedef Measure<double> MeasureDouble;
typedef Measure<int64_t> MeasureInt64;

}  // namespace stats
}  // namespace wasmsd

#endif  // WASMSD_STATS_MEASURE_H_
