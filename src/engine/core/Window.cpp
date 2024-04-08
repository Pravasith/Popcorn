#include "Window.h"
#include <glad/glad.h>
#include <iostream>
// A LINE BREAK FOR FORMATTING
#include <GLFW/glfw3.h>

Window *Window::instance = nullptr;
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void Window::Create(const char *dir) {
  std::cout << dir << '\n';
  if (!instance) {
    instance = new Window();
  }
}

Window *Window::Get() { return instance; }

void Window::Destroy() {
  if (instance) {
    delete instance;
    instance = nullptr;
  }
}

Window::Window() {
  std::cout << "WINDOW CREATED" << "\n";

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
  }

  glViewport(0, 0, 800, 600);

  while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
};

Window::~Window() { std::cout << "WINDOW DESTROYED" << "\n"; };
