#pragma once

#include "Camera.h"
#include "CommonVk.h"
#include "DeviceVk.h"
#include "Empty.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Light.h"
#include "MaterialTypes.h"
#include "Memory/Memory.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "Uniforms.h"
#include <cstring>
#include <glm/fwd.hpp>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class MemoryFactoryVk {
public:
  //
  //
  // --- SUBMESH & MATERIAL ----------------------------------------------------
  // --- SUBMESH & MATERIAL ----------------------------------------------------
  // --- SUBMESH & MATERIAL ----------------------------------------------------
  template <MaterialTypes T>
  void
  ExtractMaterialSubmeshOffsets(MaterialSubmeshesMap<T> &materialSubmeshesMap);

  void ExtractLightsCamerasEmptysOffsets(std::vector<Light *> &lights,
                                         std::vector<Camera *> &cameras,
                                         std::vector<Empty *> &emptys);
  void SetBufferViewOffsets();

  // TODO:
  // -------------------------------------------------
  // 1. Adjust bufferViews of objects - 20 mins
  //    - Set each bufferView offset (for UBOs & SSBOs)
  //    - Check offsets & sizes for minBufferAlignment
  //
  // 2. Alloc Vulkan Memory for - 20 mins
  //    a. Submesh world matrices - UBO
  //    b. Basic material values - UBO
  //    c. Pbr material values - UBO
  //    d. Camera matrix & other values - UBO
  //    e. Empty values - UBO
  //    f. Lights values - SSBO
  //
  // 3. Create mappings for all needed ubos & ssbos - 20 mins
  //    a. basicMatUboMapping
  //    b. pbrMatUboMapping
  //    c. submeshUboMapping
  //    d. camerasUboMapping
  //    e. emptysUboMapping
  //    f. LightsSsboMapping
  //
  // 4. Descriptor sets allocation --
  //    - Plan this before copying values
  //    - basically bind memory (buffers, images & samplers) to descriptor sets
  //      and get them ready for the scene updates process function
  //
  // 5. Process scene updates - Copy buffer values --
  //    a. Submesh world matrices - UBO - submesh->GetWorldMatrix()
  //    b. Basic material values - UBO - materialsMap[matId]
  //    c. Pbr material values - UBO - materialsMap[matId]
  //    d. Camera matrix & other values - UBO - camera->GetWorldMatrix(),
  //       camera->GetViewProjMatrix() ..etc
  //    e. Empty values - UBO - empty->GetEmptyValues()
  //    f. Lights values - SSBO - light->GetLightValues()
  //
  // 6. Make pipelines ready for each renderflows
  // 7. Finally, the draw call
  //
  //
  //
  void AllocVboIboStagingBuffers(VkDeviceSize vboSize, VkDeviceSize iboSize);
  void AllocVboIboLocalBuffers(VkDeviceSize vboSize, VkDeviceSize iboSize);
  void AllocUboLocalBuffers(AccSubmeshBufferSizes &submeshSizes,
                            AccGameObjectUboSizes &gameObjectSizes);

  template <MaterialTypes T>
  void
  FillMaterialSubmeshBuffers(MaterialSubmeshesMap<T> &materialSubmeshesMap);

  void FlushVBOsAndIBOsStagingToLocal(VkDeviceSize submeshVbosSize,
                                      VkDeviceSize submeshIbosSize);
  void CleanUpVboIboStagingBuffers();

  void CleanUpVboIboLocalBuffers();
  void CleanUpUboBuffers();

  //
  // --- UBOs ------------------------------------------------------------------

  //
  // --- UTILS -----------------------------------------------------------------
  [[nodiscard]] const BufferViews &GetBufferViews() const {
    return m_bufferViews;
  };

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
