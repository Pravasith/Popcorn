#pragma once

#include "BufferObjects.h"
#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "RenderPassVk.h"
#include "RenderWorkflowVk.h"
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class BasicRenderWorkflowVk : public RenderWorkflowVk {
public:
  BasicRenderWorkflowVk() {
    s_vertexBufferLayout
        .Set<BufferDefs::AttrTypes::Float2, BufferDefs::AttrTypes::Float3>();
    PC_PRINT("CREATED", TagType::Constr, "BasicWorkflowVk")
  };
  virtual ~BasicRenderWorkflowVk() override {
    PC_PRINT("DESTROYED", TagType::Destr, "BasicWorkflowVk")
  };

  [[nodiscard]] inline static const BufferDefs::Layout &
  GetBasicWorkflowVertexLayout() {
    return s_vertexBufferLayout;
  }

  [[nodiscard]] virtual const VkRenderPass &GetRenderPass() const override {
    return m_basicRenderPassVk.GetVkRenderPass();
  };

  virtual void AddMeshToWorkflow(Mesh *mesh) override;

  virtual void
  RecordRenderCommands(const uint32_t frameIndex, const uint32_t currentFrame,
                       VkCommandBuffer &currentFrameCommandBuffer) override;
  virtual void ProcessSceneUpdates(const uint32_t currentFrame) override;

  virtual void CreateRenderPass() override;
  virtual void CreatePipelines() override;
  virtual void CreateFramebuffers() override;
  virtual void AllocateVkVertexBuffers() override;
  virtual void AllocateVkIndexBuffers() override;
  virtual void AllocateVkUniformBuffers() override;
  virtual void CreateDescriptorSetLayouts() override;
  virtual void CreateDescriptorPool() override;
  virtual void CreateDescriptorSets() override;

  virtual void CleanUp() override;

private:
  static BufferDefs::Layout s_vertexBufferLayout;

  RenderPassVk m_basicRenderPassVk;
  GfxPipelineVk m_colorPipelineVk;

  std::vector<VkDeviceSize> m_vertexBufferOffsets;
  std::vector<VkDeviceSize> m_indexBufferOffsets;
  std::vector<VkDeviceSize> m_uniformBufferOffsets;

  VkBuffer m_vkVertexBuffer;
  VkDeviceMemory m_vkVertexBufferMemory;

  VkBuffer m_vkIndexBuffer;
  VkDeviceMemory m_vkIndexBufferMemory;

  std::vector<VkBuffer> m_uniformBuffers;
  std::vector<VkDeviceMemory> m_uniformBuffersMemory;
  std::vector<void *> m_uniformBuffersMapped;

  // TODO: Make this a global
  UniformBuffer m_viewProjUBO;

  // TODO: Move to global resources (and index into it whenever needed)
  VkDescriptorPool m_descriptorPool;

  VkDescriptorSetLayout m_globalUBOsDSetLayout;
  VkDescriptorSetLayout m_localUBOsDSetLayout;

  std::vector<VkDescriptorSet> m_globalDescriptorSets;
  std::vector<VkDescriptorSet> m_localDescriptorSets;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
