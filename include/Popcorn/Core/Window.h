#pragma once

#include "Global_Macros.h"
#include "Popcorn/Events/Publisher.h"
#include <cstdint>
#include <string>

ENGINE_NAMESPACE_BEGIN
class Window : public Publisher {

  /* ---- NOTE ---- */
  /* Multiple window creation not supported at the moment */
  /* So multiple calls to Window::Create will result in  */
  /* a same window pointer */
  /* ---- END NOTE ---- */

public:
  struct Props {
    std::string Title;
    uint16_t W;
    uint16_t H;

    Props(const std::string &title = "Popcorn Engine", uint16_t w = 1600,
          uint16_t h = 900)
        : Title(title), W(w), H(h){};
  };

  static Window &Create(const Props &props = Props());
  static void AddSubscriber(const Subscriber *);
  static void OnUpdate();
  static void Destroy();

  virtual uint16_t GetWidth() const = 0;
  virtual uint16_t GetHeight() const = 0;
  virtual void *GetOSWindow() const = 0;

protected:
  Window();
  virtual ~Window();
  /* = default; */

private:
  static void *s_platform_window_instance;
  static Window *s_instance;
};
ENGINE_NAMESPACE_END
