#include <Application.h>
#include <Popcorn.h>
#include <Popcorn/Graphics/Renderer.h>
#include <Popcorn/Graphics/VertexBuffer.h>
#include <glm/glm.hpp>
#include <iostream>

using namespace Popcorn;

class GameLayer : public Layer {
public:
  GameLayer() {
    PC_PRINT("CREATED", TagType::Constr, "GAME-LAYER")

  };
  ~GameLayer() { PC_PRINT("DESTROYED", TagType::Destr, "GAME-LAYER") };

  virtual void OnAttach() override {
    // struct Vertex {
    //   glm::vec2 pos;
    //   glm::vec3 color;
    //   std::string Print() {
    //     std::stringstream ss;
    //     ss << pos.x << ", " << pos.y << "; " << color.r << ", " << color.g
    //        << ", " << color.b;
    //
    //     return ss.str();
    //   };
    // };

    // Gfx::VertexBuffer bfr;
    // bfr.Fill<Vertex>({
    //     //
    //     {{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
    //     {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    //     {{0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    //     //
    // });
    // bfr.PrintBuffer<Vertex>();
  };
  virtual void OnDetach() override {};
  virtual void OnUpdate() override { Renderer::Get().DrawFrame(); };
  virtual void OnEvent(Event &e) override {};
};

int main(int argc, char **argv) {
  // START POPCORN ENGINE
  Popcorn::Application::Start();
  auto &app = Application::Get();

  // CREATE AND SET RENDERER
  auto &renderer =
      *(Renderer::Create<RendererType::Vulkan>(app.GetAppWindow()));
  app.SetRenderer(renderer);

  // CREATE A GAME LAYER
  auto gameLayer = new GameLayer();
  Application::AddLayer(gameLayer);

  // GAME LOOP
  Popcorn::Application::StartGameLoop();

  // FREE RESOURCES
  Popcorn::Application::Stop();

  return 0;
}
