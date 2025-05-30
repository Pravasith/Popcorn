#include "Memory/MemoryVk.h"
#include "BufferObjectsVk.h"
#include "Camera.h"
#include "CommonVk.h"
#include "ContextVk.h"
#include "Empty.h"
#include "GlobalMacros.h"
#include "Light.h"
#include "Material.h"
#include "MaterialTypes.h"
#include "Memory/HelpersVk.h"
#include "Memory/MemoryDefsVk.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "Uniforms.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <glm/fwd.hpp>
#include <glm/matrix.hpp>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <MaterialTypes T>
void MemoryVk::ExtractOffsetsMaterialsSubmeshes(
    PcMaterialSubmeshesMap<T> &materialSubmeshesMap) {

  auto *device = ContextVk::Device();
  VkPhysicalDeviceProperties properties{};
  device->GetPhysicalDeviceProperties(properties);
  VkDeviceSize alignment = properties.limits.minUniformBufferOffsetAlignment;

  // temps
  VkDeviceSize vboOffset = 0, iboOffset = 0, submeshUboOffset = 0;
  VkDeviceSize basicMatOffset = 0, pbrMatOffset = 0;

  for (auto &[matId, submeshes] : materialSubmeshesMap) {
    //
    // Extract Material Offsets ------------------------------------------------
    if constexpr (T == MaterialTypes::BasicMat) {
      m_bufferOffsets.materialOffsets[matId] = basicMatOffset;
      basicMatOffset +=
          PC_AlignCeil(UniformDefs::BasicMaterialUniform::size, alignment);
    } else if constexpr (T == MaterialTypes::PbrMat) {
      m_bufferOffsets.materialOffsets[matId] = pbrMatOffset;
      pbrMatOffset +=
          PC_AlignCeil(UniformDefs::PbrMaterialUniform::size, alignment);
    }

    //
    // Extract VBO & IBO & UBO offsets -----------------------------------------
    for (Submesh<T> *submesh : submeshes) {
      m_bufferOffsets.submeshesOffsets[matId].push_back(
          {vboOffset, iboOffset, submeshUboOffset});

      VertexBuffer *vertexBuffer = submesh->GetVertexBuffer();
      vboOffset +=
          vertexBuffer->GetCount() * vertexBuffer->GetLayout().strideValue;

      IndexBuffer<uint32_t> *indexBuffer = submesh->GetIndexBuffer();
      iboOffset += indexBuffer->GetCount() * sizeof(uint32_t);

      submeshUboOffset +=
          PC_AlignCeil(UniformDefs::SubmeshUniform::size, alignment);
    }
  }

  if constexpr (T == MaterialTypes::BasicMat) {
    m_bufferViews.basicMatUbo = {0, basicMatOffset};
  } else if constexpr (T == MaterialTypes::PbrMat) {
    m_bufferViews.pbrMatUbo = {0, pbrMatOffset};
  }

  m_bufferViews.submeshVbo += {0, vboOffset};
  m_bufferViews.submeshIbo += {0, iboOffset};
  m_bufferViews.submeshUbo += {0, submeshUboOffset};
}

void MemoryVk::ExtractOffsetsLightsCamerasEmptys(std::vector<Light *> &lights,
                                                 std::vector<Camera *> &cameras,
                                                 std::vector<Empty *> &emptys) {
  auto *device = ContextVk::Device();
  VkPhysicalDeviceProperties properties{};
  device->GetPhysicalDeviceProperties(properties);
  auto &limits = properties.limits;

  VkDeviceSize lightOffsets = 0, cameraOffsets = 0, emptyOffsets = 0;

  for (Light *light : lights) {
    m_bufferOffsets.lightsOffsets.push_back(lightOffsets);
    lightOffsets += PC_AlignCeil(UniformDefs::LightUniform::size,
                                 limits.minStorageBufferOffsetAlignment);
  };

  for (Camera *camera : cameras) {
    m_bufferOffsets.camerasOffsets.push_back(cameraOffsets);
    cameraOffsets += PC_AlignCeil(UniformDefs::CameraUniform::size,
                                  limits.minUniformBufferOffsetAlignment);
  };

  for (Empty *empty : emptys) {
    m_bufferOffsets.emptysOffsets.push_back(emptyOffsets);
    emptyOffsets += PC_AlignCeil(UniformDefs::EmptyUniform::size,
                                 limits.minUniformBufferOffsetAlignment);
  };

  m_bufferViews.lightsSsbo = {0, lightOffsets};
  m_bufferViews.camerasUbo = {0, cameraOffsets};
  m_bufferViews.emptysUbo = {0, emptyOffsets};
};

