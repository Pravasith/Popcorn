#include "VertexBuffer.h"
#include "GlobalMacros.h"
#include <initializer_list>

ENGINE_NAMESPACE_BEGIN
namespace Gfx {
template <typename T>
VertexBuffer<T>::VertexBuffer(std::initializer_list<T> buffer)
    : m_buffer(buffer){
          PC_PRINT("CREATED", TagType::Constr, "VERTEX-BUFFER")
          //
          // Renderer
      };

template <typename T>
VertexBuffer<T>::~VertexBuffer(){
    PC_PRINT("DESTROYED", TagType::Destr, "VERTEX-BUFFER")};
}; // namespace Gfx
ENGINE_NAMESPACE_END
