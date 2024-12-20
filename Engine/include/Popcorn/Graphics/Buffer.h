#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
class VertexBuffer {
public:
  VertexBuffer() { PC_PRINT("CREATED", TagType::Constr, "VERT-BFR") };
  virtual ~VertexBuffer() { PC_PRINT("DESTROYED", TagType::Destr, "VERT-BFR") };

  virtual void Alloc() = 0;
};

ENGINE_NAMESPACE_END
