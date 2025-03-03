#pragma once

#include "GlobalMacros.h"
#include "PipelineVk.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class GfxPipelineVk : public PipelineVk<PipelineTypes::GraphicsType> {
public:
  GfxPipelineVk() { PC_PRINT("CREATED", TagType::Constr, "GfxPipelineVk"); };
  ~GfxPipelineVk() { PC_PRINT("DESTROYED", TagType::Destr, "GfxPipelineVk"); };

  virtual void CreateVkPipeline(const VkDevice &device,
                                const PipelineStateType &pipelineCreateInfo,
                                const VkRenderPass &renderPass) override;

  virtual void CleanUp(const VkDevice &device) override;

  virtual void CreateShaderStageCreateInfos(
      std::forward_list<VkShaderModule> &shaderModules) override;

  // inline virtual void
  // GetDefaultPipelineState(PiplelineStateType &pipelineState) override {
  //   auto &createGfxPipelineInfo =
  //       static_cast<GfxPipelineState &>(pipelineState);
  //
  //   GetDefaultDynamicState(createGfxPipelineInfo.dynamicState);
  //   GetDefaultVertexInputState(createGfxPipelineInfo.vertexInputState);
  //   GetDefaultInputAssemblyState(createGfxPipelineInfo.inputAssemblyState);
  //   GetDefaultViewportState(createGfxPipelineInfo.viewportState);
  //   GetDefaultRasterizationState(createGfxPipelineInfo.rasterizationState);
  //   GetDefaultMultisampleState(createGfxPipelineInfo.multisampleState);
  //   GetDefaultDepthStencilState(createGfxPipelineInfo.depthStencilState);
  //   GetDefaultColorBlendingState(createGfxPipelineInfo.colorBlendState);
  //   GetDefaultPipelineLayoutCreateInfo(createGfxPipelineInfo.pipelineLayout);
  // };

  // OVERRIDE METHODS ---------------------------------------------------------
  // --------------------------------------------------------------------------
  //

  virtual void CreatePipelineLayout(
      const VkDevice &device,
      const VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) override;
  virtual void DestroyPipelineLayout(const VkDevice &device) override;

private:
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
