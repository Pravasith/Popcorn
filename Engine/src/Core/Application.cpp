#include "Popcorn/Core/Application.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace Popcorn {
Application *Application::s_instance = nullptr;

Application *Application::Get() {
  if (s_instance) {
    return s_instance;
  }
  return new Application();
}
void Application::Destroy() {
  if (s_instance) {
    delete s_instance;
    return;
  }
  std::cout << "Warning: Singleton already destroyed" << '\n';
}

Application::Application() {
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit()) {
  }

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    // return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    // glClear(GL_COLOR_BUFFER_BIT);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  // return 0;
  std::cout << "APPLICATION CREATED" << '\n';
}

Application::~Application() {}
} // namespace Popcorn
