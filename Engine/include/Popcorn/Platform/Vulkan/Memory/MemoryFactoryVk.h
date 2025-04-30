#pragma once

#include "BufferObjects.h"
#include "BufferObjectsVk.h"
#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "MaterialTypes.h"
#include "Mesh.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

struct PcSubmeshGroupOffsets {
  VkDeviceSize submeshGroupVboSize = 0;
  VkDeviceSize submeshGroupIboSize = 0;

  PcSubmeshGroupOffsets &operator+=(PcSubmeshGroupOffsets &other) {
    this->submeshGroupVboSize += other.submeshGroupVboSize;
    this->submeshGroupIboSize += other.submeshGroupIboSize;
    return *this;
  };
};

class MemoryFactoryVk {
public:
  [[nodiscard]] inline static MemoryFactoryVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new MemoryFactoryVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of MemoryFactoryVk")
    };
  };

  void CreateAndAllocStagingBuffers(VkDeviceSize vboSize, VkDeviceSize iboSize);
  void CleanUpStagingBuffers();

  void FlushBuffersStagingToLocal(VkDeviceSize submeshVbosSize,
                                  VkDeviceSize submeshIbosSize);

  void CreateAndAllocLocalBuffers(VkDeviceSize vboSize, VkDeviceSize iboSize);
  void CleanUpLocalBuffers();

  // -> current material group offset in bytes
  template <MaterialTypes T>
  void CopySubmeshGroupToStagingBuffers(
      PcSubmeshGroupOffsets &prevSubmeshGroupOffsets,
      const std::unordered_map<MaterialHashType, std::vector<Submesh<T> *>>
          &submeshGroups) {

    VkDeviceSize &vboOffset = prevSubmeshGroupOffsets.submeshGroupVboSize;
    VkDeviceSize &iboOffset = prevSubmeshGroupOffsets.submeshGroupIboSize;

    for (auto &[matId, submeshes] : submeshGroups) {
      for (Submesh<T> *submesh : submeshes) {
        // VBOs ---------------------------------------------------------------
        BufferDefs::Layout &vboLayout = submesh->GetVertexBuffer()->GetLayout();
        const VkDeviceSize vboSize =
            vboLayout.countValue * vboLayout.strideValue;

        memcpy((byte_t *)m_submeshVboMapping + vboOffset,
               submesh->GetVertexBuffer()->GetBufferData(), (size_t)vboSize);

        // IBOs ---------------------------------------------------------------
        IndexBuffer<uint32_t> indexBuffer = submesh->GetIndexBuffer();
        const VkDeviceSize iboSize = indexBuffer.GetCount() * sizeof(uint32_t);

        memcpy((byte_t *)m_submeshIboMapping + iboOffset,
               indexBuffer.GetBufferData(), (size_t)iboSize);

        m_vboIboOffsets[matId].emplace_back(std::pair(vboOffset, iboOffset));

        vboOffset += vboSize;
        iboOffset += iboSize;
      }
    };
  };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  MemoryFactoryVk(const MemoryFactoryVk &) = delete;
  MemoryFactoryVk &operator=(const MemoryFactoryVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  MemoryFactoryVk(MemoryFactoryVk &&) = delete;
  MemoryFactoryVk &operator=(MemoryFactoryVk &&) = delete;

  void CleanUp();

private:
  MemoryFactoryVk() {
    s_deviceVk = DeviceVk::Get();
    PC_PRINT("CREATED", TagType::Constr, "MemoryFactoryVk")
  };
  ~MemoryFactoryVk() {
    s_deviceVk = nullptr;
    PC_PRINT("DESTROYED", TagType::Destr, "MemoryFactoryVk")
  };

private:
  using VboIboOffsets =
      std::unordered_map<MaterialHashType,
                         std::vector<std::pair<VkDeviceSize, VkDeviceSize>>>;

  static MemoryFactoryVk *s_instance;
  static DeviceVk *s_deviceVk;

  //
  // Host-visible & temporary -----------------------------------------------
  VkBuffer m_submeshVBOsStaging;
  VmaAllocation m_submeshVBOsAllocStaging;
  void *m_submeshVboMapping; // temp variable

  VkBuffer m_submeshIBOsStaging;
  VmaAllocation m_submeshIBOsAllocStaging;
  void *m_submeshIboMapping; // temp variable

  //
  // Device-local -----------------------------------------------------------
  VkBuffer m_submeshVBOs;
  VmaAllocation m_submeshVBOsAlloc;

  VkBuffer m_submeshIBOs;
  VmaAllocation m_submeshIBOsAlloc;

  //
  // Host-visible -----------------------------------------------------------
  // VkBuffer m_basicMaterialUBOs; // And an equivalent VMA allocation
  // VkBuffer m_pbrMaterialUBOs;   // And an equivalent VMA allocation
  // VkBuffer m_modelMatrixUBOs;   // And an equivalent VMA allocation

  VboIboOffsets m_vboIboOffsets{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
