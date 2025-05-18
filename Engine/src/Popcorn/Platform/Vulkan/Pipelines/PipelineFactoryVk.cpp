
#include "PipelineFactoryVk.h"
#include "BufferObjectsVk.h"
#include "ContextVk.h"
#include "DeviceVk.h"
#include "SwapchainVk.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <>
void PipelineFactoryVk::CreatePipeline<Pipelines::GBuffer>(
    const BufferDefs::Layout &vertexBufferLayout,
    const RenderPassVk &basicRenderPass) {

  auto *deviceVkStn = DeviceVk::Get();
  auto *swapchainVkStn = SwapchainVk::Get();

  auto &device = deviceVkStn->GetDevice();
  const auto &swapchainExtent = swapchainVkStn->GetSwapchainExtent();

  Buffer vertShaderBuffer = std::move(material->GetShaders()[0]);
  Buffer fragShaderBuffer = std::move(material->GetShaders()[1]);

  auto vertShaderModule = PC_CreateShaderModule(device, vertShaderBuffer);
  auto fragShaderModule = PC_CreateShaderModule(device, fragShaderBuffer);

  std::forward_list<VkShaderModule> shaderModules = {vertShaderModule,
                                                     fragShaderModule};

  // SET SHADER STAGES
  m_basePipeline.SetShaderStagesMask(static_cast<ShaderStages>(
      ShaderStages::VertexBit | ShaderStages::FragmentBit));

  m_basePipeline.CreateShaderStageCreateInfos(shaderModules);

  // SET FIXED FUNCTION PIPELINE STATE & SET LAYOUT
  GfxPipelineState pipelineState{};
  PipelineUtils::GetDefaultDynamicState(pipelineState.dynamicState);
  PipelineUtils::GetDefaultVertexInputState(pipelineState.vertexInputState);

  // Vertex input descriptions
  VkVertexInputBindingDescription bindingDescription{};
  VertexBufferVk::GetDefaultVertexInputBindingDescription(bindingDescription,
                                                          vertexBufferLayout);
  bindingDescription.binding = 0;
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
  VertexBufferVk::GetDefaultVertexInputAttributeDescriptions(
      attributeDescriptions, vertexBufferLayout);

  pipelineState.vertexInputState.vertexBindingDescriptionCount = 1;
  pipelineState.vertexInputState.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  pipelineState.vertexInputState.pVertexBindingDescriptions =
      &bindingDescription;
  pipelineState.vertexInputState.pVertexAttributeDescriptions =
      attributeDescriptions.data();

  PipelineUtils::GetDefaultInputAssemblyState(pipelineState.inputAssemblyState);
  PipelineUtils::GetDefaultViewportState(pipelineState.viewportState);
  PipelineUtils::GetDefaultRasterizationState(pipelineState.rasterizationState);
  PipelineUtils::GetDefaultMultisampleState(pipelineState.multisampleState);
  PipelineUtils::GetDefaultDepthStencilState(pipelineState.depthStencilState);
  PipelineUtils::GetDefaultColorBlendingState(pipelineState.colorBlendState);
  PipelineUtils::GetDefaultPipelineLayoutCreateInfo(
      pipelineState.pipelineLayout);

  auto descriptorFactoryVkStn = DescriptorFactoryVk::Get();

  const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts =
      ContextVk::DescriptorFactory()
          ->GetDescriptorSetLayouts<Pipelines::Base>();

  pipelineState.pipelineLayout.setLayoutCount = descriptorSetLayouts.size();
  pipelineState.pipelineLayout.pSetLayouts = descriptorSetLayouts.data();

  // CREATE PIPELINE LAYOUT
  m_basePipeline.CreatePipelineLayout(device, pipelineState.pipelineLayout);

  // CREATE VULKAN PIPELINE
  m_basePipeline.CreateVkPipeline(device, pipelineState,
                                  basicRenderPass.GetVkRenderPass());

  // DESTROY SHADER MODULES
  PC_DestroyShaderModule(device, vertShaderModule);
  PC_DestroyShaderModule(device, fragShaderModule);

  //
  // --- OTHER PIPELINE --------------------------------------------------
};

void PipelineFactoryVk::CleanUp() {
  if (m_lightingPipeline.GetVkPipeline() != VK_NULL_HANDLE) {
    m_lightingPipeline.CleanUp(ContextVk::Device()->GetDevice());
  };

  if (m_gBufferPipeline.GetVkPipeline() != VK_NULL_HANDLE) {
    m_gBufferPipeline.CleanUp(ContextVk::Device()->GetDevice());
  };
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
