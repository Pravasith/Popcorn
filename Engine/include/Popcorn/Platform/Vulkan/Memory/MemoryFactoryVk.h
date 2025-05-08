#pragma once

#include "Camera.h"
#include "CommonVk.h"
#include "DeviceVk.h"
#include "Empty.h"
#include "GlobalMacros.h"
#include "Light.h"
#include "MaterialTypes.h"
#include "Memory/Memory.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include <cstring>
#include <glm/fwd.hpp>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class MemoryFactoryVk {
public:
  template <MaterialTypes T>
  void ExtractOffsetsMaterialsSubmeshes(
      MaterialSubmeshesMap<T> &materialSubmeshesMap);

  void ExtractOffsetsLightsCamerasEmptys(std::vector<Light *> &lights,
                                         std::vector<Camera *> &cameras,
                                         std::vector<Empty *> &emptys);
  void AlignVboIboBufferViews();
  void CalculateUboSsboBaseOffsets();

  void AllocSubmeshVboIboStaging();
  void AllocSubmeshVboIboLocal();
  void AllocUboSsboLocalBuffers();

  template <MaterialTypes T>
  void
  FillBuffersMaterialsSubmeshes(MaterialSubmeshesMap<T> &materialSubmeshesMap);

  void FlushVboIboStagingToLocal();
  void CleanUpSubmeshVboIboBuffersStaging();

  void CleanUpVboIboLocalBuffers();
  void CleanUpUboBuffers();

  [[nodiscard]] const BufferViews &GetBufferViews() const {
    return m_bufferViews;
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

  //
  // Offsets ----------------------------------------------------------------
  BufferOffsets m_bufferOffsets{};

  //
  // Buffer views -----------------------------------------------------------
  BufferViews m_bufferViews{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
