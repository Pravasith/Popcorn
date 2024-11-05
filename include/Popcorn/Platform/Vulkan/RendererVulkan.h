#pragma once

#include "Global_Macros.h"
#include "Renderer.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
class RendererVulkan : public Renderer {
public:
  RendererVulkan();
  ~RendererVulkan();

private:
  void InitVulkan();
  void CleanUp();

  void CreateInstance();

  virtual void OnUpdate() const override;

private:
  VkInstance m_vk_instance;
};

ENGINE_NAMESPACE_END
