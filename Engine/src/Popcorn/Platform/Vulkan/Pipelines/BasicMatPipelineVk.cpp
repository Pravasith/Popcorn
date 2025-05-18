#include "BasicMatPipelineVk.h"
#include "ContextVk.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "MaterialTypes.h"
#include "Shader.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void BasicMatPipelineVk::Create() {
  const VkDevice &device = ContextVk::Device()->GetDevice();

  const Buffer &vertShaderBuffer =
      Material<MaterialTypes::BasicMat>::GetShader(ShaderStages::VertexBit);
  const Buffer &fragShaderBuffer =
      Material<MaterialTypes::BasicMat>::GetShader(ShaderStages::FragmentBit);

  auto vertShaderModule = PC_CreateShaderModule(device, vertShaderBuffer);
  auto fragShaderModule = PC_CreateShaderModule(device, fragShaderBuffer);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
