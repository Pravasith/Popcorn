#pragma once

#include "GlobalMacros.h"
#include "PipelineVk.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class BasicMatPipelineVk : public PipelineVk<PipelineTypes::GraphicsType> {
public:
  BasicMatPipelineVk() {
    PC_PRINT("CREATED", TagType::Constr, "GBufferPipelineVk");
  };
  ~BasicMatPipelineVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "GBufferPipelineVk");
  };

  virtual void Create() override;
  virtual void Destroy(const VkDevice &device) override;

private:
  virtual void CreateVkPipeline(const VkDevice &device,
                                const PipelineStateType &pipelineCreateInfo,
                                const VkRenderPass &renderPass) override;

  virtual void CreateShaderStageCreateInfos(
      std::forward_list<VkShaderModule> &shaderModules) override;

  virtual void CreatePipelineLayout(
      const VkDevice &device,
      const VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) override;
  virtual void DestroyPipelineLayout(const VkDevice &device) override;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
