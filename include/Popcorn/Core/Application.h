#pragma once

#include "Global_Macros.h"
#include "Popcorn/Events/Subscriber.h"

ENGINE_NAMESPACE_BEGIN
class Application : public Subscriber {
public:
  static void Start();
  static Application *Get();
  static void Stop();
  void OnEvent(const std::string &s) const override;

private:
  Application();
  ~Application();

  static Application *s_instance;
};
ENGINE_NAMESPACE_END
