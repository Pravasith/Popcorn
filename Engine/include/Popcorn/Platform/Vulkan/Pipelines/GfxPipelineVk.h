
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

  virtual void CreatePipeline() override;

  virtual std::vector<VkPipelineShaderStageCreateInfo>
  CreateShaderStages(std::forward_list<VkShaderModule> shaderModules) override;

  virtual void ConfigureDynamicStates(
      VkPipelineDynamicStateCreateInfo &dynamicState) override;

  virtual void ConfigureVertexInputState(
      VkPipelineVertexInputStateCreateInfo &vertexInputState) override;

  virtual void ConfigureInputAssemblyState(
      VkPipelineInputAssemblyStateCreateInfo &inputAssemblyState) override;

  virtual void
  ConfigureViewportState(VkPipelineViewportStateCreateInfo &viewportState,
                         const VkExtent2D &extent) override;

  virtual void ConfigureRasterizationState(
      VkPipelineRasterizationStateCreateInfo &rasterizationState) override;

  virtual void ConfigureMultisampleState(
      VkPipelineMultisampleStateCreateInfo &multisampleState) override;

  virtual std::pair<VkViewport, VkRect2D>
  GetViewportAndScissor(const VkExtent2D &swapchainExtent) const override;

  virtual void
  ConfigureColorBlendingState(VkPipelineColorBlendStateCreateInfo &) override;

  // virtual void ConfigurePipelineLayout()
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
