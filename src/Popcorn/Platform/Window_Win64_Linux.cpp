#include "Window_Win64_Linux.h"
#include "GLFW_Funcs.h"
#include "Global_Macros.h"
#include <cstdint>
#include <iostream>
#include <ostream>

ENGINE_NAMESPACE_BEGIN
using namespace GLFW_Funcs;

GLFW_OSWindowType *WindowWin64Linux::s_os_window = nullptr;

Window *WindowWin64Linux::Get(const Props &props) {
  /* m_title = props.Title; */

  if (!s_window_instance) {
    s_window_instance = new WindowWin64Linux(props);
    return s_window_instance;
  }

  return nullptr;
}

WindowWin64Linux::WindowWin64Linux(const Props &props)
    // Delete this
    : m_title(props.Title)
// Delete this
{
  /* m_title = props.Title; */
  std::cout << "Win64 Class " << m_title << " created." << std::endl;

  GLFW_Init();
  GLFW_CreateWindow(&s_os_window, m_title, props.W, props.H);
  GLFW_WindowLoop(s_os_window);

  Terminate();
}

WindowWin64Linux::~WindowWin64Linux() {
  if (s_window_instance) {
    delete s_window_instance;
    s_window_instance = nullptr;
  }

  std::cout << "Win64 Class " << m_title << " terminated." << std::endl;
};

void WindowWin64Linux::Terminate() {
  GLFW_Terminate();
  std::cout << "Win64 Class GLFW terminated." << std::endl;
}

void WindowWin64Linux::OnUpdate() {}
uint16_t WindowWin64Linux::GetWidth() const { return 1; }
uint16_t WindowWin64Linux::GetHeight() const { return 1; }
void *WindowWin64Linux::GetOSWindow() const { return nullptr; }

ENGINE_NAMESPACE_END
