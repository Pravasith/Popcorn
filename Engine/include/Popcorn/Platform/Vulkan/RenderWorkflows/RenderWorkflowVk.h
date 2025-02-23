#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Scene.h"
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Material;

enum class RenderWorkflowIndices {
  Basic = 0,   // Basic colors
  Shadows = 1, // Simple hard shadows
};

class RenderWorkflowVk {
public:
  RenderWorkflowVk() {
    PC_PRINT("CREATED", TagType::Constr, "RenderWorkflowVk")
  };
  virtual ~RenderWorkflowVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "RenderWorkflowVk")
  };

  //
  // PURE
  virtual void CreateWorkflowResources(Material *materialPtr) = 0;
  virtual void RecordRenderCommands(const Scene &scene,
                                    const VkCommandBuffer &commandBuffer,
                                    uint32_t imageIndex) = 0;

private:
  //
  // WORKFLOW UTILS
  virtual void CreateRenderPass() {};
  virtual void CreateVkPipeline(Material &) {};
  virtual void CreateFramebuffers() {};
  virtual void CreateCommandBuffer() {};

  virtual void CleanUp() = 0;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
