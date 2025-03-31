#pragma once

#include "CommandPoolVk.h"
#include "DescriptorFactoryVk.h"
#include "DescriptorsVk.h"
#include "DeviceVk.h"
#include "FrameVk.h"
#include "FramebuffersVk.h"
#include "GlobalMacros.h"
#include "MemoryAllocatorVk.h"
#include "Popcorn/Core/Window.h"
#include "SurfaceVk.h"
#include "SwapchainVk.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class ContextVk {

public:
  [[nodiscard]] inline static ContextVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new ContextVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of ContextVk")
    };
  };

  void VulkanInit(const Window &appWin);
  void VulkanCleanUp();

  static DeviceVk *Device() {
    if (!s_deviceVk) {
      throw std::runtime_error("DeviceVk is null");
    }
    return s_deviceVk;
  }

  static SurfaceVk *Surface() {
    if (!s_surfaceVk) {
      throw std::runtime_error("SurfaceVk is null");
    }
    return s_surfaceVk;
  }

  static SwapchainVk *Swapchain() {
    if (!s_swapchainVk) {
      throw std::runtime_error("SwapchainVk is null");
    }
    return s_swapchainVk;
  }

  static FramebuffersVk *Framebuffers() {
    if (!s_framebuffersVk) {
      throw std::runtime_error("FramebuffersVk is null");
    }
    return s_framebuffersVk;
  }

  static CommandPoolVk *CommandPool() {
    if (!s_commandPoolVk) {
      throw std::runtime_error("CommandPoolVk is null");
    }
    return s_commandPoolVk;
  }

  static FrameVk *Frame() {
    if (!s_frameVk) {
      throw std::runtime_error("FrameVk is null");
    }
    return s_frameVk;
  }

  static MemoryAllocatorVk *MemoryAllocator() {
    if (!s_memoryAllocatorVk) {
      throw std::runtime_error("MemoryAllocatorVk is null");
    }
    return s_memoryAllocatorVk;
  }

  static DescriptorSetLayoutsVk *DescriptorSetLayouts() {
    if (!s_descriptorSetLayoutsVk) {
      throw std::runtime_error("DescriptorSetLayoutsVk is null");
    }
    return s_descriptorSetLayoutsVk;
  }

  static DescriptorFactoryVk *DescriptorFactory() {
    if (!s_descriptorFactoryVk) {
      throw std::runtime_error("DescriptorFactoryVk is null");
    }
    return s_descriptorFactoryVk;
  }

private:
  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  ContextVk(const ContextVk &) = delete;
  ContextVk &operator=(const ContextVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  ContextVk(ContextVk &&) = delete;
  ContextVk &operator=(ContextVk &&) = delete;

private:
  ContextVk() {
    s_deviceVk = DeviceVk::Get();
    s_surfaceVk = SurfaceVk::Get();
    s_swapchainVk = SwapchainVk::Get();
    s_framebuffersVk = FramebuffersVk::Get();
    s_commandPoolVk = CommandPoolVk::Get();
    s_frameVk = FrameVk::Get();
    // TOOD: move to workflows
    s_descriptorSetLayoutsVk = DescriptorSetLayoutsVk::Get();
    s_memoryAllocatorVk = MemoryAllocatorVk::Get();
    s_descriptorFactoryVk = DescriptorFactoryVk::Get();
    PC_PRINT("CREATED", TagType::Constr, "ContextVk.h")
  };
  ~ContextVk() { PC_PRINT("DESTROYED", TagType::Destr, "ContextVk.h") };

private:
  static ContextVk *s_instance;
  static DeviceVk *s_deviceVk;
  static SurfaceVk *s_surfaceVk;
  static SwapchainVk *s_swapchainVk;
  static FramebuffersVk *s_framebuffersVk;
  static CommandPoolVk *s_commandPoolVk;
  static FrameVk *s_frameVk;
  static MemoryAllocatorVk *s_memoryAllocatorVk;
  static DescriptorSetLayoutsVk *s_descriptorSetLayoutsVk;
  static DescriptorFactoryVk *s_descriptorFactoryVk;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
