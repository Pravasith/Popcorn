#pragma once

#include "GlobalMacros.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include <cstdint>

ENGINE_NAMESPACE_BEGIN
class WindowAgnostic : public Window {
public:
  [[nodiscard]] static Window *Init(const Props &props);
  static void Terminate();

  [[nodiscard]] void *GetOSWindow() const override;

  virtual void OnUpdate();

  [[nodiscard]] inline const std::pair<uint32_t, uint32_t>
  GetFramebufferSize() const override {
    if (GetOSWindow() == nullptr) {
      throw std::runtime_error("OS WINDOW IS NULL");
    };

    int width, height;
    glfwGetFramebufferSize(static_cast<GLFWwindow *>(GetOSWindow()), &width,
                           &height);
    return std::make_pair(width, height);
  };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  WindowAgnostic(const WindowAgnostic &) = delete;
  WindowAgnostic &operator=(const WindowAgnostic &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  WindowAgnostic(WindowAgnostic &&) = delete;
  WindowAgnostic &operator=(WindowAgnostic &&) = delete;

private:
  WindowAgnostic(const Props &props);
  ~WindowAgnostic() override;

private:
  std::string m_title;

  static GLFWwindow *s_osWindow;
  static WindowAgnostic *s_instance;
};
ENGINE_NAMESPACE_END
