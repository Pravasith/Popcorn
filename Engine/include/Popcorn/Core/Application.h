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
  LayerStack &GetLayerStack() const { return *s_layerStack; };
  static bool IsGameLoopRunning();

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  Application(const Application &) = delete;
  Application &operator=(const Application &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  Application(Application &&) = delete;
  Application &operator=(Application &&) = delete;

private:
  Application();
  ~Application();

  void OnEvent(Event &) const override;
  bool OnWindowResize(WindowResizeEvent &) const;
  bool OnFrameBfrResize(FrameBfrResizeEvent &) const;
  bool OnWindowClose(WindowCloseEvent &) const;
  bool OnCPUClockTick(TimeEvent &) const;

private:
  static Application *s_instance;
  static LayerStack *s_layerStack;
  static Window *s_window;
  static Time *s_time;

  static DebugUIOverlay *s_debugUIOverlay;
  static RenderLayer *s_renderLayer;
};
ENGINE_NAMESPACE_END
