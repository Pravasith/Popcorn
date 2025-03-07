#pragma once

#include "GlobalMacros.h"
#include "Material.h"
#include "Mesh.h"
#include "Popcorn/Core/Base.h"
#include "Scene.h"
#include "BufferObjects.h"
#include "VertexBufferVk.h"
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Material;

enum class RenderWorkflowIndices {
  Basic = 0,   // Basic colors
  Shadows = 1, // Simple hard shadows
};

class RenderWorkflowVk {
public:
  RenderWorkflowVk() {
    PC_PRINT("CREATED", TagType::Constr, "RenderWorkflowVk")
  };
  virtual ~RenderWorkflowVk() {
    m_meshes.clear();
    m_materials.clear();

    PC_PRINT("DESTROYED", TagType::Destr, "RenderWorkflowVk")
  };

  virtual void RecordRenderCommands(const Scene &scene,
                                    const VkCommandBuffer &commandBuffer,
                                    const uint32_t imageIndex) = 0;
  virtual const VkRenderPass &GetRenderPass() const {
    throw std::runtime_error(
        "GetRenderPass is not defined in the inherited class");
  };

  virtual void CreatePipeline(Material &) = 0;
  virtual void CleanUp() = 0;

  //
  // WORKFLOW UTILS
  virtual void CreateRenderPass() {};
  virtual void CreateFramebuffers() {};
  virtual void AllocateVkVertexBuffers() {};
  virtual void AllocateVkIndexBuffers() {};
  virtual void CreateCommandBuffer() {};

  virtual void AddMeshToWorkflow(Mesh *mesh) {};

  void RegisterMaterial(Material *materialPtr) {
    auto ptr = std::find(m_materials.begin(), m_materials.end(), materialPtr);

    if (ptr != m_materials.end()) {
      PC_WARN("Material already exists in the material library!")
      return;
    };

    //
    // ADD TO MATERIAL LIBRARY ---------------------------------------------
    m_materials.push_back(materialPtr);
    //
    // CREATE MATERIAL PIPELINES -------------------------------------------
    CreatePipeline(*materialPtr);
  }

  void UnRegisterMaterial(Material *materialPtr) {
    auto ptr = std::find(m_materials.begin(), m_materials.end(), materialPtr);

    if (ptr == m_materials.end()) {
      PC_WARN("Material doesn't exist in the material library!")
      return;
    };

    m_materials.erase(ptr);
  }

protected:
  std::vector<Mesh *> m_meshes;
  std::vector<Material *> m_materials;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
