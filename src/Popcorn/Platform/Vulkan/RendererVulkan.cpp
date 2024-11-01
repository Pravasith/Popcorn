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

void RendererVulkan::InitVulkan() {};
void RendererVulkan::OnUpdate() {};
void RendererVulkan::CleanUp() {};

ENGINE_NAMESPACE_END
