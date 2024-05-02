#include "Window_Win64_Linux.h"
#include "GLFW_Funcs.h"
#include "Global_Macros.h"
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>

ENGINE_NAMESPACE_BEGIN

using namespace GLFW_Funcs;

GLFW_OSWindowType *WindowWin64Linux::s_os_window = nullptr;

void WindowWin64Linux::Init(const Props &props) {
  m_title = props.Title;
  std::cout << "Win64 Class " << m_title << " created." << std::endl;

  GLFW_Init();
  GLFW_CreateWindow(&s_os_window, m_title, props.W, props.H);
  GLFW_WindowLoop(s_os_window);
}

void WindowWin64Linux::Terminate() { GLFW_Terminate(); }

void WindowWin64Linux::OnUpdate() {}
uint16_t WindowWin64Linux::GetWidth() const { return 1; }
uint16_t WindowWin64Linux::GetHeight() const { return 1; }
void *WindowWin64Linux::GetOSWindow() const { return nullptr; }

WindowWin64Linux::~WindowWin64Linux() {
  GLFW_Terminate();
  std::cout << "Win64 Class " << m_title << " terminated." << std::endl;
};
ENGINE_NAMESPACE_END
