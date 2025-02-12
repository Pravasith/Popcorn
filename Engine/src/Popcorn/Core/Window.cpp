#include "Window.h"
/* #ifdef IS_WINDOWS_OR_LINUX */
#include "Base.h"
#include "WindowAgnostic.h"
/* #else */
/* write_log("Error Importing Platform!"); */
/* #endif */

// SAME -- IF OPEN_GL ...

ENGINE_NAMESPACE_BEGIN

void *Window::s_osWindow = nullptr;

Window::Window() : Publisher("Window") {
  PC_PRINT("CREATED", TagType::Constr, "WINDOW");
};
Window::~Window() { PC_PRINT("DESTROYED", TagType::Destr, "WINDOW"); };

Window *Window::Create(const Props &props) {
  if (s_osWindow) {
    return static_cast<Window *>(s_osWindow);
  };

  /* ---- NOTE ---- */
  /* Multiple window creation not supported at the moment */
  /* So multiple calls to Window::Create will result in  */
  /* same window pointer */
  /* ---- END NOTE ---- */

  /* #ifdef IS_WINDOWS_OR_LINUX */
  s_osWindow = WindowAgnostic::Init(props);
  /* #else */
  /* write_log("Error Creating Window - Wrong platform!"); */
  /* #endif */

  return static_cast<Window *>(s_osWindow);
};

// void Window::AddSubscriber(const Subscriber *s) {
//
//   /* #ifdef IS_WINDOWS_OR_LINUX */
//   (static_cast<WindowAgnostic
//   *>(s_osWindow))->Subscribe(s);
//   /* #else */
//   /* write_log("Error Creating Window - Wrong platform!"); */
//   /* #endif */
// }

void Window::Destroy() {
  /* #ifdef IS_WINDOWS_OR_LINUX */
  WindowAgnostic::Terminate();
  s_osWindow = nullptr;

  /* #else */
  /* write_log("Error Destroying Window - Wrong platform!"); */
  /* #endif */
};

void Window::OnUpdate(TimeEvent &) {
  /* #ifdef IS_WINDOWS_OR_LINUX */
  (static_cast<WindowAgnostic *>(s_osWindow))->OnUpdate();
  /* #else */
  /* write_log("Error Creating Window - Wrong platform!"); */
  /* #endif */
};

ENGINE_NAMESPACE_END
