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
                                const PiplelineStateType &pipelineCreateInfo,
                                const VkRenderPass &renderPass) override {
    if (m_pipeline != VK_NULL_HANDLE) {
      PC_WARN("Attempt to create GfxPipeline when it already exists")
      return;
    };

    GfxPipelineState createInfo = pipelineCreateInfo;
    if (m_shaderStageCreateInfos.size() == 0) {
      PC_WARN("No shader stages set")
    };

    PC_VK_NULL_CHECK(m_pipelineLayout);

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pipelineInfo.stageCount = m_shaderStageCreateInfos.size();
    pipelineInfo.pStages = m_shaderStageCreateInfos.data();

    pipelineInfo.pVertexInputState = &createInfo.vertexInputState;
    pipelineInfo.pInputAssemblyState = &createInfo.inputAssemblyState;
    pipelineInfo.pViewportState = &createInfo.viewportState;
    pipelineInfo.pRasterizationState = &createInfo.rasterizationState;
    // pipelineInfo.pDepthStencilState = &createInfo.depthStencilState;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pMultisampleState = &createInfo.multisampleState;
    pipelineInfo.pColorBlendState = &createInfo.colorBlendState;
    pipelineInfo.pDynamicState = &createInfo.dynamicState;

    pipelineInfo.layout = m_pipelineLayout;

    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                  nullptr, &m_pipeline) != VK_SUCCESS) {
      throw std::runtime_error("failed to create graphics pipeline!");
    }
  };

  virtual void Destroy(const VkDevice &device) override {
    PC_VK_NULL_CHECK(device)
    PC_VK_NULL_CHECK(m_pipeline)
    DestroyPipelineLayout(device);

    vkDestroyPipeline(device, m_pipeline, nullptr);
    m_pipeline = VK_NULL_HANDLE;
  };

  //
  // --------------------------------------------------------------------------
  // OVERRIDE METHODS ---------------------------------------------------------
  [[nodiscard]] virtual std::vector<VkPipelineShaderStageCreateInfo>
  CreateShaderStages(std::forward_list<VkShaderModule> &shaderModules) override;

  inline virtual void
  GetDefaultPipelineState(PiplelineStateType &pipelineState) override {
    auto &createGfxPipelineInfo =
        static_cast<GfxPipelineState &>(pipelineState);

    GetDefaultDynamicState(createGfxPipelineInfo.dynamicState);
    GetDefaultVertexInputState(createGfxPipelineInfo.vertexInputState);
    GetDefaultInputAssemblyState(createGfxPipelineInfo.inputAssemblyState);
    GetDefaultViewportState(createGfxPipelineInfo.viewportState);
    GetDefaultRasterizationState(createGfxPipelineInfo.rasterizationState);
    GetDefaultMultisampleState(createGfxPipelineInfo.multisampleState);
    GetDefaultDepthStencilState(createGfxPipelineInfo.depthStencilState);
    GetDefaultColorBlendingState(createGfxPipelineInfo.colorBlendState);
    GetDefaultPipelineLayoutCreateInfo(createGfxPipelineInfo.pipelineLayout);
  };

  void GetDefaultPipelineLayoutCreateInfo(
      VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) const override;

  virtual void SetPipelineLayout(
      const VkDevice &device,
      const VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) override;
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
  GetDefaultViewportAndScissorState(VkViewport &viewport, VkRect2D &scissor,
                                    const VkExtent2D &swapchainExtent) const;

  virtual void GetDefaultColorBlendingState(
      VkPipelineColorBlendStateCreateInfo &colorBlendState);

  // GFX PIPELINE METHODS -----------------------------------------------------
  // --------------------------------------------------------------------------
  //

private:
  std::vector<VkDynamicState> m_dynamicStatesDefault{VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineColorBlendAttachmentState m_colorBlendAttachmentDefault{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
