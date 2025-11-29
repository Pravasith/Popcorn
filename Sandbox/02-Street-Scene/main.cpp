#include "SplineUtils.h"
#include <Animation.h>
#include <AnimationDefs.h>
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
#include <cmath>
#include <functional>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>

using namespace Popcorn;
class BlenderScene : public Scene {}; // Game Objects are owned by Scene
class GameLayer : public Layer {
public:
  GameLayer() = default;

  virtual void OnAttach() override {
    Popcorn::Context::ConvertGltfToScene("../assets/models/test-scene.gltf",
                                         scene);

    Popcorn::Context::RegisterScene(scene);

    const Spline<glm::vec3> *camSpline =
        PC_MakeBezierSplineFromBlenderJson("../assets/curves/rail.json");

    camera = static_cast<Camera *>(scene.FindObjectByName("Camera"));
    blu = scene.FindObjectByName("blue");
    rotorA = scene.FindObjectByName("rotor-a");
    rotorB = scene.FindObjectByName("rotor-b");
    rotorC = scene.FindObjectByName("rotor-c");
    rotorD = scene.FindObjectByName("rotor-d");
    eva = scene.FindObjectByName("Cube.033");

    TimeTrain tt(camera->GetAnimationProperty_Pos(), camSpline, 0.0, 1.0);

    AnimationTrack cameraAnimTrack;
    cameraAnimTrack.Insert_Slow(tt);

    scene.AddAnimationTrack(std::move(cameraAnimTrack));
    camera->ActivateLookAtTarget(true);

    // scene.GetAnimationTrack(0).Play(20);
  }

  virtual void OnDetach() override {
    // Popcorn::Context::DisposeScene(scene);
  }

  virtual void OnUpdate(TimeEvent &e) override {
    // camera->SetLookAtDirection(glm::normalize(-camera->GetPosition()));
    // PC_WARN(rotor->GetPosition().x << ' ' << rotor->GetPosition().y << ' '
    //                                << rotor->GetPosition().z)
    //
    // PC_WARN(sin(glm::radians(20.0) * e.GetDeltaS()))

    float fullRotation = glm::radians(10.0) * e.GetDeltaS();
    float angle = glm::radians(10.0) * sin(e.GetElapsedS()) * 0.2;

    blu->RotateLocalEuler<Axes::Y>(angle * .1);

    rotorA->RotateLocalEuler<Axes::Z>(fullRotation);
    eva->RotateLocalEuler<Axes::Y>(fullRotation);

    rotorB->RotateLocalEuler<Axes::Z>(angle);

    rotorC->RotateLocalEuler<Axes::X>(angle);

    rotorD->RotateLocalEuler<Axes::Y>(angle);
  }

  virtual void OnRender() override {
    // Draws all scenes
    Popcorn::Context::RenderScenes(scene);
  }
  virtual bool OnEvent(Event &e) override { return false; }

private:
  BlenderScene scene;
  Camera *camera;

  GameObject *rotorA;
  GameObject *rotorB;
  GameObject *rotorC;
  GameObject *rotorD;

  GameObject *blu;
  GameObject *eva;
};

int main(int argc, char **argv) {
  Popcorn::Context::BeginContext();

  auto gameLayer = new GameLayer();

  Popcorn::Context::AddLayer(gameLayer);
  Popcorn::Context::StartGame();

  Popcorn::Context::EndContext();

  return 0;
}
