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
  ~GfxPipelineVk() {
    Destroy();

    PC_PRINT("DESTROYED", TagType::Destr, "GfxPipelineVk");
  };

  virtual void Create(const CreateInfo_type &pipelineCreateInfo) override {
    if (m_pipeline != VK_NULL_HANDLE) {
      PC_WARN("Attempt to create GfxPipeline when it already exists")
      return;
    };

    GfxPipelineCreateInfo createInfo = pipelineCreateInfo;
    if (m_shaderStageCreateInfos.size() == 0) {
      PC_WARN("No shader stages set")
    };

    PC_VK_NULL_CHECK(m_pipelineLayout);
    PC_VK_NULL_CHECK(m_renderPass);

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

    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                  nullptr, &m_pipeline) != VK_SUCCESS) {
      throw std::runtime_error("failed to create graphics pipeline!");
    }
  };

  virtual void Destroy() override {
    PC_VK_NULL_CHECK(m_device)
    DestroyPipelineLayout(m_device);

    if (m_pipeline != VK_NULL_HANDLE) {
      vkDestroyPipeline(m_device, m_pipeline, nullptr);
      m_pipeline = VK_NULL_HANDLE;
      m_device = VK_NULL_HANDLE;
    };
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

  virtual void SetPipelineLayout(
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
