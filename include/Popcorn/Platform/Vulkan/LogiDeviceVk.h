#pragma once

#include "PhysDeviceVk.h"
#define GLFW_INCLUDE_VULKAN
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN

class RendererVk;

class LogiDeviceVk {
  friend class RendererVk;

private:
  LogiDeviceVk(const VkInstance &vkInst) : m_vkInst(vkInst) {
    PC_PRINT("CREATED", TagType::Constr, "LOGICAL-DEVICE");
  };
  ~LogiDeviceVk() { PC_PRINT("DESTROYED", TagType::Destr, "LOGICAL-DEVICE"); };

  void CreateLogicalDevice(const PhysDeviceVk::QueueFamilyIndices &qFamIndices);
  const VkInstance &m_vkInst;
};

ENGINE_NAMESPACE_END