void MemoryVk::AlignVboIboBufferViews() {
  VkPhysicalDeviceProperties properties{};
  ContextVk::Device()->GetPhysicalDeviceProperties(properties);

  m_bufferViews.submeshVbo.alignedSize =
      PC_AlignCeil(m_bufferViews.submeshVbo.alignedSize,
                   properties.limits.optimalBufferCopyOffsetAlignment);
  m_bufferViews.submeshIbo.alignedSize =
      PC_AlignCeil(m_bufferViews.submeshIbo.alignedSize,
                   properties.limits.optimalBufferCopyOffsetAlignment);
};

void MemoryVk::CalculateUboSsboBaseOffsets() {
  auto *device = ContextVk::Device();
  VkPhysicalDeviceProperties properties{};
  device->GetPhysicalDeviceProperties(properties);
  auto &limits = properties.limits;

  VkDeviceSize bufferViewOffset = 0;

  m_bufferViews.submeshUbo.offset = bufferViewOffset;
  VkDeviceSize submeshUboSize =
      PC_AlignCeil(m_bufferViews.submeshUbo.alignedSize,
                   limits.minUniformBufferOffsetAlignment);
  bufferViewOffset += submeshUboSize;
  m_bufferViews.submeshUbo.alignedSize = submeshUboSize;

  m_bufferViews.basicMatUbo.offset = bufferViewOffset;
  VkDeviceSize basicMatUboSize =
      PC_AlignCeil(m_bufferViews.basicMatUbo.alignedSize,
                   limits.minUniformBufferOffsetAlignment);
  bufferViewOffset += basicMatUboSize;
  m_bufferViews.basicMatUbo.alignedSize = basicMatUboSize;

  m_bufferViews.pbrMatUbo.offset = bufferViewOffset;
  VkDeviceSize pbrMatUboSize =
      PC_AlignCeil(m_bufferViews.pbrMatUbo.alignedSize,
                   limits.minUniformBufferOffsetAlignment);
  bufferViewOffset += pbrMatUboSize;
  m_bufferViews.pbrMatUbo.alignedSize = pbrMatUboSize;

  m_bufferViews.camerasUbo.offset = bufferViewOffset;
  VkDeviceSize camerasUboSize =
      PC_AlignCeil(m_bufferViews.camerasUbo.alignedSize,
                   limits.minUniformBufferOffsetAlignment);
  bufferViewOffset += camerasUboSize;
  m_bufferViews.camerasUbo.alignedSize = camerasUboSize;

  m_bufferViews.emptysUbo.offset = bufferViewOffset;
  VkDeviceSize emptysUboSize =
      PC_AlignCeil(m_bufferViews.emptysUbo.alignedSize,
                   limits.minUniformBufferOffsetAlignment);
  bufferViewOffset += emptysUboSize;
  m_bufferViews.emptysUbo.alignedSize = emptysUboSize;

  // clang-format off
  PC_PRINT(
          "\nSubmesh UBO: \n" <<
          m_bufferViews.submeshUbo.offset << " " <<
          m_bufferViews.submeshUbo.alignedSize << " "
          "\nBasicMat UBO: \n" << " " <<
          m_bufferViews.basicMatUbo.offset << " " <<
          m_bufferViews.basicMatUbo.alignedSize <<
          "\nPbrMat UBO: \n" <<
          m_bufferViews.pbrMatUbo.offset << " " <<
          m_bufferViews.pbrMatUbo.alignedSize<< " " <<
          "\nCamerasMat UBO: \n" <<
          m_bufferViews.camerasUbo.offset<< " " <<
          m_bufferViews.camerasUbo.alignedSize << " " <<
          "\nEmptys UBO: \n" <<
          m_bufferViews.emptysUbo.offset << " " <<
          m_bufferViews.emptysUbo.alignedSize  << " " <<
          "\nLights SSBO: \n" <<
          m_bufferViews.lightsSsbo.offset << " " <<
          m_bufferViews.lightsSsbo.alignedSize << " " <<
          "\n"
          , TagType::Print, "MemoryVk")
  // clang-format on
};

