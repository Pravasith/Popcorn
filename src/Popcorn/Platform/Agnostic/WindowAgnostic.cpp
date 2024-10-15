
#include <glad/glad.h>

#include "GLFW_Funcs.h"
#include "Global_Macros.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "Utilities.h"
#include "WindowAgnostic.h"
#include "WindowEvent.h"
#include <cstdint>
#include <string>

ENGINE_NAMESPACE_BEGIN
using namespace GLFW_Funcs;

GLFW_Types::GLFW_OSWindow_T *WindowAgnostic::s_os_window = nullptr;
WindowAgnostic *WindowAgnostic::s_instance = nullptr;

Window *WindowAgnostic::Init(const Props &props) {
  if (!s_instance) {
    s_instance = new WindowAgnostic(props);
  }

  return s_instance;
}

WindowAgnostic::WindowAgnostic(const Props &props) : m_title(props.Title) {
  PC_PRINT_DEBUG("PLATFORM WINDOW CREATED", 2, "WIN64-LINUX");

  GLFW_Init();
  PC_PRINT_DEBUG("GLFW INIT", 2, "WIN64-LINUX");

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

void WindowAgnostic::OnUpdate() {

  // glViewport(0, 0, WindowAgnostic::GetWidth(),
  // WindowAgnostic::GetHeight());

  GLFW_UpdateWindow(s_os_window);
}

WindowAgnostic::~WindowAgnostic() {
  GLFW_Terminate();

  PC_PRINT_DEBUG("GLFW KILL", 2, "WIN64-LINUX");
  PC_PRINT_DEBUG("PLATFORM WINDOW DESTROYED", 2, "WIN64-LINUX");
};

void WindowAgnostic::Terminate() {
  if (!s_os_window || !s_instance) {
    write_log("Error: WindowAgnostic Terminate -- s_os_window or s_instance "
              "of class not found.");
  }

  else {
    // No need to delete bc GLFW_Terminate() handles deletion
    s_os_window = nullptr;

    delete s_instance;
    s_instance = nullptr;
  }
}

uint16_t WindowAgnostic::GetWidth() const { return 1; }

uint16_t WindowAgnostic::GetHeight() const { return 1; }

void *WindowAgnostic::GetOSWindow() const {
  if (s_os_window)
    return s_os_window;

  return nullptr;
}

ENGINE_NAMESPACE_END
