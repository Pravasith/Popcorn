
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include "Popcorn/Core/Helpers.h"
#include <cstdint>
#include <forward_list>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class PipelineVk {
public:
  enum Types { GraphicsType = 1, ComputeType, RaytracingType };

  enum ShaderStages {
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

  PipelineVk(Types type) {
    type_value = type;
    PC_PRINT("CREATED", TagType::Constr, "PipelineVk");
  };
  ~PipelineVk() { PC_PRINT("DESTROYED", TagType::Destr, "PipelineVk"); };

  inline void SetShaderStages(int enabledShaderStagesMask) {
    // Error check
    switch (type_value) {
    case GraphicsType:
      if (enabledShaderStagesMask &
          (ShaderStages::Vertex | ShaderStages::Fragment)) {
        PC_ERROR(
            "Either vertex shader or fragment shader or both are not enabled",
            "PipelineVk")
      };
      break;
    case ComputeType:
      if (enabledShaderStagesMask & ShaderStages::Compute)
        PC_ERROR("Compute shader is not enabled", "PipelineVk")
      break;
    case RaytracingType:
      // TODO: Fill it out
      break;
    }

    enabledShaderStagesMask = m_enabledShaderStagesMask;
  };

  inline static VkShaderModule CreateShaderModule(const VkDevice &device,
                                                  const Buffer &bytecode) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = bytecode.GetSize();
    createInfo.pCode = bytecode.AsType<uint32_t>();

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
  };

  inline static void DestroyShaderModule(const VkDevice &device,
                                         VkShaderModule shaderModule) {
    vkDestroyShaderModule(device, shaderModule, nullptr);
  };

protected:
  virtual void CreatePipeline() = 0;

  virtual std::vector<VkPipelineShaderStageCreateInfo>
  CreateShaderStages(std::forward_list<VkShaderModule> shaderModules) = 0;

  /** graphics pipelines helpers ------------------------------------------- */
  virtual void
  ConfigureDynamicStates(VkPipelineDynamicStateCreateInfo &dynamicState);

  virtual void ConfigureVertexInputState(
      VkPipelineVertexInputStateCreateInfo &vertexInputState);

  virtual void ConfigureInputAssemblyState(
      VkPipelineInputAssemblyStateCreateInfo &inputAssemblyState);

  virtual void
  ConfigureViewportState(VkPipelineViewportStateCreateInfo &viewportState,
                         const VkExtent2D &extent);

  virtual void ConfigureRasterizationState(
      VkPipelineRasterizationStateCreateInfo &rasterizationState);

  virtual void ConfigureMultisampleState(
      VkPipelineMultisampleStateCreateInfo &multisampleState);

  virtual std::pair<VkViewport, VkRect2D>
  GetViewportAndScissor(const VkExtent2D &swapchainExtent) const;

  // TODO: Color blending

protected:
  int m_enabledShaderStagesMask = 0;
  // Defaults to Graphics pipeline
  uint16_t type_value = (uint16_t)Types::GraphicsType;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
