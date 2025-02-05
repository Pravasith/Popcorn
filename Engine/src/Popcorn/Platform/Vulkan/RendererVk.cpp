#include "RendererVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
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

  // CREATE INSTANCE, SET UP DEBUGGING LAYERS
  m_deviceVk.CreateInstance({"Vulkan App", 1, 0, 0});
  m_deviceVk.SetupDebugMessenger();

  const auto &instance = m_deviceVk.GetInstance();

  // CREATE WINDOW SURFACE
  m_surfaceVk.CreateWindowSurface(instance, osWindow);

  const auto &surface = m_surfaceVk.GetSurface();

  // CREATE PHYSICAL & LOGICAL DEVICE
  m_deviceVk.PickPhysicalDevice(surface);
  m_deviceVk.CreateLogicalDevice(surface);
};

void RendererVk::VulkanDestroy() {
  const auto &instance = m_deviceVk.GetInstance();

  m_surfaceVk.CleanUp(instance);
  m_deviceVk.CleanUp();
};

void RendererVk::DrawFrame() {};
void RendererVk::PresentFrame() {};
bool RendererVk::OnFrameBfrResize(FrameBfrResizeEvent &) { return true; };

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
