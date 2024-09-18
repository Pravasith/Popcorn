#include "Renderer.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN

// SINGLETON
Renderer *Renderer::s_instance = nullptr;

Renderer::Renderer() {
  PC_PRINT_DEBUG("RENDERER INSTANCE CREATED", 2, "GRAPHICS")

#ifdef PC_DEBUG
  std::cout << "PC_DEBUG\n";
#endif
};

Renderer::~Renderer() {
  PC_PRINT_DEBUG("RENDERER INSTANCE DELETED", 2, "GRAPHICS")
};

void Renderer::Create() {

  if (s_instance) {
    return;
  }

  s_instance = new Renderer();
};

void Renderer::Destroy() {
  delete s_instance;
  s_instance = nullptr;
};

ENGINE_NAMESPACE_END
