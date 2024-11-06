#pragma once

#include "Global_Macros.h"
#include "Renderer.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class RendererVulkan : public Renderer {
public:
  RendererVulkan();
  ~RendererVulkan();

private:
  void InitVulkan();
  void CleanUp();

  void CreateInstance();
  bool CheckValidationLayerSupport();

  virtual void OnUpdate() const override;

private:
  VkInstance m_vkInstance;
  bool m_enableValidationLayers;
};

ENGINE_NAMESPACE_END
