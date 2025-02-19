#include "Mesh.h"
#include "MaterialHandler.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

const MaterialHandler *Mesh::s_materialHandler = MaterialHandler::Get();

// Fires when the mesh is added to a Scene
void Mesh::OnAttach(const SceneData &) {
  // PREPARE MATERIALS FOR RENDER
  s_materialHandler->RegisterMaterial(&m_material);
};

void Mesh::OnUpdate(const SceneData &) {};

void Mesh::OnRender(const SceneData &) {
  // if (m_material == nullptr) {
  //   PC_WARN("m_material is nullptr");
  //   return;
  // }

  // FIND MATERIALS & RENDER THEM WITH THE RENDERPASS INFORMATION
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
