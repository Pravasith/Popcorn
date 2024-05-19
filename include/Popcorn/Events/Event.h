#pragma once

#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN
enum class Event_T {
  None = 0,

  // Window Events
  WindowClose,
  WindowResize,

  // Application Events
  AppTick,
  AppUpdate,
  AppRender,

  // Keyboard Events
  KeyPressed,
  KeyReleased,
  KeyTyped,

  // Mouse Events
  MouseButtonPressed,
  MouseButtonReleased,
  MouseMoved,
  MouseScrolled
};

class Event {};
ENGINE_NAMESPACE_END
