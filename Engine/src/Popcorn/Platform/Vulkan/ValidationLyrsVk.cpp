#include "ValidationLyrsVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

VkResult PC_CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void PC_DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                      VkDebugUtilsMessengerEXT debugMessenger,
                                      const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

ValidationLyrsVk::ValidationLyrsVk(const VkInstance &vkInst)
    : m_vkInst(vkInst) {
        PC_PRINT("CREATED", TagType::Constr, "VK-VALIDATION-LAYERS")
      };

ValidationLyrsVk::~ValidationLyrsVk() {
  PC_PRINT("DESTROYED", TagType::Destr, "VK-VALIDATION-LAYERS")
};

void ValidationLyrsVk::PopulateDbgMsngrCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = DebugCallback;
}

bool ValidationLyrsVk::CheckVkVLSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : m_validationLayers) {
    bool layerFound = false;

    for (const auto &layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
};

void ValidationLyrsVk::SetupDbgMsngr() {
  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  PopulateDbgMsngrCreateInfo(createInfo);

  if (PC_CreateDebugUtilsMessengerEXT(m_vkInst, &createInfo, nullptr,
                                      &m_debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("FAILED TO SET UP DEBUG MESSENGER!");
  }
};

void ValidationLyrsVk::CleanUp() {
  PC_DestroyDebugUtilsMessengerEXT(m_vkInst, m_debugMessenger, nullptr);
  m_debugMessenger = VK_NULL_HANDLE;
};

// STATIC FUNCTIONS -----------------------------------------------------------
//
VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLyrsVk::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {
  PC_ERROR(pCallbackData->pMessage, "VALIDATION LAYER");
  return VK_FALSE;
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
