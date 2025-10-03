#include "SplineUtils.h"
#include <Animation.h>
#include <BufferObjects.h>
#include <Camera.h>
#include <CurveDefs.h>
#include <Curves.h>
#include <GameObject.h>
#include <Helpers.h>
#include <Material.h>
#include <Mesh.h>
#include <Popcorn.h>
#include <Popcorn/Core/Base.h>
#include <Renderer.h>
#include <Scene.h>
#include <SceneDefs.h>
#include <Sources.h>
#include <SplineDefs.h>
#include <Splines.h>
#include <Time.h>
#include <TimeEvent.h>
#include <functional>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace Popcorn;
class BlenderScene : public Scene {}; // Game Objects are owned by Scene
class GameLayer : public Layer {
public:
  GameLayer() = default;

  virtual void OnAttach() override {
    Popcorn::Context::ConvertGltfToScene("../assets/models/test-scene.gltf",
                                         scene);
    Popcorn::Context::RegisterScene(scene);
  }

  virtual void OnDetach() override {
    // Popcorn::Context::DisposeScene(scene);
  }

  virtual void OnUpdate(TimeEvent &e) override {}

  virtual void OnRender() override {
    // Draws all scenes
    Popcorn::Context::RenderScenes(scene);
  }
  virtual bool OnEvent(Event &e) override { return false; }

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
