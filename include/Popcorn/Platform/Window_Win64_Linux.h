#pragma once

#include "GLFW_Funcs.h"
#include "Global_Macros.h"
#include "Window.h"
#include <cstdint>

ENGINE_NAMESPACE_BEGIN
class WindowWin64Linux : public Window {
public:
  static Window *Get(const Props &props);
  void Terminate();

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

private:
  std::string m_title;
  static GLFW_Funcs::GLFW_OSWindowType *s_os_window;
};
ENGINE_NAMESPACE_END
