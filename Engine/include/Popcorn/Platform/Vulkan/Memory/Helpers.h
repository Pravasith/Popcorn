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

// Note: all buffer sizes are ALIGNED!!
// -> Return type of MemoryFactoryVk::GetAccSubmeshesBufferSizes function
// struct AccSubmeshBufferSizes {
//   // VBO & IBO sizes
//   VkDeviceSize vboSize = 0;
//   VkDeviceSize iboSize = 0;
//
//   // World matrices sizes of submeshes (submeshes only, other GameObject type
//   // world matrix ubos are accumulated in the next struct)
//   VkDeviceSize submeshesWorldMatrixUboSize = 0;
//
//   VkDeviceSize basicMatUboSize = 0;
//   VkDeviceSize pbrMatUboSize = 0;
// };
//
// // Note: all buffer sizes are ALIGNED!!
// // -> Return type of MemoryFactoryVk::GetAccGameObjectsBufferSizes function
// struct AccGameObjectUboSizes {
//   VkDeviceSize lightsWorldMatrixUboSize = 0;
//   VkDeviceSize emptysWorldMatrixUboSize = 0;
//   VkDeviceSize camerasWorldMatrixUboSize = 0;
// };

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

struct GameObjectOffsets {
  // Dynamic buffers
  std::vector<VkDeviceSize> lightsWorldMatrixOffsets{};  // each element aligned
  std::vector<VkDeviceSize> emptysWorldMatrixOffsets{};  // each element aligned
  std::vector<VkDeviceSize> camerasWorldMatrixOffsets{}; // each element aligned
};

//
// --- Buffer layouts ------------------------------------------------------
//

struct UboBufferView {
  VkDeviceSize offset = 0;
  VkDeviceSize alignedSize = 0;
};

struct BufferLayouts {
  UboBufferView
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
