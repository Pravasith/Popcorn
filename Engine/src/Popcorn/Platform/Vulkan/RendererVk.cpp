#include "RendererVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "VertexBufferVk.h"
#include <cstdint>
#include <cstring>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

RendererVk::RendererVk(const Window &appWin)
    : Renderer(appWin), m_ValLayersVk(m_vkInstance),
      m_WinSurfaceVk(m_vkInstance), m_PhysDeviceVk(m_vkInstance, GetSurface()),
      m_LogiDeviceVk(m_vkInstance), m_GfxPipelineVk(),
      m_SwapChainVk(m_LogiDeviceVk.GetLogiDevice(),
                    m_PhysDeviceVk.GetPhysDevice(), m_WinSurfaceVk.GetSurface(),
                    m_PhysDeviceVk.GetQueueFamilyIndices(),
                    m_GfxPipelineVk.GetRenderPass()),
      m_CmdPoolVk(),
      m_queueFamilyIndices(m_PhysDeviceVk.GetQueueFamilyIndices()),
      m_PresentVk{m_LogiDeviceVk.GetLogiDevice(), m_SwapChainVk.GetSwapChain(),
                  m_LogiDeviceVk.GetDeviceQueue(),
                  m_LogiDeviceVk.GetPresentQueue(),
                  m_AppWin.GetFramebufferSize()} {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER-VULKAN");
};

RendererVk::~RendererVk() {
  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER-VULKAN");
  CleanUp();
};

void RendererVk::InitVulkan() {
  // CREATES INSTANCE
  CreateInstance();

  // ENABLE VALIDATION LAYERS
  if constexpr (s_enableValidationLayers) {
    m_ValLayersVk.SetupDbgMsngr();
  };

  // CREATES SURFACE FOR SWAPCHAIN
  m_WinSurfaceVk.CreateSurface(m_AppWin.GetOSWindow());

  m_PhysDeviceVk.PickPhysDevice(m_SwapChainVk);

  m_LogiDeviceVk.CreateLogicalDevice(m_PhysDeviceVk.GetQueueFamilyIndices(),
                                     m_ValLayersVk.GetValidationLayers(),
                                     m_PhysDeviceVk.GetPhysDevice(),
                                     m_PhysDeviceVk.GetDeviceExts());

  // SWAP CHAIN RELATED
  m_SwapChainVk.CreateSwapChain(
      m_AppWin.GetFramebufferSize().first, // FRAME BFR WIDTH
      m_AppWin.GetFramebufferSize().second // FRAME BFR HEIGHT
  );

  m_SwapChainVk.CreateImgViews();

  // CREATE GFX PIPELINE
  m_GfxPipelineVk.CreateRenderPass(m_SwapChainVk.GetImgFormat(),
                                   m_LogiDeviceVk.GetLogiDevice());

  m_GfxPipelineVk.AttachVertexBuffer(m_vertexBufferVk);

  m_GfxPipelineVk.CreateGfxPipeline(m_LogiDeviceVk.GetLogiDevice(),
                                    m_SwapChainVk.GetSwapChainExtent());

  // FRAME BUFFERS
  m_SwapChainVk.CreateFrameBfrs();

  // CMD BFRS
  m_CmdPoolVk.CreateCmdPool(m_PhysDeviceVk.GetQueueFamilyIndices(),
                            m_LogiDeviceVk.GetLogiDevice());

  // CREATE VULKAN VERTEX BUFFER
  m_vertexBufferVk->CreateVulkanBuffer(m_LogiDeviceVk.GetLogiDevice(),
                                       m_PhysDeviceVk);

  m_CmdPoolVk.CreateCmdBfrs(m_LogiDeviceVk.GetLogiDevice());

  // PRESENTATION SYNC OBJS FOR DRAW FRAME
  m_PresentVk.CreateSyncObjs(m_LogiDeviceVk.GetLogiDevice());
};

void RendererVk::CreateInstance() {
  VkApplicationInfo appInfo{};

  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "HELLO TRIANGLE";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Popcorn Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  // EXTENSIONS INFO
  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  auto extensions = GetRequiredExtensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  createInfo.enabledLayerCount = 0;

  // CHECK FOR VALIDATION LAYER SUPPORT
  if constexpr (s_enableValidationLayers) {

    if (!m_ValLayersVk.CheckVkVLSupport()) {
      throw std::runtime_error(
          "VALIDATION LAYERS REQUESTED, BUT NOT AVAILABLE!");
    }

    VkDebugUtilsMessengerCreateInfoEXT dbgCreateInfo{};

    // UPDATE CREATE INFO
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(m_ValLayersVk.GetValidationLayers().size());
    createInfo.ppEnabledLayerNames = m_ValLayersVk.GetValidationLayers().data();

    m_ValLayersVk.PopulateDbgMsngrCreateInfo(dbgCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&dbgCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
  }

  // CREATE INSTANCE
  VkResult result = vkCreateInstance(&createInfo, nullptr, &m_vkInstance);

  if (vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS) {
    throw std::runtime_error("FAILED TO CREATE VK INSTANCE!");
  }
};

std::vector<const char *> RendererVk::GetRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  if constexpr (s_enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  };

  return extensions;
};

bool RendererVk::OnFrameBfrResize(FrameBfrResizeEvent &e) {
  m_PresentVk.SetFrameBfrResized(true);
  return true;
};

void RendererVk::DrawFrame() {
  if (!m_vertexBufferVk) {
    return;
  };

  m_PresentVk.DrawFrame(
      m_CmdPoolVk.GetCmdBfrs(),

      // TODO: USE A LAMBDA
      CmdPoolVk::RecordCmdBfrFtr{
          m_GfxPipelineVk.GetRenderPass(), m_SwapChainVk.GetFrameBfrs(),
          m_SwapChainVk.GetSwapChainExtent(), m_GfxPipelineVk.GetGfxPipeline(),
          m_vertexBufferVk},

      // TODO: USE A LAMBDA
      SwapChainVk::RecreateSwapChainFtr{
          m_SwapChainVk, m_LogiDeviceVk.GetLogiDevice(), m_AppWin});
};

void RendererVk::CleanUp() {
  auto &device = m_LogiDeviceVk.GetLogiDevice();

  vkDeviceWaitIdle(device);
  m_vertexBufferVk->DestroyVulkanBuffer(device);
  m_SwapChainVk.CleanUp();
  m_PresentVk.CleanUp(device);
  m_CmdPoolVk.CleanUp(device);
  m_GfxPipelineVk.CleanUp(device);
  m_WinSurfaceVk.CleanUp();
  m_LogiDeviceVk.CleanUp();
  m_ValLayersVk.CleanUp();

  vkDestroyInstance(m_vkInstance, nullptr);
  m_vkInstance = VK_NULL_HANDLE;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
