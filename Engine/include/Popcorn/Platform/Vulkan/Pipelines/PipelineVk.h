#pragma once

#include "BufferObjects.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "Shader.h"
#include <forward_list>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include "PipelineDefsVk.h"
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <PipelineTypes T> class PipelineVk {
public:
  [[nodiscard]] inline const VkPipeline &GetVkPipeline() const {
    return m_pipeline;
  };
  [[nodiscard]] inline const VkPipelineLayout &GetVkPipelineLayout() const {
    return m_pipelineLayout;
  };

  void SetShaderStagesMask(int enabledShaderStagesMask);

  virtual void Create(const BufferDefs::Layout &vertexBufferLayout,
                      const VkRenderPass &renderPass) = 0;
  void Destroy();

public:
  PipelineVk() { PC_PRINT("CREATED", TagType::Constr, "PipelineVk"); };
  virtual ~PipelineVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "PipelineVk");
  };

  using PipelineStateType = DerivePipelineCreateInfoType<T>::type;

protected:
  inline void RecordBindCmdPipelineCommand(const VkCommandBuffer &cmdBfr) {
    PC_VK_NULL_CHECK(cmdBfr)
    PC_VK_NULL_CHECK(m_pipeline)

    if constexpr (T == PipelineTypes::GraphicsType) {
      vkCmdBindPipeline(cmdBfr, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    } else if constexpr (T == PipelineTypes::ComputeType) {
    } else if constexpr (T == PipelineTypes::RaytracingType) {
    }
  };

  void CreateVkPipeline(const VkDevice &device,
                        const PipelineStateType &pipelineCreateInfo,
                        const VkRenderPass &renderPass);

  void CreateShaderStageCreateInfos(
      std::forward_list<VkShaderModule> &shaderModules);

  void CreatePipelineLayout(
      const VkDevice &device,
      const VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo);
  void DestroyPipelineLayout(const VkDevice &device);

protected:
  VkPipeline m_pipeline = VK_NULL_HANDLE;
  VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
  int m_enabledShaderStagesMask = ShaderStages::None;
  std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
