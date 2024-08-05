#pragma once

#include "Global_Macros.h"
#include "LayerStack.h"
#include "Popcorn/Events/Event.h"
#include "Popcorn/Events/Subscriber.h"

ENGINE_NAMESPACE_BEGIN
class Application : public Subscriber {
public:
  static void Start();
  static Application *Get();
  static void Run();
  static void Stop();

  void OnEvent(Event &) const override;

  LayerStack &GetLayerStack() const { return *m_layer_stack; };
  bool IsGameLoopRunning() const { return s_is_game_loop_running; };

private:
  Application();
  ~Application();

private:
  LayerStack *m_layer_stack;

  static bool s_is_window_minimized;
  static bool s_is_game_loop_running;

  static Application *s_instance;
};
ENGINE_NAMESPACE_END
