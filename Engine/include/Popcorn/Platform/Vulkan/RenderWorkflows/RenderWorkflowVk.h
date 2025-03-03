#pragma once

#include "GlobalMacros.h"
#include "Material.h"
#include "Mesh.h"
#include "Popcorn/Core/Base.h"
#include "Scene.h"
#include "VertexBuffer.h"
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
  virtual void CleanUp() = 0;

  virtual void CreatePipeline(Material &) {};

  //
  // WORKFLOW UTILS
  virtual void CreateRenderPass() {};
  virtual void CreateFramebuffers() {};
  virtual void CreateCommandBuffer() {};

  void AddMeshToWorkflow(Mesh *mesh) {
    auto ptr = std::find(m_meshes.begin(), m_meshes.end(), mesh);

    if (ptr != m_meshes.end()) {
      PC_WARN("Mesh already exists in m_meshes!")
      return;
    };

    auto &material = mesh->GetMaterial();

    if (material.GetMaterialType() != MaterialTypes::BasicMat) {
      PC_ERROR("Attempt to add a mesh to basicRenderWorkflow that is not "
               "MaterialTypes::BasicMat",
               "BasicRenderWorkflow");
      return;
    };

    m_meshes.push_back(mesh);

    // Each material can potentially have the same material type but different
    // descriptor sets (shaders, textures ..etc)
    RegisterMaterial(&material);
  };

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
