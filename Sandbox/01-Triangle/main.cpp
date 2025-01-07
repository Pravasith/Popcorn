#include <Popcorn.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <tuple>

using namespace Popcorn;

class GameLayer : public Layer {
public:
  struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
  };

  std::tuple<glm::vec2, glm::vec3> tup;

  // const std::vector<Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
  //                                    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
  //                                    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  // const std::vector<Buffer<Float2, Float3>> vertices = {
  //     {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
  //     {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
  //     {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  GameLayer() { PC_PRINT("CREATED", TagType::Constr, "GAME-LAYER") };
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
