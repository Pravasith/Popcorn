#pragma once

#include "GlobalMacros.h"
#include "Material.h"
#include "MaterialTypes.h"
#include "Mesh.h"
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
// --- Typedefs ------------------------------------------------------------
template <MaterialTypes T>
using MaterialMap = std::unordered_map<MaterialHashType, Material<T> *>;

using MaterialOffsetsMap = std::unordered_map<MaterialHashType, VkDeviceSize>;

template <MaterialTypes T>
using MaterialSubmeshesMap =
    std::unordered_map<MaterialHashType, std::vector<Submesh<T> *>>;

struct SubmeshOffsets {
  VkDeviceSize vboOffset = 0;
  VkDeviceSize iboOffset = 0;
  VkDeviceSize worldMatrixOffset = 0;
};

using SubmeshesOffsetsMap =
    std::unordered_map<MaterialHashType, std::vector<SubmeshOffsets>>;

//
// --- Buffer offsets ------------------------------------------------------
struct BufferOffsets {
  SubmeshesOffsetsMap submeshesOffsets{}; // aligned
  MaterialOffsetsMap materialOffsets{};   // aligned

  std::vector<VkDeviceSize> lightsWorldMatrixOffsets{};  // aligned
  std::vector<VkDeviceSize> camerasWorldMatrixOffsets{}; // aligned
  std::vector<VkDeviceSize> emptysWorldMatrixOffsets{};  // aligned
};

//
// --- Buffer views --------------------------------------------------------
struct BufferView {
  VkDeviceSize offset = 0;
  VkDeviceSize size = 0;

  void operator+=(const BufferView &other) {
    offset += other.offset;
    size += other.size;
  };
};

struct BufferViews {
  // VBOs & IBOs -----------------
  BufferView submeshVbo{}; // done
  BufferView submeshIbo{}; // done

  // UBOs ------------------------
  BufferView basicMatUbo{}; // done
  BufferView pbrMatUbo{};   // done
  BufferView submeshUbo{};  // done
  BufferView camerasUbo{};  // done
  BufferView emptysUbo{};   // done

  // SSBOs -----------------------
  BufferView lightsSsbo{}; // done
};

static constexpr VkDeviceSize PC_AlignCeil(VkDeviceSize size,
                                           VkDeviceSize alignment) {
  VkDeviceSize result = 0;
  result = (size + (alignment - 1)) & ~(alignment - 1);

  return result;
};

static constexpr VkDeviceSize PC_AlignFloor(VkDeviceSize size,
                                            VkDeviceSize alignment) {
  VkDeviceSize result = 0;
  result = size & ~(alignment - 1);

  return result;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
