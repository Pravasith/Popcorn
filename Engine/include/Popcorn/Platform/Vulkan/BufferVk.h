#pragma once

#include "Buffer.h"
#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN
class VertexBufferVk : public VertexBuffer {
public:
  VertexBufferVk() { PC_PRINT("CREATED", TagType::Constr, "VERT-BFR-VK") };
  ~VertexBufferVk() { PC_PRINT("DESTROYED", TagType::Destr, "VERT-BFR-VK") };

  virtual void Alloc() override;
};
ENGINE_NAMESPACE_END
