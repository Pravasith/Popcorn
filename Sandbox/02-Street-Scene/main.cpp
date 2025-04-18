#include <Base.h>
#include <Material.h>
#include <Popcorn.h>
#include <Popcorn/Core/Application.h>
#include <Popcorn/Graphics/BufferObjects.h>
#include <Popcorn/Graphics/Materials/BasicMaterial.h>
#include <Popcorn/Graphics/Mesh.h>
#include <Popcorn/Graphics/Renderer.h>
#include <Popcorn/Scene/Scene.h>
#include <Sources.h>
#include <TimeEvent.h>
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vector>

using namespace Popcorn;
class BlenderScene : public Scene {};

class GameLayer : public Layer {

public:
  GameLayer() { PC_PRINT("CREATED", TagType::Constr, "GAME-LAYER") };
  ~GameLayer() { PC_PRINT("DESTROYED", TagType::Destr, "GAME-LAYER") };

  virtual void OnAttach() override {
    Context::AddGltfToScene("assets/models/blenderModel.gltf", scene);
    auto &renderer = Context::GetRenderer();
    renderer.AddScene(&scene);
  };

  virtual void OnDetach() override {
    // TODO: Delete scene -- and all it's heap allocated children
    // Client is the sole owner of all GameObject resources

    delete triMat;
    triMat = nullptr;
  };

  virtual void OnUpdate(TimeEvent &e) override {
    mesh->RotateZ(glm::radians(90.f) * e.GetDeltaS());
    triScene.Update();
  };

  virtual void OnRender() override { Renderer::Get().DrawFrame(triScene); };

  virtual bool OnEvent(Event &e) override { return false; };

private:
  Material *triMat;
  BlenderScene scene;
};

int main(int argc, char **argv) {
  Popcorn::Context::BeginContext();

  auto gameLayer = new GameLayer();
  Application::AddLayer(gameLayer); // Adds scene to renderer on attach

  Popcorn::Context::StartGame();
  Popcorn::Context::EndContext();

  return 0;
}
