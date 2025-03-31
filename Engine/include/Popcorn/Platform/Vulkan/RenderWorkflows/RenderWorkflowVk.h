#pragma once

#include "GlobalMacros.h"
#include "Material.h"
#include "Mesh.h"
#include "PipelineFactoryVk.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class RenderWorkflowIndices {
  Basic = 0,   // Basic colors
  Shadows = 1, // Simple hard shadows
};

class RenderWorkflowVk {
public:
  RenderWorkflowVk() {
    m_pipelineFactory = PipelineFactoryVk::Get();
    PC_PRINT("CREATED", TagType::Constr, "RenderWorkflowVk")
  };
  virtual ~RenderWorkflowVk() {
    m_meshes.clear();
    m_basicMaterials.clear();

    PipelineFactoryVk::Destroy();
    m_pipelineFactory = nullptr;
    PC_PRINT("DESTROYED", TagType::Destr, "RenderWorkflowVk")
  };

  virtual const VkRenderPass &GetRenderPass() const {
    throw std::runtime_error(
        "GetRenderPass is not defined in the inherited class");
  };

  virtual void
  RecordRenderCommands(const uint32_t frameIndex, const uint32_t currentFrame,
                       VkCommandBuffer &currentFrameCommandBuffer) = 0;
  virtual void ProcessSceneUpdates(const uint32_t currentFrame) = 0;
  virtual void CreatePipelines() = 0;
  virtual void CleanUp() = 0;

  //
  // WORKFLOW UTILS
  virtual void CreateRenderPass() {};
  virtual void CreateFramebuffers() {};
  virtual void AllocateVkVertexBuffers() {};
  virtual void AllocateVkIndexBuffers() {};
  virtual void AllocateVkUniformBuffers() {};
  virtual void CreateCommandBuffer() {};
  virtual void CreateDescriptorSetLayouts() {};
  virtual void CreateDescriptorPool() {};
  virtual void CreateDescriptorSets() {};

  virtual void AddMeshToWorkflow(Mesh *mesh) {};

  // UTILS
  template <MaterialTypes T> void AddMaterialByType(Material<T> *materialPtr) {
    switch (materialPtr->GetMaterialType()) {
    case Popcorn::Gfx::MaterialTypes::BasicMat: {
      PC_ValidateAndAddMaterial(materialPtr, m_basicMaterials);
      break;
    }
    case Popcorn::Gfx::MaterialTypes::PbrMat: {
      PC_ValidateAndAddMaterial(materialPtr, m_pbrMaterials);
      break;
    }
    default:
      PC_WARN("material type not found") { break; }
    }
  }

  template <MaterialTypes T>
  void RemoveMaterialByType(Material<T> *materialPtr) {
    switch (materialPtr->GetMaterialType()) {
    case Popcorn::Gfx::MaterialTypes::BasicMat: {
      PC_ValidateAndRemoveMaterial(materialPtr, m_basicMaterials);
      break;
    }
    case Popcorn::Gfx::MaterialTypes::PbrMat: {
      PC_ValidateAndRemoveMaterial(materialPtr, m_pbrMaterials);
      break;
    }
    default:
      PC_WARN("material type not found") { break; }
    }
  }

  template <MaterialTypes T> void RegisterMaterial(Material<T> *materialPtr) {
    PC_AddMaterialByType(materialPtr, m_basicMaterials);
  }

  template <MaterialTypes T> void UnRegisterMaterial(Material<T> *materialPtr) {
    PC_RemoveMaterialByType(materialPtr, m_basicMaterials);
  }

protected:
  std::vector<Mesh *> m_meshes;

  std::vector<Material<MaterialTypes::BasicMat> *> m_basicMaterials;
  std::vector<Material<MaterialTypes::PbrMat> *> m_pbrMaterials;

  PipelineFactoryVk *m_pipelineFactory;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
