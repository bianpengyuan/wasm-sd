#ifndef WASMSD_STATS_TAGS_TAGS_MAP_H_
#define WASMSD_STATS_TAGS_TAGS_MAP_H_

#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

#include "absl/strings/string_view.h"
#include "tag_key.h"

namespace wasmsd {
namespace stats {
namespace tags {

// TagMap represents an immutable map of TagKeys to tag values (strings), and
// provides efficient equality and hash operations. A TagMap is expensive to
// construct, and should be shared between uses where possible.
class TagMap final {
public:
    // Both constructors are not explicit so that Record({}, {{"k", "v"}}) works.
    // This constructor is needed because even though we copy to a vector
    // internally because c++ cannot deduce the conversion needed.
    TagMap(std::initializer_list<std::pair<TagKey, absl::string_view>> tags);

    // This constructor is needed so that callers can dynamically construct
    // TagMaps. It takes the argument by value to allow it to be moved.
    TagMap(std::vector<std::pair<TagKey, std::string>> tags);

    // Accesses the tags sorted by key (in an implementation-defined, not
    // lexicographic, order).
    const std::vector<std::pair<TagKey, std::string>> &tags() const {
      return tags_;
    }

    struct Hash {
        std::size_t operator()(const TagMap &tags) const;
    };

    bool operator==(const TagMap &other) const;

    bool operator!=(const TagMap &other) const { return !(*this == other); }

    // Returns a human-readable string for debugging. Do not rely on its format or
    // try to parse it. Do not use it to retrieve tags.
    std::string DebugString() const;

private:
    void Initialize();

    std::size_t hash_;
    // TODO: add an option to store string_views to avoid copies.
    std::vector<std::pair<TagKey, std::string>> tags_;
};

}  // namespace tags
}  // namespace stats
}  // namespace wasmsd

#endif  // WASMSD_STATS_TAGS_TAGS_MAP_H_
