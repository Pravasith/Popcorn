#pragma once

#include "CommonVk.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <stdexcept>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class CmdPoolVk {
  friend class RendererVk;

public:
  // TODO: CHANGE NAME
  // FUNCTOR TO RECORD CMD BFRS
  struct RecordCmdBfrFtr {
    RecordCmdBfrFtr(const VkRenderPass &rpass,
                    const std::vector<VkFramebuffer> &fbfrs,
                    const VkExtent2D &ext, const VkPipeline &pl)
        : m_rndrPass(rpass), m_frameBfrs(fbfrs), m_swpChnExt(ext),
          m_pipeline(pl) {};

    void operator()(VkCommandBuffer cmdBfr, uint32_t imgIdx);

    const VkRenderPass &m_rndrPass;
    const std::vector<VkFramebuffer> &m_frameBfrs;
    const VkExtent2D &m_swpChnExt;
    const VkPipeline &m_pipeline;
  };

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
  void CleanUp(const VkDevice &dev);

private:
  VkCommandPool m_cmdPool;
  std::vector<VkCommandBuffer> m_cmdBfrs;
};

ENGINE_NAMESPACE_END
