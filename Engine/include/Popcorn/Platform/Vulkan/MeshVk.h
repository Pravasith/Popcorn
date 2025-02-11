#pragma once

#include "Mesh.h"

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class MeshVk : public Mesh {
  MeshVk() { PC_PRINT("CREATED", TagType::Constr, "MESH-VK"); };
  ~MeshVk() { PC_PRINT("DESTROYED", TagType::Destr, "MESH-VK"); };

  // SETTERS & GETTERS -------------------------------------------------
  // Vertex data - geometry, uv, normals ... etc
  // Material data - shaders
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
