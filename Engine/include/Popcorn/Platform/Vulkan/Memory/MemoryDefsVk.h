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
using PcMaterialMap = std::unordered_map<MaterialHashType, Material<T> *>;
using PcMaterialOffsetsMap = std::unordered_map<MaterialHashType, VkDeviceSize>;

template <MaterialTypes T>
using PcMaterialSubmeshesMap =
    std::unordered_map<MaterialHashType, std::vector<Submesh<T> *>>;

struct PcSubmeshOffsets {
  VkDeviceSize vboOffset = 0;
  VkDeviceSize iboOffset = 0;
  VkDeviceSize worldMatrixOffset = 0;
};

using PcSubmeshesOffsetsMap =
    std::unordered_map<MaterialHashType, std::vector<PcSubmeshOffsets>>;

//
// --- Buffer offsets ------------------------------------------------------
struct PcBufferOffsets {
  PcSubmeshesOffsetsMap submeshesOffsets{}; // aligned
  PcMaterialOffsetsMap materialOffsets{};   // aligned

  std::vector<VkDeviceSize> lightsOffsets{};  // aligned
  std::vector<VkDeviceSize> camerasOffsets{}; // aligned
  std::vector<VkDeviceSize> emptysOffsets{};  // aligned
};

//
// --- Buffer views --------------------------------------------------------
struct PcBufferView {
  VkDeviceSize offset = 0;
  VkDeviceSize alignedSize = 0;

  void operator+=(const PcBufferView &other) {
    offset += other.offset;
    alignedSize += other.alignedSize;
  };
};

struct PcBufferViews {
  // VBOs & IBOs -----------------
  PcBufferView submeshVbo{}; // done
  PcBufferView submeshIbo{}; // done

  // UBOs ------------------------
  PcBufferView basicMatUbo{}; // done
  PcBufferView pbrMatUbo{};   // done
  PcBufferView submeshUbo{};  // done
  PcBufferView camerasUbo{};  // done
  PcBufferView emptysUbo{};   // done

  // SSBOs -----------------------
  PcBufferView lightsSsbo{}; // done
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
