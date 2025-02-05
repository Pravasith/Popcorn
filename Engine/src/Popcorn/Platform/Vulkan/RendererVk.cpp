#include "RendererVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cstring>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

RendererVk::RendererVk(const Window &appWin) : Renderer(appWin) {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER-VK");

  m_deviceVk.CreateInstance({"Vulkan App", 1, 0, 0}); // Vulkan instance
  m_deviceVk.SetupDebugMessenger();                   // Validation layers
  m_deviceVk.PickPhysicalDevice();                    // Physical device
  m_deviceVk.CreateLogicalDevice();                   // Logical device
};

RendererVk::~RendererVk() {
  m_deviceVk.CleanUp();
  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER-VULKAN");
};

void RendererVk::DrawFrame() {};
void RendererVk::PresentFrame() {};
bool RendererVk::OnFrameBfrResize(FrameBfrResizeEvent &) { return true; };

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
