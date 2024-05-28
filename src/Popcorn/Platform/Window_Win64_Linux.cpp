#include "Window_Win64_Linux.h"
#include "GLFW_Funcs.h"
#include "Global_Macros.h"
#include "MouseEvent.h"
#include "Utilities.h"
#include "WindowEvent.h"
#include <cstdint>
#include <string>

ENGINE_NAMESPACE_BEGIN
using namespace GLFW_Funcs;

GLFW_Types::GLFW_OSWindow_T *WindowWin64Linux::s_os_window = nullptr;
WindowWin64Linux *WindowWin64Linux::s_instance = nullptr;

Window *WindowWin64Linux::Init(const Props &props) {
  if (!s_instance) {
    s_instance = new WindowWin64Linux(props);
  }

  return s_instance;
}

WindowWin64Linux::WindowWin64Linux(const Props &props) : m_title(props.Title) {
  PC_PRINT_DEBUG("WIN64 CLASS " << m_title << "CREATED", 3, "WIN64");

  GLFW_Init();
  PC_PRINT_DEBUG("GLFW INIT", 4, "WIN64");

  GLFW_CreateWindow(&s_os_window, m_title, props.W, props.H);
  GLFW_SetWindowCallbacks(
      s_os_window,

      // Window Close Callback
      [](GLFW_Types::GLFW_OSWindow_T *os_window) {
        WindowCloseEvent winCloseEvt;
        s_instance->PublishEvent(winCloseEvt);
      },

      // Mouse Position Callback
      [](GLFW_Types::GLFW_OSWindow_T *os_window, double x, double y) {
        MouseMovedEvent mMoveEvt((float)x, (float)y);
        s_instance->PublishEvent(mMoveEvt);
      }

  );

  if (!s_os_window) {
    write_log("No OS Window yet!");
  }
}

void WindowWin64Linux::StartWindowLoop() { GLFW_WindowLoop(s_os_window); }

WindowWin64Linux::~WindowWin64Linux() {
  GLFW_Terminate();

  PC_PRINT_DEBUG("GLFW KILLED", 4, "WIN64");
  PC_PRINT_DEBUG("WIN64 CLASS "
                 "DESTROYED",
                 3, "WIN64");
};

void WindowWin64Linux::Terminate() {
  if (!s_os_window || !s_instance) {
    write_log("Error: WindowWin64Linux Terminate -- s_os_window or s_instance "
              "of class not found.");
  }

  else {
    // No need to delete bc GLFW_Terminate() handles deletion
    s_os_window = nullptr;

    delete s_instance;
    s_instance = nullptr;
  }
}

uint16_t WindowWin64Linux::GetWidth() const { return 1; }

uint16_t WindowWin64Linux::GetHeight() const { return 1; }

void *WindowWin64Linux::GetOSWindow() const {
  if (s_os_window)
    return s_os_window;

  return nullptr;
}

ENGINE_NAMESPACE_END
