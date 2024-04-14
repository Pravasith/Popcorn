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

  static void Init();
  static void CreateWindow();

  static void InitGraphicsLib();
  static void SetGraphicsViewport();

  static void StartMainGameLoop();

  static void TerminateWindow();

  static Window *instance;
  static const char *__dir;
  static GLFWwindow *__glfw_window;
};
