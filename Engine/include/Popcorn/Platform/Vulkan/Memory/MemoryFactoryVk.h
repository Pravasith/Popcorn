#pragma once

#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "MaterialTypes.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "RenderFlows/RenderFlowVk.h"
#include <cstring>
#include <unordered_map>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

struct SubmeshOffsets {
  VkDeviceSize vboOffset = 0;
  VkDeviceSize iboOffset = 0;

  SubmeshOffsets &operator+=(SubmeshOffsets &other) {
    this->vboOffset += other.vboOffset;
    this->iboOffset += other.iboOffset;
    return *this;
  };
};

enum UboViews {
  WorldMatrix = 1,
  BasicMatValues,
  PbrMatValues,
  LightsValues,
  CameraMatrix
};

struct UboBufferView {
  VkDeviceSize offset = 0;
  VkDeviceSize alignedSize = 0;
};

class MemoryFactoryVk {
public:
  //
  // --- VBOs & IBOs -----------------------------------------------------------
  void AllocVboIboStagingBuffers(VkDeviceSize vboSize, VkDeviceSize iboSize);
  template <MaterialTypes T>
  void
  CopySubmeshGroupToVboIboStagingBuffers(SubmeshOffsets &accSubmeshGroupOffsets,
                                         const SubmeshGroups<T> &submeshGroups);
  void CleanUpVboIboStagingBuffers();

  void FlushVBOsAndIBOsStagingToLocal(VkDeviceSize submeshVbosSize,
                                      VkDeviceSize submeshIbosSize);

  void AllocVboIboLocalBuffers(VkDeviceSize vboSize, VkDeviceSize iboSize);
  void CleanUpVboIboLocalBuffers();

  //
  // --- UBOs ------------------------------------------------------------------
  void AllocUboLocalBuffers();

  //
  // --- UTILS -----------------------------------------------------------------
  template <MaterialTypes T>
  void GetVboIboUboSizes(SubmeshGroups<T> &submeshGroups, VkDeviceSize &vboSize,
                         VkDeviceSize &iboSize, VkDeviceSize &uboSize) {
    VkDeviceSize vbo, ibo, ubo;

    // auto *device = ContextVk::Device();
    // VkPhysicalDeviceProperties properties{};
    // device->GetPhysicalDeviceProperties(properties);
    // VkDeviceSize alignment =
    // properties.limits.optimalBufferCopyOffsetAlignment;

    for (auto &[matId, submeshes] : submeshGroups) {
      for (Submesh<T> *submesh : submeshes) {
        const BufferDefs::Layout &vboLayout =
            submesh->GetVertexBuffer()->GetLayout();
        vbo = vboLayout.countValue * vboLayout.strideValue;

        IndexBuffer<uint32_t> *indexBuffer = submesh->GetIndexBuffer();
        ibo = indexBuffer->GetCount() * sizeof(uint32_t);

        vboSize += vbo;
        iboSize += ibo;

        // TODO: Calculate:
        // - Vbo, ibo total sizes -- DONE
        // - Calculate ubo final size as well (incl. lights UBOs)
      }
    };
  };

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

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  MemoryFactoryVk(const MemoryFactoryVk &) = delete;
  MemoryFactoryVk &operator=(const MemoryFactoryVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  MemoryFactoryVk(MemoryFactoryVk &&) = delete;
  MemoryFactoryVk &operator=(MemoryFactoryVk &&) = delete;

  void CleanUp() {
    // Nothing so far
  };

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

  // One big VBO
  // One big IBO
  // One big UBO

  //
  // Host-visible & temporary -----------------------------------------------
  VkBuffer m_vboStaging;
  VmaAllocation m_vboStagingAlloc;
  void *m_vboStagingMapping; // temp variable

  VkBuffer m_iboStaging;
  VmaAllocation m_iboStagingAlloc;
  void *m_iboStagingMapping; // temp variable

  //
  // Device-local -----------------------------------------------------------
  VkBuffer m_vbo;
  VmaAllocation m_vboAlloc;

  VkBuffer m_ibo;
  VmaAllocation m_iboAlloc;

  //
  // Host-visible -----------------------------------------------------------
  VkBuffer m_ubo;
  VmaAllocation m_uboAlloc;

  using VboIboOffsets =
      std::unordered_map<MaterialHashType,
                         std::vector<std::pair<VkDeviceSize, VkDeviceSize>>>;

  VboIboOffsets
      m_vboIboOffsets{}; // Accumulated offsets for BOTH basic & pbr
                         // material types. Don't worry about the MatType
                         // sequence mismatch because matId (the key) is
                         // uniquely hashed based on material properties.

  using UboBufferViewMap = std::unordered_map<UboViews, UboBufferView>;

  UboBufferViewMap m_uboBufferViewMap{
      {UboViews::WorldMatrix, {}},  {UboViews::BasicMatValues, {}},
      {UboViews::PbrMatValues, {}}, {UboViews::LightsValues, {}},
      {UboViews::WorldMatrix, {}},
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
