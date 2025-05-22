#pragma once

#include "CommandPoolVk.h"
#include "DescriptorLayoutsVk.h"
#include "DescriptorPoolsVk.h"
#include "DeviceVk.h"
#include "FrameVk.h"
#include "FramebufferVk.h"
#include "GlobalMacros.h"
#include "Memory/MemoryAllocatorVk.h"
#include "Memory/MemoryVk.h"
#include "PipelineFactoryVk.h"
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

  static FramebufferVk *Framebuffer() {
    if (!s_framebufferVk) {
      throw std::runtime_error("FramebufferVk is null");
    }
    return s_framebufferVk;
  }

  static MemoryAllocatorVk *MemoryAllocator() {
    if (!s_memoryAllocatorVk) {
      throw std::runtime_error("MemoryAllocatorVk is null");
    }
    return s_memoryAllocatorVk;
  }

  static MemoryVk *Memory() {
    if (!s_memoryVk) {
      throw std::runtime_error("MemoryVk is null");
    }
    return s_memoryVk;
  }

  static DescriptorPoolsVk *DescriptorPools() {
    if (!s_descriptorPoolsVk) {
      throw std::runtime_error("DescriptorPoolsVk is null");
    }
    return s_descriptorPoolsVk;
  }

  static DescriptorLayoutsVk *DescriptorLayouts() {
    if (!s_descriptorLayoutsVk) {
      throw std::runtime_error("DescriptorLayoutsVk is null");
    }
    return s_descriptorLayoutsVk;
  }

  static PipelineFactoryVk *PipelineFactory() {
    if (!s_pipelineFactoryVk) {
      throw std::runtime_error("PipelineFactoryVk is null");
    }
    return s_pipelineFactoryVk;
  }

  void VulkanInit(const Window &appWin);
  void VulkanCleanUp();

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
    s_commandPoolVk = CommandPoolVk::Get();
    s_frameVk = FrameVk::Get();
    s_framebufferVk = FramebufferVk::Get();
    s_memoryAllocatorVk = MemoryAllocatorVk::Get();
    s_memoryVk = MemoryVk::Get();
    s_descriptorPoolsVk = DescriptorPoolsVk::Get();
    s_descriptorLayoutsVk = DescriptorLayoutsVk::Get();
    s_pipelineFactoryVk = PipelineFactoryVk::Get();

    PC_PRINT("CREATED", TagType::Constr, "ContextVk.h")
  };
  ~ContextVk() { PC_PRINT("DESTROYED", TagType::Destr, "ContextVk.h") };

private:
  static ContextVk *s_instance;
  static DeviceVk *s_deviceVk;
  static SurfaceVk *s_surfaceVk;
  static SwapchainVk *s_swapchainVk;
  static CommandPoolVk *s_commandPoolVk;
  static FrameVk *s_frameVk;
  static FramebufferVk *s_framebufferVk;
  static MemoryVk *s_memoryVk;
  static MemoryAllocatorVk *s_memoryAllocatorVk;
  static DescriptorPoolsVk *s_descriptorPoolsVk;
  static DescriptorLayoutsVk *s_descriptorLayoutsVk;
  static PipelineFactoryVk *s_pipelineFactoryVk;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
