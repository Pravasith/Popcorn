#pragma once

#include "GlobalMacros.h"
#include "PipelineVk.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class CompositePipelineVk : public PipelineVk<PipelineTypes::GraphicsType> {
public:
  CompositePipelineVk() {
    PC_PRINT("CREATED", TagType::Constr, "CompositePipelineVk");
  };
  ~CompositePipelineVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "CompositePipelineVk");
  };

  virtual void CreateVkPipeline(const VkDevice &device,
                                const PipelineStateType &pipelineCreateInfo,
                                const VkRenderPass &renderPass) override;

  virtual void CleanUp(const VkDevice &device) override;

  virtual void CreateShaderStageCreateInfos(
      std::forward_list<VkShaderModule> &shaderModules) override;

  // inline virtual void
  // GetDefaultPipelineState(PiplelineStateType &pipelineState) override {
  //   auto &createCompositePipelineInfo =
  //       static_cast<CompositePipelineState &>(pipelineState);
  //
  //   GetDefaultDynamicState(createCompositePipelineInfo.dynamicState);
  //   GetDefaultVertexInputState(createCompositePipelineInfo.vertexInputState);
  //   GetDefaultInputAssemblyState(createCompositePipelineInfo.inputAssemblyState);
  //   GetDefaultViewportState(createCompositePipelineInfo.viewportState);
  //   GetDefaultRasterizationState(createCompositePipelineInfo.rasterizationState);
  //   GetDefaultMultisampleState(createCompositePipelineInfo.multisampleState);
  //   GetDefaultDepthStencilState(createCompositePipelineInfo.depthStencilState);
  //   GetDefaultColorBlendingState(createCompositePipelineInfo.colorBlendState);
  //   GetDefaultPipelineLayoutCreateInfo(createCompositePipelineInfo.pipelineLayout);
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
