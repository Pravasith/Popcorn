#pragma once

#include "AttachmentVk.h"
#include "CommonVk.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "Popcorn/Core/Assert.h"
#include <array>
#include <cstdint>
#include <glm/fwd.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum RenderFlows { GBuffer = 1, Lighting, Composite };

//
// --------------------------------------------------------------------------
template <uint32_t FrameCount>
using PcFramesImages = std::array<ImageVk, FrameCount>;

template <uint32_t FrameCount>
using PcFramesAttachments = std::array<AttachmentVk, FrameCount>;

template <uint32_t FrameCount>
using PcFramesDescriptorSets = std::array<VkDescriptorSet, FrameCount>;

template <uint32_t FrameCount>
using PcFramesFramebuffers = std::array<VkFramebuffer, FrameCount>;

//
// --------------------------------------------------------------------------
using PcPresentationAttachments = std::vector<AttachmentVk>;
using PcPresentationFramebuffers = std::vector<VkFramebuffer>;

//
// --------------------------------------------------------------------------
template <RenderFlows T, uint32_t Count = 0> struct PcRenderFlowImages {
  static constexpr RenderFlows type_value = T;
  static constexpr uint32_t frameCount = Count;
};

template <uint32_t Count>
struct PcRenderFlowImages<RenderFlows::GBuffer, Count> {
  PC_STATIC_ASSERT(Count == MAX_FRAMES_IN_FLIGHT,
                   "Count must be equal to MAX_FRAMES_IN_FLIGHT for Gbuffer");
  PcFramesImages<Count> albedoImages{};
  PcFramesImages<Count> depthImages{};
  PcFramesImages<Count> normalImages{};
  PcFramesImages<Count> roughnessMetallicImages{};
};

template <uint32_t Count>
struct PcRenderFlowImages<RenderFlows::Lighting, Count> {
  PC_STATIC_ASSERT(Count == MAX_FRAMES_IN_FLIGHT,
                   "Count must be equal to MAX_FRAMES_IN_FLIGHT for Lighting");
  PcFramesImages<Count> lightImages{};
};

template <> struct PcRenderFlowImages<RenderFlows::Composite> {
  std::vector<ImageVk> swapchainImages{};
  // std::vector<ImageVk> postFxImage1{}; // PostFx images - potentially
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
