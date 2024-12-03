#pragma once
#include "CommonVk.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class CmdPoolVk {
  friend class RendererVk;

public:
  using RecordCmdBfrPtr =
      void (CmdPoolVk::*)(VkCommandBuffer, uint32_t, const VkRenderPass &,
                          const std::vector<VkFramebuffer> &,
                          const VkExtent2D &, const VkPipeline &) const;

private:
  CmdPoolVk() { PC_PRINT("CREATED", TagType::Constr, "COMMAND-POOL-VK") };
  ~CmdPoolVk() { PC_PRINT("DESTROYED", TagType::Destr, "COMMAND-POOL-VK") };

  [[nodiscard]] inline std::vector<VkCommandBuffer> &GetCmdBfrs() {
    if (m_cmdBfrs.size() == 0) {
      throw std::runtime_error("ERROR: COMMAND BFRS EMPTY!");
    };

    return m_cmdBfrs;
  };

  void CreateCmdPool(const QueueFamilyIndices &, const VkDevice &);
  void CreateCmdBfrs(const VkDevice &);
  void RecordCmdBfr(VkCommandBuffer, uint32_t imgIdx, const VkRenderPass &,
                    const std::vector<VkFramebuffer> &, const VkExtent2D &,
                    const VkPipeline &) const;
  void CleanUp(const VkDevice &dev);

private:
  VkCommandPool m_cmdPool;
  std::vector<VkCommandBuffer> m_cmdBfrs;
};

ENGINE_NAMESPACE_END
