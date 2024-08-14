#include <glad/glad.h>
#include "Window_Win64_Linux.h"
#include "GLFW_Funcs.h"
#include "Global_Macros.h"
#include "KeyEvent.h"
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

  // CREATE GLFW WINDOW AND MAKE CONTEXT CURRENT
  GLFW_CreateWindow(&s_os_window, m_title, props.W, props.H);

  int status = gladLoadGLLoader(GLADloadproc(GLFW_Funcs::GLFW_GetProcAddress));

  GLFW_SetWindowCallbacks(
      s_os_window,

      /* Window Close Callback */
      [](GLFW_Types::GLFW_OSWindow_T *os_window) {
        WindowCloseEvent winCloseEvt;
        s_instance->PublishEvent(winCloseEvt);
      },

      /* Mouse Position Callback */
      [](GLFW_Types::GLFW_OSWindow_T *os_window, double x, double y) {
        MouseMovedEvent mMoveEvt((float)x, (float)y);
        s_instance->PublishEvent(mMoveEvt);
      },

      /* Key Callback */
      [](GLFW_Types::GLFW_OSWindow_T *os_window, int key, int scan_code,
         int action, int mods) {
        // GLFW ACTIONS
        // 0 = KEY_PRESS, 1 = KEY_RELEASE, 2 = KEY_REPEAT

        switch (action) {
        case GLFW_PRESS: {
          KeyPressedEvent event(key, 0);
          s_instance->PublishEvent(event);
          break;
        }
        case GLFW_RELEASE: {
          KeyReleasedEvent event(key);
          s_instance->PublishEvent(event);
          break;
        }
        case GLFW_REPEAT: {
          KeyPressedEvent event(key, true);
          s_instance->PublishEvent(event);
          break;
        }
        }
      },

      /* Window Resize Callback */
      [](GLFW_Types::GLFW_OSWindow_T *os_window, int width, int height) {
        WindowResizeEvent event(width, height);
        s_instance->PublishEvent(event);
      }

  );

  if (!s_os_window) {
    write_log("No OS Window yet!");
  }
}

void WindowWin64Linux::OnUpdate() { GLFW_UpdateWindow(s_os_window); }

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
