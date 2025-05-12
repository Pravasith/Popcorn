#pragma once

#include "Camera.h"
#include "CommonVk.h"
#include "DeviceVk.h"
#include "Empty.h"
#include "GlobalMacros.h"
#include "Light.h"
#include "MaterialTypes.h"
#include "Memory/MemoryDefsVk.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <glm/fwd.hpp>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class MemoryVk {
public:
  [[nodiscard]] const VkBuffer &GetUboSet(uint32_t frameIndex) const {
    return m_uboSet[frameIndex];
  };
  [[nodiscard]] const PcBufferViews &GetBufferViews() const {
    return m_bufferViews;
  };
  [[nodiscard]] const PcBufferOffsets &GetBufferOffsets() const {
    return m_bufferOffsets;
  };

  template <MaterialTypes T>
  void ExtractOffsetsMaterialsSubmeshes(
      PcMaterialSubmeshesMap<T> &materialSubmeshesMap);

  void ExtractOffsetsLightsCamerasEmptys(std::vector<Light *> &lights,
                                         std::vector<Camera *> &cameras,
                                         std::vector<Empty *> &emptys);
  void AlignVboIboBufferViews();
  void CalculateUboSsboBaseOffsets();

  void AllocSubmeshVboIboStaging();
  void AllocSubmeshVboIboLocal();
  void AllocUboSsboLocalBuffers();
  void MapUboSsboLocalBuffers();

  // Static buffer data, won't change per frame
  template <MaterialTypes T>
  void FillVbosIbosUbosSubmeshMaterial(
      PcMaterialSubmeshesMap<T> &materialSubmeshesMap,
      PcMaterialMap<T> &materialMap);

  // Dynamic buffer data, they all change per frame
  template <MaterialTypes T>
  void FillUbosSubmesh(PcMaterialSubmeshesMap<T> &materialSubmeshesMap);
  void FillUbosSsbosLightCameraEmpty(std::vector<Light *> &lights,
                                     std::vector<Camera *> &cameras,
                                     std::vector<Empty *> &emptys);

  void FlushVboIboStagingToLocal();
  void CleanUpSubmeshVboIboBuffersStaging();

  void CleanUpVboIboLocalBuffers();
  void CleanUpUboSsboLocalBuffers();

public:
  [[nodiscard]] inline static MemoryVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new MemoryVk();
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
  MemoryVk(const MemoryVk &) = delete;
  MemoryVk &operator=(const MemoryVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  MemoryVk(MemoryVk &&) = delete;
  MemoryVk &operator=(MemoryVk &&) = delete;

  void CleanUp() {
    // Nothing so far
  };

private:
  MemoryVk() {
    s_deviceVk = DeviceVk::Get();
    PC_PRINT("CREATED", TagType::Constr, "MemoryVk")
  };
  ~MemoryVk() {
    s_deviceVk = nullptr;
    PC_PRINT("DESTROYED", TagType::Destr, "MemoryVk")
  };

private:
  static MemoryVk *s_instance;
  static DeviceVk *s_deviceVk;

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
  std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> m_uboSet;
  std::array<VmaAllocation, MAX_FRAMES_IN_FLIGHT> m_uboAllocSet;
  std::array<void *, MAX_FRAMES_IN_FLIGHT> m_uboMappingSet;

  std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> m_ssboSet;
  std::array<VmaAllocation, MAX_FRAMES_IN_FLIGHT> m_ssboAllocSet;
  std::array<void *, MAX_FRAMES_IN_FLIGHT> m_ssboMappingSet;

  //
  // Offsets ----------------------------------------------------------------
  PcBufferOffsets m_bufferOffsets{};

  //
  // Buffer views -----------------------------------------------------------
  PcBufferViews m_bufferViews{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
