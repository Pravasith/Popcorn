#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN

class MemoryVaultVk {
public:
  [[nodiscard]] inline static MemoryVaultVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new MemoryVaultVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of MemoryVaultVk")
    };
  };

private:
  MemoryVaultVk() { PC_PRINT("CREATED", TagType::Constr, "MemoryVaultVk.h") };
  ~MemoryVaultVk() { PC_PRINT("DESTROYED", TagType::Destr, "MemoryVaultVk.h") };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  MemoryVaultVk(const MemoryVaultVk &) = delete;
  MemoryVaultVk &operator=(const MemoryVaultVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  MemoryVaultVk(MemoryVaultVk &&) = delete;
  MemoryVaultVk &operator=(MemoryVaultVk &&) = delete;

private:
  static MemoryVaultVk *s_instance;
};

ENGINE_NAMESPACE_END
