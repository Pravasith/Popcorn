#pragma once
#include <glad/glad.h>
// A LINE BREAK FOR FORMATTING
#include <GLFW/glfw3.h>

class Window {
public:
  static void Create() { Window::Create("Game Window"); };
  static void Create(const char *dir);

  static Window *Get();

  static void Destroy();

private:
  Window();
  ~Window();

  static void InitGLFW();
  static void CreateGLFWWindow();

  static Window *instance;
  static const char *__dir;
  static GLFWwindow *__glfw_window;
};
