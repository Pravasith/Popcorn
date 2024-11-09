#pragma once

#include "Global_Macros.h"
#include "Renderer.h"
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VkValidationLayers.h"

ENGINE_NAMESPACE_BEGIN
class RendererVulkan : public Renderer {
public:
  RendererVulkan();
  ~RendererVulkan();

private:
  void InitVulkan();
  void CleanUp();

  void CreateInstance();
  std::vector<const char *> GetRequiredExtensions();

  virtual void OnUpdate() const override;

private:
  // MEMBERS
  VkInstance m_vkInstance;
#ifdef NDEBUG
  static constexpr bool s_enableValidationLayers = false;
#else
  static constexpr bool s_enableValidationLayers = true;
#endif
  // CLASS MEMBERS
  VkValidationLayers m_VkValLyrs;
};

ENGINE_NAMESPACE_END
