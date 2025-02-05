#pragma once

#include "GlobalMacros.h"
#include "Layer.h"
#include "Popcorn/Events/Event.h"
#include <imgui.h>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

ENGINE_NAMESPACE_BEGIN
class DebugUIOverlay : public Layer {
public:
  DebugUIOverlay();
  ~DebugUIOverlay();

  void OnAttach() override;
  void OnDetach() override;
  void OnUpdate() override;
  void OnEvent(Event &e) override;

  // Imgui impl. methods
  static VkPhysicalDevice SetupVulkan_SelectPhysicalDevice();
  static void SetupVulkan(ImVector<const char *> instance_extensions);
  static void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd,
                                VkSurfaceKHR surface, int width, int height);
  static void CleanupVulkan();
  static void CleanupVulkanWindow();
  static void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data);
  static void FramePresent(ImGui_ImplVulkanH_Window *wd);

private:
  static GLFWwindow *s_osWindow;
  ImGuiIO m_io;

  // Imgui impl. variables
  static VkAllocationCallbacks *g_Allocator;
  static VkInstance g_Instance;
  static VkPhysicalDevice g_PhysicalDevice;
  static VkDevice g_Device;
  static uint32_t g_QueueFamily;
  static VkQueue g_Queue;
  static VkDebugReportCallbackEXT g_DebugReport;
  static VkPipelineCache g_PipelineCache;
  static VkDescriptorPool g_DescriptorPool;
  static ImGui_ImplVulkanH_Window g_MainWindowData;
  static int g_MinImageCount;
  static bool g_SwapChainRebuild;
};

ENGINE_NAMESPACE_END
