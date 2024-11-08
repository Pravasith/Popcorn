#pragma once

#include "Global_Macros.h"
#include "Renderer.h"
#include <vector>
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
  std::vector<const char *> GetRequiredExtensions();

  virtual void OnUpdate() const override;

private:
  VkInstance m_vkInstance;
  bool m_enableValidationLayers;
  std::vector<const char *> m_validationLayers;
};

ENGINE_NAMESPACE_END
