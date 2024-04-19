#pragma once

#include "Global_Macros.h"
#include <glad/glad.h>
// A LINE BREAK FOR FORMATTING
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN

class Window {
public:
  static void Create(const char *windowName);
  // Default constructor
  static void Create() { Window::Create("Game Window"); };
  static Window *Get();
  static void Destroy();

private:
  Window();
  ~Window();

  void Init();
  void CreateOSWindow();
  void InitGraphicsLib();
  void SetGraphicsViewport();
  void StartMainGameLoop();
  void TerminateWindow();

  GLFWwindow *m_glfw_window;

private:
  static Window *s_instance;
  static const char *s_window_name;
};

ENGINE_NAMESPACE_END
