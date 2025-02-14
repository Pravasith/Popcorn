#include <Application.h>
#include <Popcorn.h>
#include <Popcorn/Graphics/Renderer.h>
#include <Popcorn/Graphics/VertexBuffer.h>
#include <Popcorn/Scene/Scene.h>
#include <glm/glm.hpp>

using namespace Popcorn;

class GameLayer : public Layer {
public:
  GameLayer() { PC_PRINT("CREATED", TagType::Constr, "GAME-LAYER") };
  ~GameLayer() { PC_PRINT("DESTROYED", TagType::Destr, "GAME-LAYER") };

  virtual void OnAttach() override {

    class TriangleScene : public Scene {};
    // class S2 : public Scene {};

    TriangleScene triangleScene;

    // triangleMesh();

    // Mesh

    // triangleScene.Add(GameObject *node);
  };

  virtual void OnDetach() override {};
  virtual void OnUpdate() override {
    //
    // Renderer::Get().DrawFrame();
  };
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
