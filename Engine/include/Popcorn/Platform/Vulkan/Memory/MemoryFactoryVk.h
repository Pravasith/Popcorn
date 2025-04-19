
#pragma once

#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class MemoryFactoryVk {
public:
  [[nodiscard]] inline static MemoryFactoryVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new MemoryFactoryVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of MemoryFactoryVk")
    };
  };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  MemoryFactoryVk(const MemoryFactoryVk &) = delete;
  MemoryFactoryVk &operator=(const MemoryFactoryVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  MemoryFactoryVk(MemoryFactoryVk &&) = delete;
  MemoryFactoryVk &operator=(MemoryFactoryVk &&) = delete;

  void CleanUp();

private:
  MemoryFactoryVk() {
    s_deviceVk = DeviceVk::Get();
    PC_PRINT("CREATED", TagType::Constr, "MemoryFactoryVk")
  };
  ~MemoryFactoryVk() {
    s_deviceVk = nullptr;
    PC_PRINT("DESTROYED", TagType::Destr, "MemoryFactoryVk")
  };

private:
  static MemoryFactoryVk *s_instance;
  static DeviceVk *s_deviceVk;

  // Device-Local VkBufferMemory
  VkBuffer m_submeshVBOs;
  VkBuffer m_submeshIBOs;

  // Host-visible VkBufferMemory
  VkBuffer m_basicMaterialUBOs;
  VkBuffer m_pbrMaterialUBOs;
  VkBuffer m_modelMatrixUBOs;
  VkBuffer m_viewProjMatrixUBO; // Small size -- can be a push constant
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
