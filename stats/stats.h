#ifndef STATS_STATS_H_
#define STATS_STATS_H_

#include "stats/tags/tag_map.h"
#include "stats/measure.h"
#include "stats/internal/delta_producer.h"

namespace stats {

void Record(std::initializer_list<Measurement> measurements,
            tags::TagMap tags) {
    DeltaProducer::Get()->Record(measurements, std::move(tags));
}

}  // namespace stats

#endif  // STATS_STATS_H_
