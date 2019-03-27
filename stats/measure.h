

namespace stackdriver {
namespace stats {

class Measure final {
 public:
  static Measure<MeasureT> Register(absl::string_view name,
                                    absl::string_view description,
                                    absl::string_view units);
};

}  // namespace stats
}  // namespace stackdriver