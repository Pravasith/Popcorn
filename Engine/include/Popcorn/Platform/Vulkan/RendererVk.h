
#include "GlobalMacros.h"
#include "Popcorn/Core/Window.h"
#include "Popcorn/Events/WindowEvent.h"
#include "Renderer.h"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class RendererVk : public Renderer {
public:
  RendererVk(const Window &appWin);
  ~RendererVk() override;

  // Sets up devices, configure swapchains, creates depth buffers
  // also allocates command pools
  void VulkanInit() {};
  void VulkanDestroy() {};

  virtual void DrawFrame() override;
  virtual void PresentFrame() override;
  virtual bool OnFrameBfrResize(FrameBfrResizeEvent &) override;

private:
  static VkInstance s_instance;
  static VkPhysicalDevice s_physicalDevice;
  static VkDevice s_device;

  uint32_t s_queueFamilyIndex;
  VkQueue s_queue;

  std::vector<VkDescriptorPool> s_descriptorPools;
  std::vector<VkRenderPass> s_renderPasses;

  // Swapchain images
  uint32_t s_minImageCount;
  uint32_t s_imageCount;
  uint32_t s_subpass;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
