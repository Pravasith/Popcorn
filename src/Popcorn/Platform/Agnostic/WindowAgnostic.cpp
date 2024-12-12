
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Base.h"
#include "Global_Macros.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "Utilities.h"
#include "WindowAgnostic.h"
#include "WindowEvent.h"
#include <string>

ENGINE_NAMESPACE_BEGIN

GLFWwindow *WindowAgnostic::s_osWindow = nullptr;
WindowAgnostic *WindowAgnostic::s_instance = nullptr;

Window *WindowAgnostic::Init(const Props &props) {
  if (!s_instance) {
    s_instance = new WindowAgnostic(props);
  }

  return s_instance;
}

WindowAgnostic::WindowAgnostic(const Props &props) : m_title(props.Title) {
  PC_PRINT("CREATED", TagType::Constr, "WINDOW-AGNOSTIC");

  // GLFW INIT
  // ---------------------------------------------------
  int success = glfwInit();
  if (!success) {
    pc_write_log("GLFW Error: Couldn't initiate GLFW");
  };

  glfwSetErrorCallback([](int error, const char *desc) {
    pc_write_log(desc);
    throw("GLFW error, see common_logs file");
  });

  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  PC_PRINT("INITED", TagType::Constr, "GLFW");

  // CREATE GLFW WINDOW AND MAKE CONTEXT CURRENT
  // ---------------------------------------------------
  s_osWindow = glfwCreateWindow(props.W, props.H, m_title.c_str(), NULL, NULL);

  if (!s_osWindow) {
    pc_write_log("Failed to create GLFW Window");
    glfwTerminate();
  }

  // glfwMakeContextCurrent(s_osWindow);
  // glfwSwapInterval(1); // Enable vsync

  // int status = gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));

  // GLFW SET WINDOW CALLBACKS
  // ---------------------------------------------------

  /* WINDOW CLOSE CALLBACK */
  glfwSetWindowCloseCallback(s_osWindow, [](GLFWwindow *os_window) {
    WindowCloseEvent winCloseEvt;
    s_instance->PublishEvent(winCloseEvt);
  });

  /* MOUSE POSITION CALLBACK */
  glfwSetCursorPosCallback(s_osWindow,
                           [](GLFWwindow *os_window, double x, double y) {
                             MouseMovedEvent mMoveEvt((float)x, (float)y);
                             s_instance->PublishEvent(mMoveEvt);
                           });

  /* KEY CALLBACK */
  glfwSetKeyCallback(s_osWindow, [](GLFWwindow *os_window, int key,
                                    int scan_code, int action, int mods) {
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
  });

  /* WINDOW RESIZE CALLBACK */
  glfwSetWindowSizeCallback(s_osWindow,
                            [](GLFWwindow *os_window, int width, int height) {
                              WindowResizeEvent event(width, height);
                              s_instance->PublishEvent(event);
                            });

  /* FRAME BFR RESIZE CALLBACK */
  glfwSetFramebufferSizeCallback(
      s_osWindow, [](GLFWwindow *os_window, int width, int height) {
        FrameBfrResizeEvent event(width, height);
        s_instance->PublishEvent(event);
      });

  if (!s_osWindow) {
    pc_write_log("No OS Window yet!");
  }
}

void WindowAgnostic::OnUpdate() {
  // glViewport(0, 0, WindowAgnostic::GetWidth(),
  // WindowAgnostic::GetHeight());

  // glfwSwapBuffers(s_osWindow);
  glfwPollEvents();
}

WindowAgnostic::~WindowAgnostic() {
  glfwDestroyWindow(s_osWindow);
  glfwTerminate();

  PC_PRINT("TERMINATED", TagType::Destr, "GLFW");
  PC_PRINT("DESTROYED", TagType::Destr, "WINDOW-AGNOSTIC");
};

void WindowAgnostic::Terminate() {
  if (!s_osWindow || !s_instance) {
    pc_write_log("Error: WindowAgnostic Terminate -- s_osWindow or s_instance "
                 "of class not found.");
  }

  else {
    // NO NEED TO DELETE s_osWindow BC glfwTerminate() HANDLES DELETION
    s_osWindow = nullptr;

    delete s_instance;
    s_instance = nullptr;
  }
}

void *WindowAgnostic::GetOSWindow() const {
  if (s_osWindow)
    return s_osWindow;

  return nullptr;
}

ENGINE_NAMESPACE_END
