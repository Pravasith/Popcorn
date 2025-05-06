#pragma once

#include "GlobalMacros.h"
#include "MaterialTypes.h"
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

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

//
// --- Buffer offsets ------------------------------------------------------
struct MaterialOffsets {
  std::unordered_map<MaterialHashType, VkDeviceSize> basicMaterialOffsets{};
  std::unordered_map<MaterialHashType, VkDeviceSize> pbrMaterialOffsets{};
};

struct SubmeshOffsets {
  // Static buffers
  std::vector<VkDeviceSize> vboOffsets{}; // aligned to whole size
  std::vector<VkDeviceSize> iboOffsets{}; // aligned to whole size

  // Dynamic buffers
  std::vector<VkDeviceSize> worldMatrixOffsets{}; // each element aligned
};

struct EmptysCamerasLightsOffsets {
  // Dynamic buffers
  std::vector<VkDeviceSize> emptysWorldMatrixOffsets{};  // each element aligned
  std::vector<VkDeviceSize> camerasWorldMatrixOffsets{}; // each element aligned
  std::vector<VkDeviceSize> lightsWorldMatrixOffsets{};  // each element aligned
};

//
// --- Buffer views --------------------------------------------------------
struct BufferView {
  VkDeviceSize offset = 0;
  VkDeviceSize alignedSize = 0;
};

struct UboBufferViews {
  // Material info values for each unique type
  BufferView basicMatVkUbo{};
  BufferView pbrMatVkUbo{};

  // World matrices
  BufferView submeshVkUbo{};
  BufferView emptysVkUbo{};
  BufferView camerasVkUbo{};
};

struct SsboBufferViews {
  // Lights info values
  BufferView lightsVkSsbo{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
