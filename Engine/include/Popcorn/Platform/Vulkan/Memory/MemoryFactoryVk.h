
#pragma once

#include "BufferObjects.h"
#include "BufferObjectsVk.h"
#include "ContextVk.h"
#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "MaterialTypes.h"
#include "Mesh.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

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

  void AllocateVkMemory(VkDeviceSize vboSize) {
    VkBuffer vboStagingBuffer{};
    VmaAllocation vboStagingAlloc{};

    VkBufferCreateInfo vboStagingInfo{};
    BufferVkUtils::GetDefaultVkBufferState(vboStagingInfo, vboSize);
    vboStagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VmaAllocationCreateInfo vboStagingVmaInfo{};
    vboStagingVmaInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    vmaCreateBuffer(ContextVk::MemoryAllocator()->GetVMAAllocator(),
                    &vboStagingInfo, &vboStagingVmaInfo, &vboStagingBuffer,
                    &vboStagingAlloc, nullptr);

    // DON'T PREMATURELY ABSTRACT CODE!!!!
    // DO EVERYTHING IN THIS FUNCTION AND ONCE DONE, ABSTRACT.
  };

  template <MaterialTypes T>
  void CreateSubmeshVBOs(
      const std::unordered_map<MaterialHashType, std::vector<Submesh<T> *>>
          &submeshGroups) {

    // basicMat1 : [sm1, sm2, sm3, ... ]
    // basicMat2 : [sm1, sm2 ... ]

    // pbrMat1 : [sm1, sm2, sm3, ... ]
    // pbrMat2 : [sm1, sm2 ... ]

    for (auto &[matId, submeshes] : submeshGroups) {
      VkDeviceSize offset = 0;
      for (auto &submesh : submeshes) {
        // CreateVmaBuffer( offset = matId + submeshIdx );
        m_vboIboOffsets[matId].emplace_back(std::pair(offset, // VBO
                                                      0 // IBO (adds later)
                                                      ));
        // offset += (i * vtxStride);
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
  static MemoryFactoryVk *s_instance;
  static DeviceVk *s_deviceVk;

  // Device-Local VkBufferMemory
  VkBuffer m_submeshVBOs;
  VkBuffer m_submeshIBOs;

  VmaAllocationInfo m_submeshVBOsVMAInfo;
  VmaAllocationInfo m_submeshIBOsVMAInfo;

  VkBuffer m_submeshVBOsStaging;
  VkBuffer m_submeshIBOsStaging;

  VmaAllocationInfo m_submeshVBOsStagingVMAInfo;
  VmaAllocationInfo m_submeshIBOsStagingVMAInfo;

  // Host-visible VkBufferMemory
  VkBuffer m_basicMaterialUBOs;
  VkBuffer m_pbrMaterialUBOs;
  VkBuffer m_modelMatrixUBOs;

  VkBuffer m_viewProjMatrixUBO; // Small size -- can be a push constant

  using VboIboOffsets =
      std::unordered_map<MaterialHashType,
                         std::vector<std::pair<VkDeviceSize, VkDeviceSize>>>;

  VboIboOffsets m_vboIboOffsets{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
