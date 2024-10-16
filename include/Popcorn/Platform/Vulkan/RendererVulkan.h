#pragma once

#include "Global_Macros.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
class RendererVulkan {
public:
  RendererVulkan();
  ~RendererVulkan();

private:
  void InitVulkan();
  void CleanUp();
  virtual void OnUpdate();

private:
  VkInstance m_vk_instance;
};

ENGINE_NAMESPACE_END
