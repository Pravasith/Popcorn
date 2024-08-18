#pragma once

#include "Global_Macros.h"
#include "LayerStack.h"
#include "Popcorn/Events/Event.h"
#include "Popcorn/Events/Subscriber.h"
#include "Window.h"
#include "WindowEvent.h"

ENGINE_NAMESPACE_BEGIN
class Application : public Subscriber {
public:
  static void Start();
  static Application &Get();
  static void Run();
  static void Stop();

  void OnEvent(Event &) const override;
  bool OnWindowResize(WindowResizeEvent &) const;
  bool OnWindowClose(WindowCloseEvent &) const;

  Window &GetAppWindow() const;

  LayerStack &GetLayerStack() const { return *s_layer_stack; };
  bool IsGameLoopRunning() const { return s_is_game_loop_running; };

private:
  Application();
  ~Application();

private:
  static bool s_is_window_minimized;
  static bool s_is_game_loop_running;

  static Application *s_instance;
  static LayerStack *s_layer_stack;
  static Window *s_window;
};
ENGINE_NAMESPACE_END
