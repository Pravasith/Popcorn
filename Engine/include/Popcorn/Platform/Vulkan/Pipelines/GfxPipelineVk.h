
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

  virtual void ConfigureDynamicStates() override;

  virtual std::vector<VkPipelineShaderStageCreateInfo>
  CreateShaderStages(std::forward_list<VkShaderModule> shaderModules) override;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
