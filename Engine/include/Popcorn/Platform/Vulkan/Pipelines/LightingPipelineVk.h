#pragma once

#include "GlobalMacros.h"
#include "PipelineVk.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class LightingPipelineVk : public PipelineVk<PipelineTypes::GraphicsType> {
public:
  LightingPipelineVk() {
    PC_PRINT("CREATED", TagType::Constr, "LightingPipelineVk");
  };
  ~LightingPipelineVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "LightingPipelineVk");
  };

  virtual void CreateVkPipeline(const VkDevice &device,
                                const PipelineStateType &pipelineCreateInfo,
                                const VkRenderPass &renderPass) override;

  virtual void CleanUp(const VkDevice &device) override;

  virtual void CreateShaderStageCreateInfos(
      std::forward_list<VkShaderModule> &shaderModules) override;

  // inline virtual void
  // GetDefaultPipelineState(PiplelineStateType &pipelineState) override {
  //   auto &createLightingPipelineInfo =
  //       static_cast<LightingPipelineState &>(pipelineState);
  //
  //   GetDefaultDynamicState(createLightingPipelineInfo.dynamicState);
  //   GetDefaultVertexInputState(createLightingPipelineInfo.vertexInputState);
  //   GetDefaultInputAssemblyState(createLightingPipelineInfo.inputAssemblyState);
  //   GetDefaultViewportState(createLightingPipelineInfo.viewportState);
  //   GetDefaultRasterizationState(createLightingPipelineInfo.rasterizationState);
  //   GetDefaultMultisampleState(createLightingPipelineInfo.multisampleState);
  //   GetDefaultDepthStencilState(createLightingPipelineInfo.depthStencilState);
  //   GetDefaultColorBlendingState(createLightingPipelineInfo.colorBlendState);
  //   GetDefaultPipelineLayoutCreateInfo(createLightingPipelineInfo.pipelineLayout);
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