template <MaterialTypes T>
void MemoryVk::FillVbosIbosUbosSubmeshMaterial(
    PcMaterialSubmeshesMap<T> &materialSubmeshesMap,
    PcMaterialMap<T> &materialMap) {
  for (auto &[matId, submeshes] : materialSubmeshesMap) {
    //
    // TODO: 2. Optimise the extra buffer:
    // Instead of having 2 buffers (one per maxFIF), have one buffer with all
    // the submeshes material data - only for submesh materials
    //

    if constexpr (T == MaterialTypes::BasicMat) {
      Material<MaterialTypes::BasicMat> *material = materialMap[matId];
      PcCopyUniformToMemory<UniformDefs::Uniforms::BasicMat>
          copyUniformToMemory{material->GetMaterialData(), m_bufferViews,
                              m_bufferOffsets};
      for (uint16_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        copyUniformToMemory(m_uboMappingSet[i], matId);
      }
    } else if constexpr (T == MaterialTypes::PbrMat) {
      Material<MaterialTypes::PbrMat> *material = materialMap[matId];
      PcCopyUniformToMemory<UniformDefs::Uniforms::PbrMat> copyUniformToMemory{
          material->GetMaterialData(), m_bufferViews, m_bufferOffsets};
      for (uint16_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        copyUniformToMemory(m_uboMappingSet[i], matId);
      }
    } else {
      PC_ERROR("Wrong material type", "MemoryVk")
    };

    for (int i = 0; i < submeshes.size(); ++i) {
      Submesh<T> *submesh = submeshes[i];
      //
      // VBOs ---------------------------------------------------------------
      const BufferDefs::Layout &vboLayout =
          submesh->GetVertexBuffer()->GetLayout();
      const VkDeviceSize vboSize = vboLayout.countValue * vboLayout.strideValue;
      memcpy((byte_t *)m_vboStagingMapping +
                 m_bufferOffsets.submeshesOffsets[matId][i].vboOffset,
             submesh->GetVertexBuffer()->GetBufferData(), (size_t)vboSize);

      //
      // IBOs ---------------------------------------------------------------
      IndexBuffer<uint32_t> *indexBuffer = submesh->GetIndexBuffer();
      const VkDeviceSize iboSize = indexBuffer->GetCount() * sizeof(uint32_t);
      memcpy((byte_t *)m_iboStagingMapping +
                 m_bufferOffsets.submeshesOffsets[matId][i].iboOffset,
             indexBuffer->GetBufferData(), (size_t)iboSize);
    }
  };
};

template <MaterialTypes T>
void MemoryVk::FillUbosSubmesh(PcMaterialSubmeshesMap<T> &materialSubmeshesMap,
                               const uint32_t currentFrame) {
  for (auto &[matId, submeshes] : materialSubmeshesMap) {
    for (size_t i = 0; i < submeshes.size(); ++i) {
      Submesh<T> *submesh = submeshes[i];
      glm::mat4 &worldMatrix = submesh->GetParentMesh()->GetWorldMatrix();
      // Normal matrix is for maintaining the property of normal staying
      // perpendicular to the tangents of the vertex.
      glm::mat4 normalMatrix = glm::transpose(glm::inverse(worldMatrix));

      PcCopyUniformToMemory<Uniforms::Submesh> copyUniformToMemory{
          worldMatrix, normalMatrix, m_bufferViews, m_bufferOffsets};
      copyUniformToMemory(m_uboMappingSet[currentFrame], matId, i);
    }
  };
};

void MemoryVk::FillUbosSsbosLightCameraEmpty(std::vector<Light *> &lights,
                                             std::vector<Camera *> &cameras,
                                             std::vector<Empty *> &emptys,
                                             const uint32_t currentFrame) {
  size_t i = 0;
  for (Light *light : lights) {
    PcCopyUniformToMemory<Uniforms::Light> copyUniformToMemory{
        light, m_bufferViews, m_bufferOffsets};
    copyUniformToMemory(m_ssboMappingSet[currentFrame], i);
    ++i;
  }

  i = 0;
  for (Camera *camera : cameras) {
    PcCopyUniformToMemory<Uniforms::Camera> copyUniformToMemory{
        camera, m_bufferViews, m_bufferOffsets};
    copyUniformToMemory(m_uboMappingSet[currentFrame], i);
    ++i;
  }

  i = 0;
  for (Empty *empty : emptys) {
    PcCopyUniformToMemory<Uniforms::Empty> copyUniformToMemory{
        empty, m_bufferViews, m_bufferOffsets};
    copyUniformToMemory(m_uboMappingSet[currentFrame], i);
    ++i;
  }
};

