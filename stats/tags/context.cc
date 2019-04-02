#include "context.h"

#include <functional>
#include <utility>

#include "absl/strings/str_cat.h"
#include "with_context.h"

namespace wasmsd {
namespace stats {
namespace tags {

Context::Context()
    : tags_(TagMap({})) {}

// static
const Context& Context::Current() { return *InternalMutableCurrent(); }

std::function<void()> Context::Wrap(std::function<void()> fn) const {
  Context copy(Context::Current());
  return [fn, copy]() {
    WithContext wc(copy);
    fn();
  };
}

std::string Context::DebugString() const {
  return absl::StrCat("ctx@", absl::Hex(this),
                      ", tags=", tags_.DebugString());
}

// static
Context* Context::InternalMutableCurrent() {
  static thread_local Context* thread_ctx = nullptr;
  if (thread_ctx == nullptr) thread_ctx = new Context;
  return thread_ctx;
}

void swap(Context& a, Context& b) {
  using std::swap;
  swap(a.tags_, b.tags_);
}

}  // namespace tags
}  // namespace stats
}  // namespace wasmsd
