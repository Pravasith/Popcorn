#include "RendererVulkan.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan.h>

// HELLO TRIANGLE!
ENGINE_NAMESPACE_BEGIN

RendererVulkan::RendererVulkan() {
  PC_PRINT_DEBUG("VULKAN CREATED", 2, "RENDERER");

  InitVulkan();
};

RendererVulkan::~RendererVulkan() {
  // APPLICATION
  PC_PRINT_DEBUG("VULKAN DESTROYED", 2, "RENDERER");
};

void RendererVulkan::InitVulkan() {
  PC_PRINT_DEBUG(s_os_window, 2, "RENDERER VULKAN")
};

void RendererVulkan::OnUpdate() const {};
void RendererVulkan::CleanUp() {};

ENGINE_NAMESPACE_END
