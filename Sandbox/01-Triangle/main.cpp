#include <Popcorn.h>
#include <Popcorn/Graphics/VertexBuffer.h>
#include <glm/glm.hpp>
#include <iostream>

using namespace Popcorn;

class GameLayer : public Layer {
public:
  // struct Vertex {
  //   glm::vec2 pos;
  //   glm::vec3 color;
  // };

  // VertexBufferVk<ElementType::Float2, ElementType::Float3> bfr{
  //     {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
  //     {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
  //     {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  // const std::vector<Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
  //                                    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
  //                                    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  // VertexBuffer<Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
  //                               {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
  //                               {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  GameLayer() {
    PC_PRINT("CREATED", TagType::Constr, "GAME-LAYER")

    struct Vertex {
      glm::vec2 pos;
      glm::vec3 color;
      std::string Print() {
        std::stringstream ss;
        ss << pos.x << ", " << pos.y << "; " << color.r << ", " << color.g
           << ", " << color.b;

        return ss.str();
      };
    };

    Gfx::VertexBuffer bfr;
    bfr.Fill<Vertex>({{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}});
    bfr.PrintBuffer<Vertex>();
  };
  ~GameLayer() { PC_PRINT("DESTROYED", TagType::Destr, "GAME-LAYER") };

  virtual void OnAttach() override {};
  virtual void OnDetach() override {};
  virtual void OnUpdate() override {};
  virtual void OnEvent(Event &e) override {};
};

int main(int argc, char **argv) {
  // INIT POPCORN ENGINE
  Popcorn::Application::Init();

  // CREATE A GAME LAYER
  auto gameLayer = new GameLayer();
  Application::AddLayer(gameLayer);

  // GAME LOOP
  Popcorn::Application::StartGameLoop();

  // FREE RESOURCES
  Popcorn::Application::Stop();

  return 0;
}
