#pragma once

#include "GLFW_Funcs.h"
#include "Global_Macros.h"
#include "Window.h"
#include <cstdint>

ENGINE_NAMESPACE_BEGIN
class WindowAgnostic : public Window {
public:
  [[nodiscard]] static Window *Init(const Props &props);
  static void Terminate();

  uint16_t GetWidth() const override;
  uint16_t GetHeight() const override;
  [[nodiscard]] void *GetOSWindow() const override;

  virtual void OnUpdate();

private:
  WindowAgnostic(const Props &props);
  ~WindowAgnostic() override;

private:
  std::string m_title;

  static GLFW_Types::GLFW_OSWindow_T *s_os_window;
  static WindowAgnostic *s_instance;
};
ENGINE_NAMESPACE_END
