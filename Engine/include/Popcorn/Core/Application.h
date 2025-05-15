#pragma once

#include "Assert.h"
#include "Event.h"
#include "GlobalMacros.h"
#include "LayerStack.h"
#include "Renderer.h"
#include "Subscriber.h"
#include "Time.h"
#include "TimeEvent.h"
#include "Window.h"
#include "WindowEvent.h"

ENGINE_NAMESPACE_BEGIN
using namespace Gfx;

class DebugUIOverlay;

// SINGLETON
class Application : public Subscriber {

public:
  static void Start();
  static Application &Get();
  static void Stop();

  static void StartGameLoop();
  static void CreateWorkflowResources();
  static void AddLayer(Layer *);

  Window &GetAppWindow() const;
  LayerStack &GetLayerStack() const { return *s_layerStack; };
  static bool IsGameLoopRunning();

  inline void SetRenderer(Renderer &renderer) {
    PC_ASSERT(!s_renderer, "A renderer already exists!");
    s_renderer = &renderer;
  };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  Application(const Application &) = delete;
  Application &operator=(const Application &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  Application(Application &&) = delete;
  Application &operator=(Application &&) = delete;

private:
  Application();
  virtual ~Application() override;

  virtual void OnEvent(Event &) override;
  virtual bool OnUpdate(TimeEvent &) override;

  bool OnClockTick(TimeEvent &);

  bool OnWindowResize(WindowResizeEvent &) const;
  bool OnWindowClose(WindowCloseEvent &) const;

private:
  static Application *s_instance;
  static LayerStack *s_layerStack;
  static Window *s_window;
  static Time *s_time;
  static Renderer *s_renderer;
  static DebugUIOverlay *s_debugUIOverlay;
};

ENGINE_NAMESPACE_END
