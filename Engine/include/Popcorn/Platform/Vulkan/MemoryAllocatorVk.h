#pragma once

#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <stdexcept>
#include <vk_mem_alloc.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class MemoryAllocatorVk {
public:
  [[nodiscard]] inline static MemoryAllocatorVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new MemoryAllocatorVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of MemoryAllocatorVk")
    };
  };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  MemoryAllocatorVk(const MemoryAllocatorVk &) = delete;
  MemoryAllocatorVk &operator=(const MemoryAllocatorVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  MemoryAllocatorVk(MemoryAllocatorVk &&) = delete;
  MemoryAllocatorVk &operator=(MemoryAllocatorVk &&) = delete;

  void CreateVMAAllocator();
  void CleanUp();

  const VmaAllocator &GetVMAAllocator() const {
    //
    return m_allocator;
  };

private:
  MemoryAllocatorVk() {
    s_deviceVk = DeviceVk::Get();
    PC_PRINT("CREATED", TagType::Constr, "MemoryAllocatorVk")
  };
  ~MemoryAllocatorVk() {
    s_deviceVk = nullptr;
    PC_PRINT("DESTROYED", TagType::Destr, "MemoryAllocatorVk")
  };

private:
  static MemoryAllocatorVk *s_instance;
  static DeviceVk *s_deviceVk;

  VmaAllocator m_allocator;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
