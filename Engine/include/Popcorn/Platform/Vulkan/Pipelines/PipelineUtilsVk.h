#pragma once

#include "GlobalMacros.h"
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class PipelineUtilsVk {
public:
  static void
  GetDefaultDynamicState(VkPipelineDynamicStateCreateInfo &dynamicState);

  static void GetDefaultVertexInputState(
      VkPipelineVertexInputStateCreateInfo &vertexInputState);

  static void GetDefaultInputAssemblyState(
      VkPipelineInputAssemblyStateCreateInfo &inputAssemblyState);

  static void
  GetDefaultViewportState(VkPipelineViewportStateCreateInfo &viewportState);

  static void GetDefaultRasterizationState(
      VkPipelineRasterizationStateCreateInfo &rasterizationState);

  static void GetDefaultMultisampleState(
      VkPipelineMultisampleStateCreateInfo &multisampleState);

  static void GetDefaultDepthStencilState(
      VkPipelineDepthStencilStateCreateInfo &depthStencilState);

  static void
  GetDefaultViewportAndScissorState(VkViewport &viewport, VkRect2D &scissor,
                                    const VkExtent2D &swapchainExtent);

  static void GetDefaultColorBlendingState(
      VkPipelineColorBlendStateCreateInfo &colorBlendState);

  static void GetDefaultPipelineLayoutCreateInfo(
      VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo);

private:
  static std::vector<VkDynamicState> s_dynamicStatesDefault;
  static VkPipelineColorBlendAttachmentState s_colorBlendAttachmentDefault;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
