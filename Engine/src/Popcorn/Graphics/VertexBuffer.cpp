#include "VertexBuffer.h"
#include "GlobalMacros.h"
#include <initializer_list>

ENGINE_NAMESPACE_BEGIN
template <typename T>
Gfx::VertexBuffer<T>::VertexBuffer(std::initializer_list<T> buffer)
    : m_buffer(buffer){PC_PRINT("CREATED", TagType::Constr, "VERTEX-BUFFER")};
ENGINE_NAMESPACE_END
