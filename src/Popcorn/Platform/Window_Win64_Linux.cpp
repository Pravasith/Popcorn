#include "Window_Win64_Linux.h"
#include "GLFW_Funcs.h"
#include "Global_Macros.h"
#include "Utilities.h"
#include <cstdint>
#include <iostream>
#include <ostream>

ENGINE_NAMESPACE_BEGIN
using namespace GLFW_Funcs;

GLFW_OSWindowType *WindowWin64Linux::s_os_window = nullptr;
WindowWin64Linux *WindowWin64Linux::s_instance = nullptr;

Window *WindowWin64Linux::Init(const Props &props) {
  if (!s_instance) {
    s_instance = new WindowWin64Linux(props);
  }

  return s_instance;
}

WindowWin64Linux::WindowWin64Linux(const Props &props) : m_title(props.Title) {
  std::cout << "  -- Win64::Class: " << m_title << " created.\n";

  GLFW_Init();
  std::cout << "    -- GLFW init.\n";
  GLFW_CreateWindow(&s_os_window, m_title, props.W, props.H);

  if (!s_os_window) {
    write_log("No OS Window yet!");
  } else {
    StartWindowLoop();
  }
}

void WindowWin64Linux::StartWindowLoop() { GLFW_WindowLoop(s_os_window); }

WindowWin64Linux::~WindowWin64Linux() {
  GLFW_Terminate();

  std::cout << "    -- GLFW killed.\n";
  std::cout << "  -- Win64::Class: " << m_title << " terminated.\n";
};

void WindowWin64Linux::Terminate() {
  if (!s_os_window || !s_instance) {
    write_log("Error: WindowWin64Linux Terminate -- s_os_window or s_instance "
              "of class not found.");
  }

  else {
    // No need to delete bc GLFW_Terminate() handles deletion
    s_os_window = nullptr;

    delete s_instance;
    s_instance = nullptr;
  }
}

void WindowWin64Linux::OnUpdate() {}
uint16_t WindowWin64Linux::GetWidth() const { return 1; }
uint16_t WindowWin64Linux::GetHeight() const { return 1; }
void *WindowWin64Linux::GetOSWindow() const { return nullptr; }

ENGINE_NAMESPACE_END
