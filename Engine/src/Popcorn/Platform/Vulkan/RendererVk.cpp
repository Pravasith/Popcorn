#include "RendererVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <cstring>
#include <vector>

ENGINE_NAMESPACE_BEGIN

RendererVk::RendererVk(const Window &appWin)
    : Renderer(appWin), m_ValLyrsVk(m_vkInstance), m_WinSrfcVk(m_vkInstance),
      m_PhysDevVk(m_vkInstance, GetSurface()), m_LogiDevVk(m_vkInstance),
      m_GfxPlineVk(),
      m_SwpChnVk(m_LogiDevVk.GetLogiDevice(), m_PhysDevVk.GetPhysDevice(),
                 m_WinSrfcVk.GetSurface(), m_PhysDevVk.GetQueueFamilyIndices(),
                 m_GfxPlineVk.GetRenderPass()),
      m_CmdPoolVk(), m_qFamIndices(m_PhysDevVk.GetQueueFamilyIndices()),
      m_PresentVk{m_LogiDevVk.GetLogiDevice(), m_SwpChnVk.GetSwapChain(),
                  m_LogiDevVk.GetDeviceQueue(), m_LogiDevVk.GetPresentQueue(),
                  m_AppWin.GetFramebufferSize()} {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER-VULKAN");
  InitVulkan();
};

RendererVk::~RendererVk() {
  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER-VULKAN");
  CleanUp();
};

void RendererVk::InitVulkan() {
  CreateInstance();

  if constexpr (s_enableValidationLayers) {
    m_ValLyrsVk.SetupDbgMsngr();
  };

  // SURFACE, PHYS DEVICE, LOGICAL DEVICE CREATION
  m_WinSrfcVk.CreateSurface(m_AppWin.GetOSWindow());
  m_PhysDevVk.PickPhysDevice(m_SwpChnVk);
  m_LogiDevVk.CreateLogicalDevice(
      m_PhysDevVk.GetQueueFamilyIndices(), m_ValLyrsVk.GetValidationLayers(),
      m_PhysDevVk.GetPhysDevice(), m_PhysDevVk.GetDeviceExts());

  // SWAP CHAIN RELATED
  m_SwpChnVk.CreateSwapChain(
      m_AppWin.GetFramebufferSize().first, // FRAME BFR WIDTH
      m_AppWin.GetFramebufferSize().second // FRAME BFR HEIGHT
  );
  m_SwpChnVk.CreateImgViews();

  // CREATE GFX PIPELINE
  m_GfxPlineVk.CreateRenderPass(m_SwpChnVk.GetImgFormat(),
                                m_LogiDevVk.GetLogiDevice());
  m_GfxPlineVk.CreateGfxPipeline(m_LogiDevVk.GetLogiDevice(),
                                 m_SwpChnVk.GetSwapChainExtent());

  // FRAME BUFFERS
  m_SwpChnVk.CreateFrameBfrs();

  // CMD BFRS
  m_CmdPoolVk.CreateCmdPool(m_PhysDevVk.GetQueueFamilyIndices(),
                            m_LogiDevVk.GetLogiDevice());
  m_CmdPoolVk.CreateCmdBfrs(m_LogiDevVk.GetLogiDevice());

  // PRESENTATION SYNC OBJS FOR DRAW FRAME
  m_PresentVk.CreateSyncObjs(m_LogiDevVk.GetLogiDevice());
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

    if (!m_ValLyrsVk.CheckVkVLSupport()) {
      throw std::runtime_error(
          "VALIDATION LAYERS REQUESTED, BUT NOT AVAILABLE!");
    }

    VkDebugUtilsMessengerCreateInfoEXT dbgCreateInfo{};

    // UPDATE CREATE INFO
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(m_ValLyrsVk.GetValidationLayers().size());
    createInfo.ppEnabledLayerNames = m_ValLyrsVk.GetValidationLayers().data();

    m_ValLyrsVk.PopulateDbgMsngrCreateInfo(dbgCreateInfo);
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
  m_PresentVk.DrawFrame(
      m_CmdPoolVk.GetCmdBfrs(),
      // TODO: USE A LAMBDA
      CmdPoolVk::RecordCmdBfrFtr{
          m_GfxPlineVk.GetRenderPass(), m_SwpChnVk.GetFrameBfrs(),
          m_SwpChnVk.GetSwapChainExtent(), m_GfxPlineVk.GetGfxPipeline()},
      // TODO: USE A LAMBDA
      SwapChainVk::RecreateSwapChainFtr{m_SwpChnVk, m_LogiDevVk.GetLogiDevice(),
                                        m_AppWin});
};

void RendererVk::CleanUp() {
  vkDeviceWaitIdle(m_LogiDevVk.GetLogiDevice());

  m_SwpChnVk.CleanUp();
  m_PresentVk.CleanUp(m_LogiDevVk.GetLogiDevice());
  m_CmdPoolVk.CleanUp(m_LogiDevVk.GetLogiDevice());
  m_GfxPlineVk.CleanUp(m_LogiDevVk.GetLogiDevice());
  m_WinSrfcVk.CleanUp();
  m_LogiDevVk.CleanUp();
  m_ValLyrsVk.CleanUp();

  vkDestroyInstance(m_vkInstance, nullptr);
  m_vkInstance = VK_NULL_HANDLE;
};

ENGINE_NAMESPACE_END
