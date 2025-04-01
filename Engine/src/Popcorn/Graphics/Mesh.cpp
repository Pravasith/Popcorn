#include "Mesh.h"
#include "Material.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

[[nodiscard]] uint32_t PC_GetHashedSubmeshId(VertexBuffer *vertexBuffer,
                                             IndexBuffer<uint16_t> *indexBuffer,
                                             Material *material) {
  uintptr_t vAddr = reinterpret_cast<uintptr_t>(vertexBuffer);
  uintptr_t iAddr = reinterpret_cast<uintptr_t>(indexBuffer);
  uintptr_t mAddr = reinterpret_cast<uintptr_t>(material);

  return static_cast<uint32_t>((vAddr ^ (iAddr >> 3) ^ (mAddr >> 6)) *
                               2654435761u); // Knuth's golden ratio
}

// Fires when the mesh is added to a Scene
void Mesh::OnAttach() {};

void Mesh::OnUpdate() {
  // Resets buffer data and fills again

  m_uniformBuffer.modelMatrix = m_matrix;
};

void Mesh::OnRender() {};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
