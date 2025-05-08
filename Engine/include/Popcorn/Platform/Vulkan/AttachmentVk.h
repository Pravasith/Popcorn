#pragma once

#include "GlobalMacros.h"
#include "ImageVk.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class AttachmentVk {
public:
  AttachmentVk() { PC_PRINT("CREATED", TagType::Constr, "AttachmentVk.h") };
  ~AttachmentVk() { PC_PRINT("DESTROYED", TagType::Destr, "AttachmentVk.h") };

  void SetAttachmentDescription(const VkAttachmentDescription &description) {
    m_description = description;
  };
  [[nodiscard]] const VkAttachmentDescription &
  GetAttachmentDescription() const {
    return m_description;
  };

  void SetImageVk(ImageVk *imageVk) {
    PC_ASSERT(imageVk != nullptr, "imageVk is nullptr");
    m_imageVk = imageVk;
  };

  static void
  GetDefaultAttachmentDescription(VkAttachmentDescription &attachment) {
    // Color attachment
    attachment.format = VK_FORMAT_B8G8R8A8_SRGB; // Ex. Swapchain image format
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  };

  static void GetAttachmentRef(VkAttachmentReference &attachmentRef,
                               const uint32_t attachmentIndex) {
    attachmentRef.attachment = attachmentIndex;
    attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  };

private:
  ImageVk *m_imageVk = nullptr;
  VkAttachmentDescription m_description{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
