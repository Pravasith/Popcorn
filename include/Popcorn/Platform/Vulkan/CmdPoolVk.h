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
                          const VkExtent2D &, const VkPipeline &);

private:
  CmdPoolVk() { PC_PRINT("CREATED", TagType::Constr, "COMMAND-POOL-VK") };
  ~CmdPoolVk() { PC_PRINT("DESTROYED", TagType::Destr, "COMMAND-POOL-VK") };

  [[nodiscard]] inline VkCommandBuffer &GetCmdBfr() {
    if (m_cmdBfr == NULL) {
      throw std::runtime_error("ERROR: COMMAND BFR IS NULL!");
    };

    return m_cmdBfr;
  };

  void CreateCmdPool(const QueueFamilyIndices &, const VkDevice &);
  void CreateCmdBfr(const VkDevice &);
  void RecordCmdBfr(VkCommandBuffer, uint32_t imgIdx, const VkRenderPass &,
                    const std::vector<VkFramebuffer> &, const VkExtent2D &,
                    const VkPipeline &);
  void CleanUp(const VkDevice &dev);

private:
  VkCommandPool m_cmdPool;
  VkCommandBuffer m_cmdBfr;
};

ENGINE_NAMESPACE_END
