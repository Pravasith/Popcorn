#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include "Popcorn/Core/Helpers.h"
#include <forward_list>
#include <variant>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
enum class PipelineTypes {
  None = 0,
  GraphicsType = 1,
  ComputeType,
  RaytracingType
};

enum ShaderStages {
  Null = 0,
  //
  // Graphics types
  Vertex = 1,
  Tesselation = shift_l(1),
  Geometry = shift_l(2),
  Fragment = shift_l(3),
  //
  // Compute types
  Compute = shift_l(4)
  //
  // Ray tracing types
  // TODO: Fill it out
};

class PipelineVk {
public:
  struct GfxTypeCreateInfo {
    VkDevice &device;
    VkExtent2D &swapchainExtent;
    VkPipelineDynamicStateCreateInfo &dynamicState;
    VkPipelineVertexInputStateCreateInfo &vertexInputStateCreateInfo;
    VkPipelineInputAssemblyStateCreateInfo &inputAssemblyStateCreateInfo;
    VkPipelineViewportStateCreateInfo &viewportStateCreateInfo;
    VkPipelineRasterizationStateCreateInfo &rasterizationStateCreateInfo;
    VkPipelineMultisampleStateCreateInfo &multisampleStateCreateInfo;
    VkPipelineColorBlendStateCreateInfo &colorBlendStateCreateInfo;
  };

  struct ComputeTypeCreateInfo {};

  struct RaytracingTypeCreateInfo {};

  PipelineVk(PipelineTypes type) {
    type_value = type;
    PC_PRINT("CREATED", TagType::Constr, "PipelineVk");
  };
  ~PipelineVk() { PC_PRINT("DESTROYED", TagType::Destr, "PipelineVk"); };

  void SetShaderStages(ShaderStages enabledShaderStagesMask);

  virtual std::vector<VkPipelineShaderStageCreateInfo>
  CreateShaderStages(std::forward_list<VkShaderModule> shaderModules) = 0;

  [[nodiscard]] static VkShaderModule
  CreateShaderModule(const VkDevice &device, const Buffer &bytecode);

  inline static void DestroyShaderModule(const VkDevice &device,
                                         VkShaderModule shaderModule) {
    vkDestroyShaderModule(device, shaderModule, nullptr);
  };

  using CreateInfoVariant =
      std::variant<const GfxTypeCreateInfo *, const ComputeTypeCreateInfo *,
                   const RaytracingTypeCreateInfo *>;

protected:
  virtual void Make(const CreateInfoVariant *) = 0;

  /** GRAPHICS PIPELINES HELPERS ------------------------------------------- *
   *  ---------------------------------------------------------------------- *
   *  ---------------------------------------------------------------------- */
  virtual void
  SetDefaultDynamicState(VkPipelineDynamicStateCreateInfo &dynamicState);

  virtual void SetDefaultVertexInputState(
      VkPipelineVertexInputStateCreateInfo &vertexInputState);

  virtual void SetDefaultInputAssemblyState(
      VkPipelineInputAssemblyStateCreateInfo &inputAssemblyState);

  virtual void
  SetDefaultViewportState(VkPipelineViewportStateCreateInfo &viewportState,
                          const VkExtent2D &extent);

  virtual void SetDefaultRasterizationState(
      VkPipelineRasterizationStateCreateInfo &rasterizationState);

  virtual void SetDefaultMultisampleState(
      VkPipelineMultisampleStateCreateInfo &multisampleState);

  virtual void SetDefaultDepthStencilState(
      VkPipelineDepthStencilStateCreateInfo &depthStencilState);

  virtual std::pair<VkViewport, VkRect2D>
  GetViewportAndScissor(const VkExtent2D &swapchainExtent) const;

  virtual void
  SetDefaultColorBlendingState(VkPipelineColorBlendStateCreateInfo &);

  virtual void SetDefaultPipelineLayout(const VkDevice &device);
  virtual void DestroyPipelineLayout(const VkDevice &device);

protected:
  ShaderStages m_enabledShaderStagesMask = ShaderStages::Null;
  // Defaults to Graphics pipeline
  PipelineTypes type_value = PipelineTypes::GraphicsType;
  VkPipelineLayout m_pipelineLayout;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
