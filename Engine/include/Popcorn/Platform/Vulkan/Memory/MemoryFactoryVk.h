#pragma once

#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "MaterialTypes.h"
#include "Mesh.h"
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

struct PcSubmeshGroupOffsets {
  VkDeviceSize submeshGroupVboSize = 0;
  VkDeviceSize submeshGroupIboSize = 0;

  PcSubmeshGroupOffsets &operator+=(PcSubmeshGroupOffsets &other) {
    this->submeshGroupVboSize += other.submeshGroupVboSize;
    this->submeshGroupIboSize += other.submeshGroupIboSize;
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

  void FlushVBOsAndIBOsStagingToLocal(VkDeviceSize submeshVbosSize,
                                      VkDeviceSize submeshIbosSize);

  void CreateAndAllocLocalBuffers(VkDeviceSize vboSize, VkDeviceSize iboSize);
  void CleanUpLocalBuffers();

  // -> current material group offset in bytes
  template <MaterialTypes T>
  void CopySubmeshGroupToStagingBuffers(
      PcSubmeshGroupOffsets &accSubmeshGroupOffsets,
      const std::unordered_map<MaterialHashType, std::vector<Submesh<T> *>>
          &submeshGroups);

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
  VkBuffer m_gBufferPassUBOs;
  VmaAllocation m_gBufferPassUBOsAlloc;

  VkBuffer m_lightPassUBOs;
  VmaAllocation m_lightPassUBOsAlloc;

  VkBuffer m_compositePassUBOs;
  VmaAllocation m_compositePassUBOsAlloc;

  using VboIboOffsets =
      std::unordered_map<MaterialHashType,
                         std::vector<std::pair<VkDeviceSize, VkDeviceSize>>>;

  VboIboOffsets m_vboIboOffsets{}; // Accumulated offsets for BOTH basic & pbr
                                   // material types

  using UboBufferViewMap = std::unordered_map<UboViews, UboBufferView>;

  UboBufferViewMap m_uboBufferViewMap{
      {UboViews::WorldMatrix, {}},  {UboViews::BasicMatValues, {}},
      {UboViews::PbrMatValues, {}}, {UboViews::LightsValues, {}},
      {UboViews::WorldMatrix, {}},
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
