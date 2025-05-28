#include "PipelineDefsVk.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "BufferObjectsVk.h"
#include "CompositePipelinesVk.h"
#include "ContextVk.h"
#include "GlobalMacros.h"
#include "PipelineUtilsVk.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void CompositePipelineVk::Create(const BufferDefs::Layout &vertexBufferLayout,
                                 const VkRenderPass &renderPass) {
  const VkDevice &device = ContextVk::Device()->GetDevice();
  ShaderLibrary *shaders = ContextVk::Shaders();

  Buffer *vertShaderBuffer =
      &shaders->GetShader<RendererType::Vulkan, ShaderFiles::Composite_Vert>();
  Buffer *fragShaderBuffer =
      &shaders->GetShader<RendererType::Vulkan, ShaderFiles::Composite_Frag>();

  auto vertShaderModule = PC_CreateShaderModule(device, *vertShaderBuffer);
  auto fragShaderModule = PC_CreateShaderModule(device, *fragShaderBuffer);

  std::forward_list<VkShaderModule> shaderModules = {vertShaderModule,
                                                     fragShaderModule};

  SetShaderStagesMask(ShaderStages::VertexBit | ShaderStages::FragmentBit);
  CreateShaderStageCreateInfos(shaderModules);

  VkVertexInputBindingDescription bindingDescription{};
  VertexBufferVk::GetDefaultVertexInputBindingDescription(vertexBufferLayout,
                                                          bindingDescription);
  bindingDescription.binding = 0;

  GfxPipelineState pipelineState{};
  PipelineUtilsVk::GetDefaultGfxPipelineState(
      vertexBufferLayout, bindingDescription, pipelineState);

  auto *layouts = ContextVk::DescriptorLayouts();
  VkDescriptorSetLayout &presentLayout =
      layouts->GetLayout<DescriptorSets::PresentSet>();

  std::array<VkDescriptorSetLayout, 1> dSetLayouts = {presentLayout};
  pipelineState.pipelineLayout.setLayoutCount = dSetLayouts.size();
  pipelineState.pipelineLayout.pSetLayouts = dSetLayouts.data();

  CreatePipelineLayout(device, pipelineState.pipelineLayout);
  CreateVkPipeline(device, pipelineState, renderPass);

  // DESTROY SHADER MODULES
  PC_DestroyShaderModule(device, vertShaderModule);
  PC_DestroyShaderModule(device, fragShaderModule);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
