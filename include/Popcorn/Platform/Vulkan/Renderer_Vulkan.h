#pragma once

#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN
class RendererVulkan {
public:
  RendererVulkan();
  ~RendererVulkan();

private:
  void InitVulkan();
  void CleanUp();
  // virtual void OnUpdate();
};

ENGINE_NAMESPACE_END
