#ifndef ISTIO_TAG_H_
#define ISTIO_TAG_H_

#include "opencensus/stats/tag_key.h"

namespace istio {
namespace tag {

opencensus::stats::TagKey RequestLatencyKey() {
  static const auto latency_key =
      opencensus::stats::TagKey::Register("request_latency");
  return latency_key;
}

}  // namespace tag
}  // measure istio


#endif // ISTIO_TAG_H_