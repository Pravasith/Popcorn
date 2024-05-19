#pragma once

#include "GLFW_Funcs.h"
#include "Global_Macros.h"
#include "Window.h"
#include <cstdint>

ENGINE_NAMESPACE_BEGIN
class WindowWin64Linux : public Window {
public:
  [[nodiscard]] static Window *Init(const Props &props);
  static void Terminate();

  uint16_t GetWidth() const override;
  uint16_t GetHeight() const override;
  [[nodiscard]] void *GetOSWindow() const override;

  void StartWindowLoop();

private:
  WindowWin64Linux(const Props &props);
  ~WindowWin64Linux() override;

private:
  std::string m_title;

  static GLFW_Types::GLFW_OSWindow_T *s_os_window;
  static WindowWin64Linux *s_instance;
};
ENGINE_NAMESPACE_END
