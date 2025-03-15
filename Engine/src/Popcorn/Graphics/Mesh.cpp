#include "Mesh.h"
#include "DeviceVk.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

BufferDefs::Layout Mesh::s_uniformBufferLayout = BufferDefs::Layout();
uint32_t Mesh::s_uniformBufferLayoutAlignedStride = 0;

void Mesh::ValidateMembersWithSpec(const Spec &spec) {
#ifdef PC_DEBUG
  if (spec.enableIndexBuffers && m_indexBuffer == nullptr) {
    PC_ERROR("Index buffer is enabled but it's empty", "Mesh")
  };
#endif
};

void Mesh::SetAlignedUniformBufferLayoutStride() {
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(DeviceVk::Get()->GetPhysicalDevice(),
                                &deviceProperties);
  s_uniformBufferLayoutAlignedStride =
      (s_uniformBufferLayout.strideValue +
       deviceProperties.limits.minUniformBufferOffsetAlignment - 1) &
      ~(deviceProperties.limits.minUniformBufferOffsetAlignment - 1);
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
