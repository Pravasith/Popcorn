#include "ContextVk.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// Singleton members
DeviceVk *ContextVk::s_deviceVk = nullptr;
SurfaceVk *ContextVk::s_surfaceVk = nullptr;
SwapchainVk *ContextVk::s_swapchainVk = nullptr;
FramebuffersVk *ContextVk::s_framebuffersVk = nullptr;
CommandPoolVk *ContextVk::s_commandPoolVk = nullptr;
FrameVk *ContextVk::s_frameVk = nullptr;
MemoryAllocatorVk *ContextVk::s_memoryAllocatorVk = nullptr;
MemoryVk *ContextVk::s_memoryVk = nullptr;
DescriptorLayoutsVk *ContextVk::s_descriptorLayoutsVk = nullptr;
DescriptorPoolsVk *ContextVk::s_descriptorPoolsVk = nullptr;
PipelineFactoryVk *ContextVk::s_pipelineFactoryVk = nullptr;

void ContextVk::VulkanInit(const Window &appWin) {
  s_swapchainVk->SetAppWindow(appWin);
  GLFWwindow *osWindow = static_cast<GLFWwindow *>(appWin.GetOSWindow());
  //
  // CREATE INSTANCE, SET UP DEBUGGING LAYERS --------------------------------
  s_deviceVk->CreateInstance({"Vulkan App", 1, 0, 0});
  s_deviceVk->SetupDebugMessenger();

  const auto &instance = s_deviceVk->GetVkInstance();

  //
  // CREATE WINDOW SURFACE ---------------------------------------------------
  s_surfaceVk->CreateWindowSurface(instance, osWindow);

  const auto &surface = s_surfaceVk->GetSurface();

  //
  // CREATE PHYSICAL & LOGICAL DEVICE ----------------------------------------
  s_deviceVk->PickPhysicalDevice(surface);
  s_deviceVk->CreateLogicalDevice(surface);

  const auto &device = s_deviceVk->GetDevice();
  const auto &swapchainSupportDetails =
      s_deviceVk->GetSwapchainSupportDetails(surface);
  const auto &queueFamilyIndices = s_deviceVk->GetQueueFamilyIndices(surface);

  //
  // CREATE SWAPCHAIN --------------------------------------------------------
  s_swapchainVk->CreateSwapchain();
  s_swapchainVk->CreateImageViews(device);

  //
  // RENDER READY ------------------------------------------------------------
  s_commandPoolVk->CreateCommandPool();
  s_frameVk->CreateRenderSyncObjects();
};

void ContextVk::VulkanCleanUp() {
  auto &instance = s_deviceVk->GetVkInstance();
  auto &device = s_deviceVk->GetDevice();

  s_pipelineFactoryVk->CleanUp();
  s_pipelineFactoryVk->Destroy();
  s_pipelineFactoryVk = nullptr;

  s_descriptorLayoutsVk->CleanUp();
  s_descriptorLayoutsVk->Destroy();
  s_descriptorLayoutsVk = nullptr;

  s_descriptorPoolsVk->CleanUp();
  s_descriptorPoolsVk->Destroy();
  s_descriptorPoolsVk = nullptr;

  s_memoryAllocatorVk->CleanUp();
  MemoryAllocatorVk::Destroy();
  s_memoryAllocatorVk = nullptr;

  s_memoryVk->CleanUp();
  MemoryVk::Destroy();
  s_memoryVk = nullptr;

  s_frameVk->CleanUp();
  FrameVk::Destroy();
  s_frameVk = nullptr;

  s_commandPoolVk->CleanUp();
  CommandPoolVk::Destroy();
  s_commandPoolVk = nullptr;

  s_swapchainVk->CleanUp(device);
  SwapchainVk::Destroy();
  s_swapchainVk = nullptr;

  FramebuffersVk::Destroy();

  s_surfaceVk->CleanUp(instance);
  SurfaceVk::Destroy();
  s_surfaceVk = nullptr;

  s_deviceVk->CleanUp();
  DeviceVk::Destroy();
  s_deviceVk = nullptr;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
