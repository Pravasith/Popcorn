#pragma once

#include "GLFW_Funcs.h"
#include "Global_Macros.h"
#include "Popcorn/Events/Publisher.h"
#include "Window.h"
#include <cstdint>

ENGINE_NAMESPACE_BEGIN
class WindowWin64Linux : public Window, public Publisher {
public:
  static Window *Init(const Props &props);
  static void Terminate();

  void OnUpdate() override;
  uint16_t GetWidth() const override;
  uint16_t GetHeight() const override;
  void *GetOSWindow() const override;

  void SetGraphicsAPI(
      // Enum -> OpenGl or Vulkan
      /* #ifdef OPENGL */
      /*   // Set graphics Enum to OpenGL */
      /* #else */
      /*   // Set graphics Enum to other.. etc */
      /* #endif */
  );

  /* void SetGraphicsContext(); */
private:
  WindowWin64Linux(const Props &props);
  ~WindowWin64Linux() override;

  void StartWindowLoop();

private:
  std::string m_title;

  static GLFW_Funcs::GLFW_OSWindowType *s_os_window;
  static WindowWin64Linux *s_instance;
};
ENGINE_NAMESPACE_END
