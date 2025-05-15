#pragma once

#include "GlobalMacros.h"
#include "ImageVk.h"
#include <glm/fwd.hpp>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum RenderFlows { GBuffer = 1, Lighting, Composite };

template <RenderFlows T> struct PcRenderFlowImages {
  virtual ~PcRenderFlowImages() = default;
  static constexpr RenderFlows type_value = T;
};

template <> struct PcRenderFlowImages<RenderFlows::GBuffer> {
  ImageVk albedoImage{};
  ImageVk depthImage{};
  ImageVk normalImage{};
};

template <> struct PcRenderFlowImages<RenderFlows::Lighting> {
  ImageVk lightImage{};
};

template <> struct PcRenderFlowImages<RenderFlows::Composite> {
  ImageVk presentImage{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
