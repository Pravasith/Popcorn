#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Events/Publisher.h"
#include <cstdint>
#include <string>
#include <utility>

ENGINE_NAMESPACE_BEGIN
// ABSTRACT CLASS -- HAS NO INVARIANT OR ANY DATA
class Window : public Publisher {

  /* ---- NOTE ---- */
  /* Multiple window creation not supported at the moment */
  /* So multiple calls to Window::Create will result in  */
  /* a same window pointer */
  /* ---- END NOTE ---- */

public:
  struct Props {
    std::string Title;
    uint32_t W;
    uint32_t H;

    Props(const std::string &title = "Popcorn Engine", uint32_t w = 1600,
          uint32_t h = 900)
        : Title(title), W(w), H(h) {};
  };

  static Window *Create(const Props &props = Props());

  // static void AddSubscriber(const Subscriber *);
  static void OnUpdate();
  static void Destroy();

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  Window(Window &&) = delete;
  Window &operator=(Window &&) = delete;

  [[nodiscard("\n\nDON'T FORGET TO USE THE FRAMEBUFFER-"
              "SIZE HOE!!!")]] const virtual std::pair<uint32_t, uint32_t>
  GetFramebufferSize() const = 0;
  [[nodiscard]] virtual void *GetOSWindow() const = 0;

protected:
  Window();
  virtual ~Window();

private:
  static void *s_osWindow;
};
ENGINE_NAMESPACE_END
