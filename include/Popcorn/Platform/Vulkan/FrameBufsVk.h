#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class FrameBufsVk {
  friend class RendererVk;

  FrameBufsVk() { PC_PRINT("CREATED", TagType::Constr, "FRAME-BUFS-VK") };
  ~FrameBufsVk() { PC_PRINT("DESTROYED", TagType::Destr, "FRAME-BUFS-VK") };

  [[nodiscard]] inline const std::vector<VkFramebuffer> &
  GetSwpChnFrameBfrs() const {
    if (m_swpChnFrameBufs.size() == 0) {
      throw std::runtime_error("ERROR: SWAP CHAIN FRAME BUFFERS VEC IS EMPTY!");
    };

    return m_swpChnFrameBufs;
  };

  void CreateFrameBfrs(const VkDevice &, const std::vector<VkImageView> &,
                       const VkRenderPass &, const VkExtent2D &);
  void CleanUp(const VkDevice &);

private:
  std::vector<VkFramebuffer> m_swpChnFrameBufs;
};
ENGINE_NAMESPACE_END
