#pragma once

#include "Assert.h"
#include "GlobalMacros.h"
#include "LayerStack.h"
#include "Popcorn/Events/Event.h"
#include "Popcorn/Events/Subscriber.h"
#include "Popcorn/Events/TimeEvent.h"
#include "Popcorn/Events/WindowEvent.h"
#include "Popcorn/Graphics/Renderer.h"
#include "Time.h"
#include "Window.h"

ENGINE_NAMESPACE_BEGIN
using namespace Gfx;

// SINGLETON
class Application : public Subscriber {

public:
  static void Start();
  static Application &Get();
  static void Stop();

  static void StartGameLoop();
  static void AddLayer(Layer *);

  Window &GetAppWindow() const;
  LayerStack &GetLayerStack() const { return *s_layerStack; };
  static bool IsGameLoopRunning();

  inline void SetRenderer(Renderer &renderer)
  //
  {
    PC_ASSERT(!s_Renderer, "A renderer already exists!");
    s_Renderer = &renderer;
  };

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
  bool OnWindowClose(WindowCloseEvent &) const;
  bool OnCPUClockTick(TimeEvent &) const;

private:
  static Application *s_instance;
  static LayerStack *s_layerStack;
  static Window *s_window;
  static Time *s_time;
  static Renderer *s_Renderer;
};
ENGINE_NAMESPACE_END
