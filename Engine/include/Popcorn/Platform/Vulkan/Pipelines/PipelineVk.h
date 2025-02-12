#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include "Popcorn/Core/Helpers.h"
#include <forward_list>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class PipelineTypes { GraphicsType = 1, ComputeType, RaytracingType };

enum ShaderStages {
  None = 0,
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

struct GfxPipelineCreateInfo {
  constexpr static PipelineTypes type_value = PipelineTypes::GraphicsType;

  VkExtent2D swapchainExtent = {};
  VkPipelineDynamicStateCreateInfo dynamicState = {};
  VkPipelineVertexInputStateCreateInfo vertexInputState = {};
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
  VkPipelineViewportStateCreateInfo viewportState = {};
  VkPipelineRasterizationStateCreateInfo rasterizationState = {};
  VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
  VkPipelineMultisampleStateCreateInfo multisampleState = {};
  VkPipelineColorBlendStateCreateInfo colorBlendState = {};
  VkPipelineLayoutCreateInfo pipelineLayout = {};
};

struct ComputePipelineCreateInfo {
  constexpr static PipelineTypes type_value = PipelineTypes::ComputeType;
  std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;

  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkPipelineCache pipelineCache = VK_NULL_HANDLE;
  VkPipelineCreateFlags flags = 0;
  VkPipeline basePipelineHandle = VK_NULL_HANDLE;
  int32_t basePipelineIndex = -1;
};

struct RaytracingPipelineCreateInfo {
  constexpr static PipelineTypes type_value = PipelineTypes::RaytracingType;
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
  std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups;
  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  uint32_t maxRecursionDepth = 1;
  VkPipelineCache pipelineCache = VK_NULL_HANDLE;
  VkPipelineCreateFlags flags = 0;
  VkPipelineLibraryCreateInfoKHR libraryInfo = {};
};

// PRIMARY TEMPLATE
template <PipelineTypes T> struct DerivePipelineCreateInfoType;

// SPECIALIZATION TEMPLATES
template <> struct DerivePipelineCreateInfoType<PipelineTypes::GraphicsType> {
  using type = GfxPipelineCreateInfo;
};

template <> struct DerivePipelineCreateInfoType<PipelineTypes::ComputeType> {
  using type = ComputePipelineCreateInfo;
};

template <> struct DerivePipelineCreateInfoType<PipelineTypes::RaytracingType> {
  using type = RaytracingPipelineCreateInfo;
};

//
// ----------------------------------------------------------------------------
// STATIC SHADER HELPERS ------------------------------------------------------
[[nodiscard]] static VkShaderModule
PC_CreateShaderModule(const VkDevice &device, const Buffer &bytecode) {
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

inline static void PC_DestroyShaderModule(const VkDevice &device,
                                          VkShaderModule shaderModule) {
  vkDestroyShaderModule(device, shaderModule, nullptr);
};
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//

template <PipelineTypes T> class PipelineVk {
public:
  PipelineVk() {
    type_value = T;
    PC_PRINT("CREATED", TagType::Constr, "PipelineVk");
  };
  virtual ~PipelineVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "PipelineVk");
  };

  using CreateInfo_type = DerivePipelineCreateInfoType<T>::type;

  virtual void Create(const CreateInfo_type &createInfo) = 0;

  virtual void GetDefaultPipelineCreateInfo(CreateInfo_type &createInfo) = 0;

  // Shaders
  void SetShaderStagesMask(ShaderStages enabledShaderStagesMask) {
    PC_PRINT((int)enabledShaderStagesMask, TagType::Print, "PipelineVk")

    // Error check
    switch (type_value) {
    // case PipelineTypes::None:
    //   PC_ERROR("PipelineType not set yet!!", "PipelineVk")
    //   break;
    case PipelineTypes::GraphicsType:
      if ((enabledShaderStagesMask &
           (ShaderStages::Vertex | ShaderStages::Fragment)) !=
          (ShaderStages::Vertex | ShaderStages::Fragment)) {
        PC_ERROR(
            "Either vertex shader or fragment shader or both are not enabled",
            "PipelineVk")
      };
      break;
    case PipelineTypes::ComputeType:
      if (!(enabledShaderStagesMask & ShaderStages::Compute))
        PC_ERROR("Compute shader is not enabled", "PipelineVk")
      break;
    case PipelineTypes::RaytracingType:
      // TODO: Fill it out
      break;
    }

    m_enabledShaderStagesMask = enabledShaderStagesMask;
  };

  virtual std::vector<VkPipelineShaderStageCreateInfo>
  CreateShaderStages(std::forward_list<VkShaderModule> &shaderModules) = 0;

  virtual void SetShaderStageCreateInfos(
      std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos) {
    m_shaderStageCreateInfos = shaderStageCreateInfos;
  };

  inline void SetDevice(const VkDevice &device) {
    PC_VK_NULL_CHECK(device)
    m_device = device;
  };

  // Layout
  //
  virtual void GetDefaultPipelineLayout(
      VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) const = 0;

  virtual void SetPipelineLayout(
      const VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) = 0;
  virtual void DestroyPipelineLayout(const VkDevice &device) = 0;

protected:
  PipelineTypes type_value = PipelineTypes::GraphicsType;
  VkDevice m_device = VK_NULL_HANDLE;

  VkPipeline m_pipeline = VK_NULL_HANDLE;
  VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
  ShaderStages m_enabledShaderStagesMask = ShaderStages::None;
  std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
