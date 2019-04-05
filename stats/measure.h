#include "stats/measure_descriptor.h"

#ifndef STATS_MEASURE_H_
#define STATS_MEASURE_H_

namespace stats {

template <typename MeasureT>
class Measure {
 public:
  static Measure<MeasureT> Register(absl::string_view name,
                                    absl::string_view description,
                                    absl::string_view units);
  bool IsValid() const;
 private:
  friend class Measurement;
  friend class MeasureRegistryImpl;
  const uint64_t id_;
  explicit Measure(uint64_t id);
};

typedef Measure<double> MeasureDouble;
typedef Measure<int64_t> MeasureInt64;


class Measurement final {
 public:
    template <typename T, typename std::enable_if<
            std::is_floating_point<T>::value>::type* = nullptr>
    Measurement(MeasureDouble measure, T value)
            : id_(measure.id_), value_double_(value) {}

    template <typename T, typename std::enable_if<
            std::is_integral<T>::value>::type* = nullptr>
    Measurement(MeasureInt64 measure, T value)
            : id_(measure.id_), value_int_(value) {}

private:
    friend class Delta;
    const uint64_t id_;
    union {
        const double value_double_;
        const int64_t value_int_;
    };
};

template <>
bool MeasureDouble::IsValid() const;
template <>
bool MeasureInt64::IsValid() const;

}  // namespace stats

#endif  // STATS_MEASURE_H_
