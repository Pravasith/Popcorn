#include "Mesh.h"
#include "Material.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// Fires when the mesh is added to a Scene
void Mesh::OnAttach() {};

void Mesh::OnUpdate() {
  // Resets buffer data and fills again
  m_uniformBuffer.modelMatrix = GetWorldMatrix();
};

void Mesh::OnRender() {};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
