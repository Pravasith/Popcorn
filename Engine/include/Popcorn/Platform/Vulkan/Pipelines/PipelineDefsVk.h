#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Buffer.h"
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum PipelineTypes { GraphicsType = 1, ComputeType, RaytracingType };

struct GfxPipelineState {
  static constexpr PipelineTypes type_value = PipelineTypes::GraphicsType;

  VkExtent2D swapchainExtent{};
  VkPipelineDynamicStateCreateInfo dynamicState{};
  VkPipelineVertexInputStateCreateInfo vertexInputState{};
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
  VkPipelineViewportStateCreateInfo viewportState{};
  VkPipelineRasterizationStateCreateInfo rasterizationState{};
  VkPipelineDepthStencilStateCreateInfo depthStencilState{};
  VkPipelineMultisampleStateCreateInfo multisampleState{};
  VkPipelineColorBlendStateCreateInfo colorBlendState{};
  VkPipelineLayoutCreateInfo pipelineLayout{};
};

struct ComputePipelineState {
  static constexpr PipelineTypes type_value = PipelineTypes::ComputeType;

  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkPipelineCache pipelineCache = VK_NULL_HANDLE;
  VkPipelineCreateFlags flags = 0;
  VkPipeline basePipelineHandle = VK_NULL_HANDLE;
  int32_t basePipelineIndex = -1;
};

struct RaytracingPipelineState {
  static constexpr PipelineTypes type_value = PipelineTypes::RaytracingType;

  std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups{};
  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  uint32_t maxRecursionDepth = 1;
  VkPipelineCache pipelineCache = VK_NULL_HANDLE;
  VkPipelineCreateFlags flags = 0;
  VkPipelineLibraryCreateInfoKHR libraryInfo{};
};

// PRIMARY TEMPLATE
template <PipelineTypes T> struct DerivePipelineCreateInfoType;

// SPECIALIZATION TEMPLATES
template <> struct DerivePipelineCreateInfoType<PipelineTypes::GraphicsType> {
  using type = GfxPipelineState;
};

template <> struct DerivePipelineCreateInfoType<PipelineTypes::ComputeType> {
  using type = ComputePipelineState;
};

template <> struct DerivePipelineCreateInfoType<PipelineTypes::RaytracingType> {
  using type = RaytracingPipelineState;
};

//
// ----------------------------------------------------------------------------
// STATIC SHADER HELPERS ------------------------------------------------------
[[nodiscard]] static VkShaderModule
PC_CreateShaderModule(const VkDevice &device, const Buffer &bytecode) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = bytecode.GetSize();
  createInfo.pCode = bytecode.AsType<uint32_t>();

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return shaderModule;
};

inline static void PC_DestroyShaderModule(const VkDevice &device,
                                          VkShaderModule shaderModule) {
  vkDestroyShaderModule(device, shaderModule, nullptr);
};
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
