#include "Mesh.h"
#include "Material.h"
#include "SceneObjectsHandler.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

const SceneObjectsHandler *Mesh::s_meshHandler = SceneObjectsHandler::Get();

// Fires when the mesh is added to a Scene
void Mesh::OnAttach(const SceneData &) {
  s_meshHandler->RegisterMaterial(&m_material);
  // s_meshHandler->PrepareMeshForRender(this);
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
