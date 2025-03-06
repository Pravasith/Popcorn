#pragma once

#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "RenderPassVk.h"
#include "RenderWorkflowVk.h"
#include "Scene.h"
#include "VertexBuffer.h"
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class BasicRenderWorkflowVk : public RenderWorkflowVk {
public:
  BasicRenderWorkflowVk() {
    s_vertexBufferLayout.Set<VertexBuffer::AttrTypes::Float2,
                             VertexBuffer::AttrTypes::Float3>();
    PC_PRINT("CREATED", TagType::Constr, "BasicWorkflowVk")
  };
  virtual ~BasicRenderWorkflowVk() override {
    PC_PRINT("DESTROYED", TagType::Destr, "BasicWorkflowVk")
  };

  [[nodiscard]] inline static const VertexBuffer::Layout &
  GetBasicWorkflowVertexLayout() {
    return s_vertexBufferLayout;
  }

  [[nodiscard]] virtual const VkRenderPass &GetRenderPass() const override {
    return m_basicRenderPassVk.GetVkRenderPass();
  };

  virtual void CreateRenderPass() override;
  virtual void CreatePipeline(Material &) override;
  virtual void CreateFramebuffers() override;
  virtual void AllocateVkVertexBuffers() override;
  virtual void AddMeshToWorkflow(Mesh *mesh) override;
  virtual void RecordRenderCommands(const Scene &scene,
                                    const VkCommandBuffer &commandBuffer,
                                    const uint32_t imageIndex) override;
  virtual void CleanUp() override;

private:
  static VertexBuffer::Layout s_vertexBufferLayout;

  std::vector<VkDeviceSize> m_vkBufferOffsets;

  VkBuffer m_vkVertexBuffer;
  VkDeviceMemory m_vkVertexBufferMemory;

  RenderPassVk m_basicRenderPassVk;
  GfxPipelineVk m_colorPipelineVk;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
