#include "RendererVk.h"
#include "Popcorn/Core/Base.h"
#include "Shader.h"
#include "Sources.h"
#include <cstring>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

RendererVk::RendererVk(const Window &appWin) : Renderer(appWin) {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER-VK");
  VulkanInit();
};

RendererVk::~RendererVk() {
  VulkanDestroy();
  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER-VULKAN");
};

void RendererVk::VulkanInit() {
  GLFWwindow *osWindow = static_cast<GLFWwindow *>(m_AppWin.GetOSWindow());

  // CREATE INSTANCE, SET UP DEBUGGING LAYERS --------------------------------
  m_deviceVk.CreateInstance({"Vulkan App", 1, 0, 0});
  m_deviceVk.SetupDebugMessenger();

  const auto &instance = m_deviceVk.GetInstance();

  // CREATE WINDOW SURFACE ---------------------------------------------------
  m_surfaceVk.CreateWindowSurface(instance, osWindow);

  const auto &surface = m_surfaceVk.GetSurface();

  // CREATE PHYSICAL & LOGICAL DEVICE ----------------------------------------
  m_deviceVk.PickPhysicalDevice(surface);
  m_deviceVk.CreateLogicalDevice(surface);

  const auto &device = m_deviceVk.GetDevice();
  const auto &swapChainSupportDetails =
      m_deviceVk.GetSwapChainSupportDetails(surface);
  const auto &queueFamilyIndices = m_deviceVk.GetQueueFamilyIndices(surface);

  // CREATE SWAPCHAIN --------------------------------------------------------
  m_swapChainVk.CreateSwapChain(device, swapChainSupportDetails, osWindow,
                                surface, queueFamilyIndices);

  Shader s;
  auto vertShdr = s.ReadSpvFile(PC_src_map[VkTriVert]);
};

void RendererVk::VulkanDestroy() {
  const auto &instance = m_deviceVk.GetInstance();
  const auto &device = m_deviceVk.GetDevice();

  m_swapChainVk.CleanUp(device);
  m_surfaceVk.CleanUp(instance);
  m_deviceVk.CleanUp();
};

void RendererVk::DrawFrame() {};
void RendererVk::PresentFrame() {};
bool RendererVk::OnFrameBfrResize(FrameBfrResizeEvent &) { return true; };

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
