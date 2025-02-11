#pragma once

#include "GlobalMacros.h"
#include "PipelineVk.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class GfxPipelineVk : public PipelineVk<PipelineTypes::GraphicsType> {
public:
  GfxPipelineVk() { PC_PRINT("CREATED", TagType::Constr, "GfxPipelineVk"); };
  ~GfxPipelineVk() {
    PC_VK_NULL_CHECK(m_device)
    DestroyPipelineLayout(m_device);
    PC_PRINT("DESTROYED", TagType::Destr, "GfxPipelineVk");
  };

  //
  // --------------------------------------------------------------------------
  // OVERRIDE METHODS ---------------------------------------------------------
  [[nodiscard]] virtual std::vector<VkPipelineShaderStageCreateInfo>
  CreateShaderStages(std::forward_list<VkShaderModule> &shaderModules) override;

  inline virtual void
  GetDefaultPipelineCreateInfo(CreateInfo_type &createInfo) override {
    auto &createGfxPipelineInfo =
        static_cast<GfxPipelineCreateInfo &>(createInfo);

    GetDefaultDynamicState(createGfxPipelineInfo.dynamicState);
    GetDefaultVertexInputState(createGfxPipelineInfo.vertexInputState);
    GetDefaultInputAssemblyState(createGfxPipelineInfo.inputAssemblyState);
    GetDefaultViewportState(createGfxPipelineInfo.viewportState);
    GetDefaultRasterizationState(createGfxPipelineInfo.rasterizationState);
    GetDefaultMultisampleState(createGfxPipelineInfo.multisampleState);
    GetDefaultDepthStencilState(createGfxPipelineInfo.depthStencilState);
    GetDefaultColorBlendingState(createGfxPipelineInfo.colorBlendState);
    GetDefaultPipelineLayout(createGfxPipelineInfo.pipelineLayout);
  };

  void GetDefaultPipelineLayout(
      VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) const override;

  virtual void
  SetPipelineLayout(const VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo,
                    const VkDevice &device) override;
  virtual void DestroyPipelineLayout(const VkDevice &device) override;
  // OVERRIDE METHODS ---------------------------------------------------------
  // --------------------------------------------------------------------------
  //

  //
  // --------------------------------------------------------------------------
  // GFX PIPELINE METHODS -----------------------------------------------------
  virtual void
  GetDefaultDynamicState(VkPipelineDynamicStateCreateInfo &dynamicState) const;

  virtual void GetDefaultVertexInputState(
      VkPipelineVertexInputStateCreateInfo &vertexInputState) const;

  virtual void GetDefaultInputAssemblyState(
      VkPipelineInputAssemblyStateCreateInfo &inputAssemblyState) const;

  virtual void GetDefaultViewportState(
      VkPipelineViewportStateCreateInfo &viewportState) const;

  virtual void GetDefaultRasterizationState(
      VkPipelineRasterizationStateCreateInfo &rasterizationState) const;

  virtual void GetDefaultMultisampleState(
      VkPipelineMultisampleStateCreateInfo &multisampleState) const;

  virtual void GetDefaultDepthStencilState(
      VkPipelineDepthStencilStateCreateInfo &depthStencilState) const;

  virtual void
  GetDefaultViewportAndScissor(VkViewport &viewport, VkRect2D &scissor,
                               const VkExtent2D &swapchainExtent) const;

  virtual void GetDefaultColorBlendingState(
      VkPipelineColorBlendStateCreateInfo &colorBlendState) const;
  // GFX PIPELINE METHODS -----------------------------------------------------
  // --------------------------------------------------------------------------
  //
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
