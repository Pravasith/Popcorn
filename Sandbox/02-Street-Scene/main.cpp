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
    auto *curveFactory = Popcorn::Context::GetCurveFactory();

    const Spline<glm::vec3> *camSpline =
        PC_MakeBezierSplineFromBlenderJson("../assets/curves/rail.json");

    Camera *camera = static_cast<Camera *>(scene.FindObjectByName("Camera"));
    GameObject *yellowCube = scene.FindObjectByName("yellow-cube");
    GameObject *redCube = scene.FindObjectByName("red-cube");

    TimeTrain camTrain(
        camera->GetAnimationProperty_Pos(), // any animatable prop
        camSpline,                          // timeRail(spline or curve)
        0.0,                                // start t
        1.0                                 // end t
    );

    // Animate cam pos(Bezier spline)
    AnimationTrack cameraSwoosh;
    cameraSwoosh.Insert_Slow(camTrain);

    CurveInfoLinearForm<glm::vec3> lookAtRedCInfo;
    lookAtRedCInfo.p0 = glm::vec3{0.f}; // world origin
    lookAtRedCInfo.p1 = redCube->GetPosition();

    CurveInfoLinearForm<glm::vec3> lookAtYellowCInfo;
    lookAtYellowCInfo.p0 = redCube->GetPosition();
    lookAtYellowCInfo.p1 = yellowCube->GetPosition();

    // Animate lookAt dir(curve)
    const LinearCurve<glm::vec3> *redRail =
        curveFactory->GetCurvePtr(lookAtRedCInfo);
    const LinearCurve<glm::vec3> *yellowRail =
        curveFactory->GetCurvePtr(lookAtYellowCInfo);
    TimeTrain redTimeTrain(camera->GetAnimationProperty_LookAtTarget(), redRail,
                           .0, .2);
    TimeTrain yellowTimeTrain(camera->GetAnimationProperty_LookAtTarget(),
                              yellowRail, .6, .8);
    cameraSwoosh.Insert_Slow(redTimeTrain);
    cameraSwoosh.Insert_Slow(yellowTimeTrain);

    // Make camera lookAt a target
    camera->ActivateLookAtTarget(true);

    scene.AddAnimationTrack(std::move(cameraSwoosh));
    scene.GetAnimationTrack(0).Play(35);
    scene.GetAnimationTrack(1).Play(35);
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
  GameObject *building = nullptr;
};

int main(int argc, char **argv) {
  Popcorn::Context::BeginContext();

  auto gameLayer = new GameLayer();

  Popcorn::Context::AddLayer(gameLayer);
  Popcorn::Context::StartGame();

  Popcorn::Context::EndContext();

  return 0;
}
