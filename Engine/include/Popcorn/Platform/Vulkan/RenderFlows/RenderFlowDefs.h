#pragma once

#include "AttachmentVk.h"
#include "CommonVk.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include <array>
#include <glm/fwd.hpp>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum RenderFlows { GBuffer = 1, Lighting, Composite };

using PcFramesImages = std::array<ImageVk, MAX_FRAMES_IN_FLIGHT>;
using PcFramesAttachments = std::array<AttachmentVk, MAX_FRAMES_IN_FLIGHT>;
using PcFramesDescriptorSets =
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT>;
using PcFramesFramebuffers = std::array<VkFramebuffer, MAX_FRAMES_IN_FLIGHT>;

template <RenderFlows T> struct PcRenderFlowImages {
  virtual ~PcRenderFlowImages() = default;
  static constexpr RenderFlows type_value = T;
};

template <> struct PcRenderFlowImages<RenderFlows::GBuffer> {
  PcFramesImages albedoImages{};
  PcFramesImages depthImages{};
  PcFramesImages normalImages{};
  PcFramesImages roughnessMetallicImages{};
};

template <> struct PcRenderFlowImages<RenderFlows::Lighting> {
  PcFramesImages lightImages{};
};

template <> struct PcRenderFlowImages<RenderFlows::Composite> {
  PcFramesImages presentImages{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
