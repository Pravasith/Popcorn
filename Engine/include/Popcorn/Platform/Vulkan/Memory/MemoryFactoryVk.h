#pragma once

#include "CommonVk.h"
#include "DeviceVk.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "MaterialTypes.h"
#include "Memory/Helpers.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "RenderFlows/RenderFlowVk.h"
#include "Uniforms.h"
#include <cstring>
#include <glm/fwd.hpp>
#include <unordered_map>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum UboViews {
  WorldMatrixValues = 1,
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
  void CleanUpUboBuffers();

  //
  // --- UBOs ------------------------------------------------------------------
  void AllocUboLocalBuffers(AccSubmeshBufferSizes &submeshSizes,
                            AccGameObjectUboSizes &gameObjectSizes);

  //
  // --- UTILS -----------------------------------------------------------------
  template <MaterialTypes T>
  void GetAccSubmeshesBufferSizes(SubmeshGroups<T> &submeshGroups,
                                  AccSubmeshBufferSizes &sizes);

  template <GameObjectType T>
  void GetAccGameObjectsBufferSizes(std::vector<T *> &gameObjects,
                                    AccGameObjectUboSizes &sizes,
                                    VkPhysicalDeviceProperties &properties) {
    VkDeviceSize alignment = properties.limits.minUniformBufferOffsetAlignment;

    for (GameObject *gameObject : gameObjects) {
      switch (gameObject->GetType()) {
      case GameObjectTypes::Camera:
        sizes.camerasWorldMatrixUboSize +=
            PC_AlignCeil(UniformDefs::CameraUniform::size, alignment);
        break;

      case GameObjectTypes::Mesh:
        // handled already in MemoryFactoryVk::GetAccSubmeshesBufferSizes
        break;

      case GameObjectTypes::Empty:
        sizes.emptysWorldMatrixUboSize +=
            PC_AlignCeil(UniformDefs::WorldMatrixUniform::size, alignment);
        break;

      case GameObjectTypes::Light:
        sizes.lightsWorldMatrixUboSize +=
            PC_AlignCeil(UniformDefs::WorldMatrixUniform::size, alignment);
        break;

      case GameObjectTypes::None:
        PC_WARN("Unknown GameObject type");
        break;
      }
    }
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
  std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> m_uboSet;
  std::array<VmaAllocation, MAX_FRAMES_IN_FLIGHT> m_uboAllocSet;
  std::array<void *, MAX_FRAMES_IN_FLIGHT> m_uboMappingSet;

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
      {UboViews::WorldMatrixValues, {}}, {UboViews::BasicMatValues, {}},
      {UboViews::PbrMatValues, {}},      {UboViews::LightsValues, {}},
      {UboViews::CameraMatrix, {}},
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
