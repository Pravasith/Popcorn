#pragma once

#include "Global_Macros.h"
#include "ImGuiLayer.h"
#include "LayerStack.h"
#include "Popcorn/Events/Event.h"
#include "Popcorn/Events/Subscriber.h"
#include "Popcorn/Graphics/Renderer.h"
#include "Time.h"
#include "TimeEvent.h"
#include "Window.h"
#include "WindowEvent.h"

ENGINE_NAMESPACE_BEGIN
class Application : public Subscriber {
public:
  // SINGLETON
  static void Start();
  static Application &Get();
  static void Stop();

  static void InitLayers();

  static void Run();

  Window &GetAppWindow() const;
  LayerStack &GetLayerStack() const { return *s_layer_stack; };
  static bool IsGameLoopRunning();

private:
  Application();
  ~Application();

  void OnEvent(Event &) const override;
  bool OnWindowResize(WindowResizeEvent &) const;
  bool OnWindowClose(WindowCloseEvent &) const;
  bool OnCPUClockTick(TimeEvent &) const;

private:
  static Application *s_instance;
  static LayerStack *s_layer_stack;
  static ImGuiLayer *s_imgui_layer;
  static Window *s_window;
  static Renderer *s_renderer;
  static Time *s_time;
};
ENGINE_NAMESPACE_END
