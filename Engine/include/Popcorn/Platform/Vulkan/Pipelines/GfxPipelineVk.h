#pragma once

#include "GlobalMacros.h"
#include "PipelineVk.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class GfxPipelineVk : public PipelineVk {
  GfxPipelineVk() : PipelineVk(Types::GraphicsType) {
    PC_PRINT("CREATED", TagType::Constr, "GfxPipelineVk");
  };
  ~GfxPipelineVk() { PC_PRINT("DESTROYED", TagType::Destr, "GfxPipelineVk"); };

  virtual void Make() override;

  virtual std::vector<VkPipelineShaderStageCreateInfo>
  CreateShaderStages(std::forward_list<VkShaderModule> shaderModules) override;

  virtual void SetDefaultDynamicStates(
      VkPipelineDynamicStateCreateInfo &dynamicState) override;

  virtual void SetDefaultVertexInputState(
      VkPipelineVertexInputStateCreateInfo &vertexInputState) override;

  virtual void SetDefaultInputAssemblyState(
      VkPipelineInputAssemblyStateCreateInfo &inputAssemblyState) override;

  virtual void
  SetDefaultViewportState(VkPipelineViewportStateCreateInfo &viewportState,
                          const VkExtent2D &extent) override;

  virtual void SetDefaultRasterizationState(
      VkPipelineRasterizationStateCreateInfo &rasterizationState) override;

  virtual void SetDefaultMultisampleState(
      VkPipelineMultisampleStateCreateInfo &multisampleState) override;

  virtual std::pair<VkViewport, VkRect2D>
  GetViewportAndScissor(const VkExtent2D &swapchainExtent) const override;

  virtual void SetDefaultColorBlendingState(
      VkPipelineColorBlendStateCreateInfo &colorBlendState) override;

  virtual void SetDefaultPipelineLayout(const VkDevice &device) override;

  virtual void DestroyPipelineLayout(const VkDevice &device) override;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
