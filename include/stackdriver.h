#ifndef STACKDRIVER_H
#define STACKDRIVER_H

#include "api/proxy_wasm_intrinsics.h"

class StackdriverContext : public Context {
 public:
  explicit StackdriverContext(uint32_t id) : Context(id) {}
  void onStart() override;
  void onLog() override;
  void onTick() override;
 private:
};

#endif // STACKDRIVER_H