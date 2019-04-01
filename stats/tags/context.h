#ifndef WASMSD_STATS_TAGS_CONTEXT_H_
#define WASMSD_STATS_TAGS_CONTEXT_H_

#include <functional>
#include <string>

#include "tag_map.h"

namespace wasmsd {
namespace stats {
namespace tags {
// Context holds information specific to an operation, such as a TagMap and
// Span. Each thread has a currently active Context. Contexts are conceptually
// immutable: the contents of a Context cannot be modified in-place.
//
// This is a draft implementation of Context, and we chose to depend on TagMap
// and Span directly. In future, the implementation will change, so only rely
// on the public API for manipulating Contexts. In future we may support
// arbitrary keys and values.
class Context {
public:
    // Returns a const reference to the current (thread local) Context.
    static const Context &Current();

    // Context is copiable and movable.
    Context(const Context &) = default;

    Context(Context &&) = default;

    Context &operator=(const Context &) = default;

    Context &operator=(Context &&) = default;

    // Returns an std::function wrapped to run with a copy of this Context.
    std::function<void()> Wrap(std::function<void()> fn) const;

    // Returns a human-readable string for debugging. Do not rely on its format or
    // try to parse it. Do not use the DebugString to retrieve Spans or Tags.
    std::string DebugString() const;

private:
    // Creates a default Context.
    Context();

    TagMap tags_;
};

}  // namespace tags
}  // namespace stats
}  // namespace wasmsd

#endif  // WASMSD_STATS_TAGS_CONTEXT_H_
