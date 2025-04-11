#pragma once

#include "GlobalMacros.h"
#include "PipelineVk.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// The enum looks like this --
// enum class PipelineTypes { GraphicsType = 1, ComputeType, RaytracingType };

class GBufferPipelineVk : public PipelineVk<PipelineTypes::GraphicsType> {
public:
  GBufferPipelineVk() {
    PC_PRINT("CREATED", TagType::Constr, "GBufferPipelineVk");
  };
  ~GBufferPipelineVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "GBufferPipelineVk");
  };

  virtual void CreateVkPipeline(const VkDevice &device,
                                const PipelineStateType &pipelineCreateInfo,
                                const VkRenderPass &renderPass) override;

  virtual void CleanUp(const VkDevice &device) override;

  virtual void CreateShaderStageCreateInfos(
      std::forward_list<VkShaderModule> &shaderModules) override;

  // inline virtual void
  // GetDefaultPipelineState(PiplelineStateType &pipelineState) override {
  //   auto &createGBufferPipelineInfo =
  //       static_cast<GBufferPipelineState &>(pipelineState);
  //
  //   GetDefaultDynamicState(createGBufferPipelineInfo.dynamicState);
  //   GetDefaultVertexInputState(createGBufferPipelineInfo.vertexInputState);
  //   GetDefaultInputAssemblyState(createGBufferPipelineInfo.inputAssemblyState);
  //   GetDefaultViewportState(createGBufferPipelineInfo.viewportState);
  //   GetDefaultRasterizationState(createGBufferPipelineInfo.rasterizationState);
  //   GetDefaultMultisampleState(createGBufferPipelineInfo.multisampleState);
  //   GetDefaultDepthStencilState(createGBufferPipelineInfo.depthStencilState);
  //   GetDefaultColorBlendingState(createGBufferPipelineInfo.colorBlendState);
  //   GetDefaultPipelineLayoutCreateInfo(createGBufferPipelineInfo.pipelineLayout);
  // };

  // OVERRIDE METHODS ---------------------------------------------------------
  // --------------------------------------------------------------------------
  //

  virtual void CreatePipelineLayout(
      const VkDevice &device,
      const VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) override;
  virtual void DestroyPipelineLayout(const VkDevice &device) override;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
