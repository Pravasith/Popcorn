#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include "Popcorn/Core/Helpers.h"
#include "Shader.h"
#include "Sources.h"
#include <forward_list>
#include <type_traits>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum PipelineTypes { GraphicsType = 1, ComputeType, RaytracingType };

struct GfxPipelineState {
  static constexpr PipelineTypes type_value = PipelineTypes::GraphicsType;

  VkExtent2D swapchainExtent{};
  VkPipelineDynamicStateCreateInfo dynamicState{};
  VkPipelineVertexInputStateCreateInfo vertexInputState{};
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
  VkPipelineViewportStateCreateInfo viewportState{};
  VkPipelineRasterizationStateCreateInfo rasterizationState{};
  VkPipelineDepthStencilStateCreateInfo depthStencilState{};
  VkPipelineMultisampleStateCreateInfo multisampleState{};
  VkPipelineColorBlendStateCreateInfo colorBlendState{};
  VkPipelineLayoutCreateInfo pipelineLayout{};
};

struct ComputePipelineState {
  static constexpr PipelineTypes type_value = PipelineTypes::ComputeType;

  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkPipelineCache pipelineCache = VK_NULL_HANDLE;
  VkPipelineCreateFlags flags = 0;
  VkPipeline basePipelineHandle = VK_NULL_HANDLE;
  int32_t basePipelineIndex = -1;
};

struct RaytracingPipelineState {
  static constexpr PipelineTypes type_value = PipelineTypes::RaytracingType;

  std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups{};
  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  uint32_t maxRecursionDepth = 1;
  VkPipelineCache pipelineCache = VK_NULL_HANDLE;
  VkPipelineCreateFlags flags = 0;
  VkPipelineLibraryCreateInfoKHR libraryInfo{};
};

// PRIMARY TEMPLATE
template <PipelineTypes T> struct DerivePipelineCreateInfoType;

// SPECIALIZATION TEMPLATES
template <> struct DerivePipelineCreateInfoType<PipelineTypes::GraphicsType> {
  using type = GfxPipelineState;
};

template <> struct DerivePipelineCreateInfoType<PipelineTypes::ComputeType> {
  using type = ComputePipelineState;
};

template <> struct DerivePipelineCreateInfoType<PipelineTypes::RaytracingType> {
  using type = RaytracingPipelineState;
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

  using PipelineStateType = DerivePipelineCreateInfoType<T>::type;

  [[nodiscard]] inline const VkPipeline GetVkPipeline() const {
    return m_pipeline;
  };

  [[nodiscard]] inline const VkPipelineLayout GetVkPipelineLayout() const {
    return m_pipelineLayout;
  };

  // Shaders
  void SetShaderStagesMask(int enabledShaderStagesMask) {
    // Error check
    switch (type_value) {
    // case PipelineTypes::None:
    //   PC_ERROR("PipelineType not set yet!!", "PipelineVk")
    //   break;
    case PipelineTypes::GraphicsType:
      if ((enabledShaderStagesMask &
           (ShaderStages::VertexBit | ShaderStages::FragmentBit)) !=
          (ShaderStages::VertexBit | ShaderStages::FragmentBit)) {
        PC_ERROR(
            "Either vertex shader or fragment shader or both are not enabled",
            "PipelineVk")
      };
      break;
    case PipelineTypes::ComputeType:
      if (!(enabledShaderStagesMask & ShaderStages::ComputeBit))
        PC_ERROR("Compute shader is not enabled", "PipelineVk")
      break;
    case PipelineTypes::RaytracingType:
      // TODO: Fill it out
      break;
    }

    m_enabledShaderStagesMask = enabledShaderStagesMask;
  };

  virtual void CreateShaderStageCreateInfos(
      std::forward_list<VkShaderModule> &shaderModules) = 0;

  //
  // --- PIPELINES ------------------------------------------------------------
  virtual void CreateVkPipeline(const VkDevice &device,
                                const PipelineStateType &pipelineCreateInfo,
                                const VkRenderPass &renderPass) = 0;
  virtual void CleanUp(const VkDevice &) = 0;

  //
  // --- LAYOUTS --------------------------------------------------------------
  virtual void CreatePipelineLayout(
      const VkDevice &device,
      const VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) = 0;
  virtual void DestroyPipelineLayout(const VkDevice &device) = 0;

  inline void RecordBindCmdPipelineCommand(const VkCommandBuffer &cmdBfr) {
    PC_VK_NULL_CHECK(cmdBfr)
    PC_VK_NULL_CHECK(m_pipeline)

    // Bind pipeline
    vkCmdBindPipeline(cmdBfr, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
  };

protected:
  PipelineTypes type_value = PipelineTypes::GraphicsType;

  VkPipeline m_pipeline = VK_NULL_HANDLE;
  VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
  int m_enabledShaderStagesMask = ShaderStages::None;
  std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
};

class PipelineUtils {
public:
  static void
  GetDefaultDynamicState(VkPipelineDynamicStateCreateInfo &dynamicState);

  static void GetDefaultVertexInputState(
      VkPipelineVertexInputStateCreateInfo &vertexInputState);

  static void GetDefaultInputAssemblyState(
      VkPipelineInputAssemblyStateCreateInfo &inputAssemblyState);

  static void
  GetDefaultViewportState(VkPipelineViewportStateCreateInfo &viewportState);

  static void GetDefaultRasterizationState(
      VkPipelineRasterizationStateCreateInfo &rasterizationState);

  static void GetDefaultMultisampleState(
      VkPipelineMultisampleStateCreateInfo &multisampleState);

  static void GetDefaultDepthStencilState(
      VkPipelineDepthStencilStateCreateInfo &depthStencilState);

  static void
  GetDefaultViewportAndScissorState(VkViewport &viewport, VkRect2D &scissor,
                                    const VkExtent2D &swapchainExtent);

  static void GetDefaultColorBlendingState(
      VkPipelineColorBlendStateCreateInfo &colorBlendState);

  static void GetDefaultPipelineLayoutCreateInfo(
      VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo);

private:
  static std::vector<VkDynamicState> s_dynamicStatesDefault;
  static VkPipelineColorBlendAttachmentState s_colorBlendAttachmentDefault;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
