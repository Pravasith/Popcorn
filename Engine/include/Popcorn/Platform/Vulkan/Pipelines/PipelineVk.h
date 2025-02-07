
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class PipelineVk {
  enum Types { None = 0, GfxPipeline };

public:
  PipelineVk() { PC_PRINT("CREATED", TagType::Constr, "PipelineVk"); };
  ~PipelineVk() { PC_PRINT("DESTROYED", TagType::Destr, "PipelineVk"); };

  // class Builder {
  //   void Build(PipelineVk *pipeline) {};
  // };

  inline static VkShaderModule CreateShaderModule(const VkDevice &device,
                                                  const Buffer &bytecode) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = bytecode.GetSize();
    createInfo.pCode = bytecode.AsType<uint32_t>();

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
