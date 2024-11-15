#pragma once

#include "PhysDeviceVk.h"
#include <vector>
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

  void CleanUp();

  void CreateLogicalDevice(const PhysDeviceVk::QueueFamilyIndices &qFamIndices,
                           const std::vector<const char *> &valLyrsVk,
                           const VkPhysicalDevice &physDevVk);
  const VkInstance &m_vkInst;

  VkDevice m_device;
};

ENGINE_NAMESPACE_END
