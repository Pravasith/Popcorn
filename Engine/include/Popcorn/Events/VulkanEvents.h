#pragma once

#include "Event.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN

class ImageVkLayoutInitialToFinalEvent : public Event {
public:
  ImageVkLayoutInitialToFinalEvent(const VkImageLayout &initialLayout,
                                   const VkImageLayout &finalLayout)
      : m_layouts{initialLayout, finalLayout} {};

  // EVENT_CATEGORY_OVERRIDE_METHODS(WindowEvent)
  // EVENT_TYPE_OVERRIDE_METHODS(WindowClose)

  void PrintDebugData() const override {
    PC_PRINT("Image layout changed: initial -> final", TagType::Print,
             "Vulkan Event");
  };

  struct Layouts {
    const VkImageLayout &m_initialLayout;
    const VkImageLayout &m_finalLayout;
  };

private:
  Layouts m_layouts;
};

ENGINE_NAMESPACE_END
