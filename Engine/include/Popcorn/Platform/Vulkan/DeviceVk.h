#pragma once

#include "Popcorn/Core/Base.h"
#include <string>
#define GLFW_INCLUDE_VULKAN
#include "GlobalMacros.h"
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class DeviceVk {
  struct specs {
    std::string appName;
    int appVersionMajor;
    int appVersionMinor;
    int appVersionPatch;
  };

  DeviceVk() { PC_PRINT("CREATED", TagType::Constr, "DeviceVk.h") };
  ~DeviceVk() {
    CleanUp();
    PC_PRINT("DESTROYED", TagType::Destr, "DeviceVk.h")
  };

  void CreateInstance(const specs &appSpecs);
  void CleanUp();

private:
  VkInstance m_instance;
  VkPhysicalDevice m_physicalDevice;
  VkDevice m_device;

  uint32_t m_queueFamilyIndex;
  VkQueue m_queue;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
