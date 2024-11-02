#pragma once

#include "DebugUIOverlay.h"
#include "Event.h"
#include "Global_Macros.h"
#include "LayerStack.h"
#include "RenderLayer.h"
#include "Subscriber.h"
#include "Time.h"
#include "TimeEvent.h"
#include "Window.h"
#include "WindowEvent.h"

ENGINE_NAMESPACE_BEGIN

// SINGLETON
class Application : public Subscriber {
public:
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
  static Window *s_window;
  static Time *s_time;

  static DebugUIOverlay *s_debug_ui_overlay;
  static RenderLayer *s_render_layer;
};
ENGINE_NAMESPACE_END
