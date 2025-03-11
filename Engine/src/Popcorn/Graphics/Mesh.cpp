#include "Mesh.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void Mesh::ValidateMembersWithSpec(const Spec &spec) {
#ifdef PC_DEBUG
  if (spec.enableIndexBuffers && m_indexBuffer == nullptr) {
    PC_ERROR("Index buffer is enabled but it's empty", "Mesh")
  };
#endif
};

// Fires when the mesh is added to a Scene
void Mesh::OnAttach() {};

void Mesh::OnUpdate() {
  // Resets buffer data and fills again
  m_uniformBuffer.Fill({m_matrix});
};

void Mesh::OnRender() {};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
