#include <BufferObjects.h>
#include <Material.h>
#include <Mesh.h>
#include <Popcorn.h>
#include <Popcorn/Core/Base.h>
#include <Renderer.h>
#include <Scene.h>
#include <Sources.h>
#include <TimeEvent.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

using namespace Popcorn;
class BlenderScene : public Scene {};

class GameLayer : public Layer {

public:
  GameLayer() { PC_PRINT("CREATED", TagType::Constr, "GAME-LAYER") };
  ~GameLayer() { PC_PRINT("DESTROYED", TagType::Destr, "GAME-LAYER") };

  virtual void OnAttach() override {
    Popcorn::Context::ConvertGltfToScene("assets/models/blenderModel.gltf",
                                         scene);
    Popcorn::Context::RegisterScene(scene);
  };

  virtual void OnDetach() override {
    // TODO: Delete scene -- and all it's heap allocated children
    // Client is the sole owner of all GameObject resources
    Popcorn::Context::DisposeScene(scene);
  };

  virtual void OnUpdate(TimeEvent &e) override {
    // mesh->RotateZ(glm::radians(90.f) * e.GetDeltaS());
    // triScene.Update();
  };

  virtual void OnRender() override {
    // Draws all scenes
    // Popcorn::Context::RenderScenes();
  };

  virtual bool OnEvent(Event &e) override { return false; };

private:
  BlenderScene scene;
};

int main(int argc, char **argv) {
  Popcorn::Context::BeginContext();

  auto gameLayer = new GameLayer();
  Popcorn::Context::AddLayer(gameLayer);
  Popcorn::Context::StartGame();
  Popcorn::Context::EndContext();

  return 0;
}
