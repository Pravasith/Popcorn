#include "Window_Win64_Linux.h"
#include "Global_Macros.h"
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>

ENGINE_NAMESPACE_BEGIN

using namespace GLFW_Funcs;

GLFW_OSWindowType *WindowWin64Linux::s_os_window = nullptr;
std::string m_title = "";

void WindowWin64Linux::Init(const Props &props) {
  m_title = props.Title;
  std::cout << "Win64 Class " << m_title << " created." << std::endl;

  /* --- Inits GLFW and sets an error callback Fn --- */
  GLFW_Init();

  GLFW_CreateWindow(&s_os_window, m_title, props.W, props.H);

  while (!glfwWindowShouldClose(s_os_window)) {
    // Keep running
  }

  /* --- Terminates GLFW --- */
  GLFW_Terminate();
}

void WindowWin64Linux::Terminate() {
  std::cout << "Win64 Class " << m_title << " terminated." << std::endl;
}

void WindowWin64Linux::OnUpdate() {}
uint16_t WindowWin64Linux::GetWidth() const { return 1; }
uint16_t WindowWin64Linux::GetHeight() const { return 1; }
void *WindowWin64Linux::GetOSWindow() const { return nullptr; }

WindowWin64Linux::~WindowWin64Linux(){};
ENGINE_NAMESPACE_END