void MemoryVk::AllocSubmeshVboIboStaging() {
  //
  // --- VERTEX BUFFERS CREATION -------------------------------------------
  VkBufferCreateInfo vboStagingInfo{};
  BufferVkUtils::GetDefaultVkBufferState(vboStagingInfo,
                                         m_bufferViews.submeshVbo.alignedSize);
  vboStagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo vboStagingVmaInfo{};
  vboStagingVmaInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  VkResult vboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &vboStagingInfo,
      &vboStagingVmaInfo, &m_vboStaging, &m_vboStagingAlloc, nullptr);
  if (vboResult != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create VBO staging buffer");
  };

  //
  // -----------------------------------------------------------------------
  // --- VERTEX BUFFERS MAP & COPY -----------------------------------------
  if (vmaMapMemory(ContextVk::MemoryAllocator()->GetVMAAllocator(),
                   m_vboStagingAlloc, &m_vboStagingMapping) != VK_SUCCESS) {
    throw std::runtime_error("Couldn't 'map' VBO staging buffer");
  };

  //
  //
  //
  //
  // -----------------------------------------------------------------------
  // --- INDEX BUFFERS CREATION --------------------------------------------
  VkBufferCreateInfo iboStagingInfo{};
  BufferVkUtils::GetDefaultVkBufferState(iboStagingInfo,
                                         m_bufferViews.submeshIbo.alignedSize);
  iboStagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo iboStagingVmaInfo{};
  iboStagingVmaInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  VkResult iboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &iboStagingInfo,
      &iboStagingVmaInfo, &m_iboStaging, &m_iboStagingAlloc, nullptr);
  if (iboResult != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create IBO staging buffer");
  };

  //
  // -----------------------------------------------------------------------
  // --- INDEX BUFFERS MAP & COPY ------------------------------------------
  if (vmaMapMemory(ContextVk::MemoryAllocator()->GetVMAAllocator(),
                   m_iboStagingAlloc, &m_iboStagingMapping) != VK_SUCCESS) {
    throw std::runtime_error("Couldn't map IBO staging buffer");
  };
}

void MemoryVk::FlushVboIboStagingToLocal() {
  BufferVkUtils::CopyStagingToMainBuffers(m_vboStaging, m_vbo,
                                          m_bufferViews.submeshVbo.alignedSize);
  BufferVkUtils::CopyStagingToMainBuffers(m_iboStaging, m_ibo,
                                          m_bufferViews.submeshIbo.alignedSize);
};

void MemoryVk::AllocSubmeshVboIboLocal() {
  //
  // --- VERTEX BUFFERS CREATION -------------------------------------------
  VkBufferCreateInfo vboMainInfo{};
  BufferVkUtils::GetDefaultVkBufferState(vboMainInfo,
                                         m_bufferViews.submeshVbo.alignedSize);
  vboMainInfo.usage =
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo vboMainVmaInfo{};
  vboMainVmaInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VkResult vboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &vboMainInfo,
      &vboMainVmaInfo, &m_vbo, &m_vboAlloc, nullptr);
  if (vboResult != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create VBO device buffer");
  };

  //
  // --- INDEX BUFFERS CREATION --------------------------------------------
  VkBufferCreateInfo iboMainInfo{};
  BufferVkUtils::GetDefaultVkBufferState(iboMainInfo,
                                         m_bufferViews.submeshIbo.alignedSize);
  iboMainInfo.usage =
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo iboMainVmaInfo{};
  iboMainVmaInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VkResult iboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &iboMainInfo,
      &iboMainVmaInfo, &m_ibo, &m_iboAlloc, nullptr);
  if (iboResult != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create IBO device buffer");
  };
}

void MemoryVk::AllocUboSsboLocalBuffers() {
  VkDeviceSize uboTotalSize = m_bufferViews.submeshUbo.alignedSize +
                              m_bufferViews.basicMatUbo.alignedSize +
                              m_bufferViews.pbrMatUbo.alignedSize +
                              m_bufferViews.camerasUbo.alignedSize +
                              m_bufferViews.emptysUbo.alignedSize;
  VkDeviceSize ssboTotalSize = m_bufferViews.lightsSsbo.alignedSize;

  VkBufferCreateInfo uboCreateInfo{};
  uboCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  uboCreateInfo.size = uboTotalSize;
  uboCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  uboCreateInfo.sharingMode =
      VK_SHARING_MODE_EXCLUSIVE; // not sharing other queues

  VkBufferCreateInfo ssboCreateInfo{};
  ssboCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  ssboCreateInfo.size = ssboTotalSize;
  ssboCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  ssboCreateInfo.sharingMode =
      VK_SHARING_MODE_EXCLUSIVE; // not sharing other queues

  VmaAllocationCreateInfo uboAllocCreateInfo{};
  uboAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
  uboAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

  VmaAllocationCreateInfo ssboAllocCreateInfo{};
  ssboAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
  ssboAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    VmaAllocationInfo uboVmaInfo{};
    if (vmaCreateBuffer(ContextVk::MemoryAllocator()->GetVMAAllocator(),
                        &uboCreateInfo, &uboAllocCreateInfo, &m_uboSet[i],
                        &m_uboAllocSet[i], &uboVmaInfo) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create uniform buffer");
    };

    m_uboMappingSet[i] = uboVmaInfo.pMappedData;

    VmaAllocationInfo ssboVmaInfo{};
    if (vmaCreateBuffer(ContextVk::MemoryAllocator()->GetVMAAllocator(),
                        &ssboCreateInfo, &ssboAllocCreateInfo, &m_ssboSet[i],
                        &m_ssboAllocSet[i], &ssboVmaInfo) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create uniform buffer");
    };

    m_ssboMappingSet[i] = ssboVmaInfo.pMappedData;
  };
};

