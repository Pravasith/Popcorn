#pragma once

#include "Global_Macros.h"
#include <cstdint>
#include <string>

ENGINE_NAMESPACE_BEGIN
class Window {
  /* Multiple window creation not supported at the moment */
public:
  struct Props {
    std::string Title;
    uint16_t W;
    uint16_t H;

    Props(const std::string &title = "Popcorn Engine", uint16_t w = 1600,
          uint16_t h = 900)
        : Title(title), W(w), H(h){};
  };

  static void Create(const Props &props = Props());
  static void Destroy();

  virtual void OnUpdate() = 0;
  virtual uint16_t GetWidth() const = 0;
  virtual uint16_t GetHeight() const = 0;
  virtual void *GetOSWindow() const = 0;

protected:
  Window();
  virtual ~Window();
  /* = default; */
};
ENGINE_NAMESPACE_END