// Clean up functions --------------------------------------------------------
void MemoryVk::CleanUpSubmeshVboIboBuffersStaging() {
  auto &allocator = ContextVk::MemoryAllocator()->GetVMAAllocator();

  if (m_vboStagingMapping) {
    vmaUnmapMemory(allocator, m_vboStagingAlloc);
    m_vboStagingMapping = nullptr;
  }

  if (m_iboStagingMapping) {
    vmaUnmapMemory(allocator, m_iboStagingAlloc);
    m_iboStagingMapping = nullptr;
  }

  if (m_vboStaging != VK_NULL_HANDLE && m_vboStagingAlloc) {
    vmaDestroyBuffer(allocator, m_vboStaging, m_vboStagingAlloc);
    m_vboStaging = VK_NULL_HANDLE;
    m_vboStagingAlloc = nullptr;
  }

  if (m_iboStaging != VK_NULL_HANDLE && m_iboStagingAlloc) {
    vmaDestroyBuffer(allocator, m_iboStaging, m_iboStagingAlloc);
    m_iboStaging = VK_NULL_HANDLE;
    m_iboStagingAlloc = nullptr;
  }
};

void MemoryVk::CleanUpVboIboLocalBuffers() {
  auto &allocator = ContextVk::MemoryAllocator()->GetVMAAllocator();

  if (m_vbo != VK_NULL_HANDLE && m_vboAlloc) {
    vmaDestroyBuffer(allocator, m_vbo, m_vboAlloc);
    m_vbo = VK_NULL_HANDLE;
    m_vboAlloc = nullptr;
  }

  if (m_ibo != VK_NULL_HANDLE && m_iboAlloc) {
    vmaDestroyBuffer(allocator, m_ibo, m_iboAlloc);
    m_ibo = VK_NULL_HANDLE;
    m_iboAlloc = nullptr;
  }
};

void MemoryVk::CleanUpUboSsboLocalBuffers() {
  auto &allocator = ContextVk::MemoryAllocator()->GetVMAAllocator();

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    if (m_uboSet[i] != VK_NULL_HANDLE && m_uboAllocSet[i]) {
      vmaDestroyBuffer(allocator, m_uboSet[i], m_uboAllocSet[i]);
      m_uboSet[i] = VK_NULL_HANDLE;
      m_uboAllocSet[i] = nullptr;
      m_uboMappingSet[i] = nullptr;
    }

    if (m_ssboSet[i] != VK_NULL_HANDLE && m_ssboAllocSet[i]) {
      vmaDestroyBuffer(allocator, m_ssboSet[i], m_ssboAllocSet[i]);
      m_ssboSet[i] = VK_NULL_HANDLE;
      m_ssboAllocSet[i] = nullptr;
      m_ssboMappingSet[i] = nullptr;
    }
  };
};

// Linker stuff
template void
MemoryVk::ExtractOffsetsMaterialsSubmeshes<MaterialTypes::BasicMat>(
    PcMaterialSubmeshesMap<MaterialTypes::BasicMat> &);
template void MemoryVk::ExtractOffsetsMaterialsSubmeshes<MaterialTypes::PbrMat>(
    PcMaterialSubmeshesMap<MaterialTypes::PbrMat> &);

template void
MemoryVk::FillVbosIbosUbosSubmeshMaterial<MaterialTypes::BasicMat>(
    PcMaterialSubmeshesMap<MaterialTypes::BasicMat> &,
    PcMaterialMap<MaterialTypes::BasicMat> &);
template void MemoryVk::FillVbosIbosUbosSubmeshMaterial<MaterialTypes::PbrMat>(
    PcMaterialSubmeshesMap<MaterialTypes::PbrMat> &,
    PcMaterialMap<MaterialTypes::PbrMat> &);

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
