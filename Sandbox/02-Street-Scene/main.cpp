#include <Animation.h>
#include <BufferObjects.h>
#include <Camera.h>
#include <GameObject.h>
#include <Helpers.h>
#include <Material.h>
#include <Mesh.h>
#include <Popcorn.h>
#include <Popcorn/Core/Base.h>
#include <Renderer.h>
#include <Scene.h>
#include <Sources.h>
#include <SplineDefs.h>
#include <Splines.h>
#include <Time.h>
#include <TimeEvent.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

using namespace Popcorn;
class BlenderScene : public Scene {}; // Game Objects are owned by Scene
class GameLayer : public Layer {
public:
  GameLayer() = default;

  virtual void OnAttach() override {
    // Popcorn::Context::ConvertGltfToScene("../assets/models/light2.gltf",
    // scene);
    // Popcorn::Context::ConvertGltfToScene("../assets/models/planet-scene.gltf",
    //                                      scene);

    Popcorn::Context::ConvertGltfToScene(
        "../assets/models/planet-scene-new.gltf", scene);

    // Popcorn::Context::ConvertGltfToScene("../assets/models/light-test.gltf",
    //                                      scene);
    Popcorn::Context::RegisterScene(scene);
    // building = scene.FindObjectByName("building");

    AnimationTrack &animTrack1 = scene.GetAnimationTrack(0);
    AnimationTrack &animTrack2 = scene.GetAnimationTrack(1);
    AnimationTrack &animTrack3 = scene.GetAnimationTrack(2);

    float x = 1.f;

    const std::vector<LinearKnot<glm::vec3>> knots{
        {{-x, -x, x}, 0},   {{x, -x, x}, 0.25}, {{x, x, x}, 0.50},
        {{-x, x, x}, 0.75}, {{-x, -x, x}, 1.0},
    };

    auto *splineFactory = Popcorn::Context::GetSplineFactory();
    auto *curveFactory = Popcorn::Context::GetCurveFactory();

    const Spline<glm::vec3> *cmr_Spl =
        splineFactory->MakeAutomaticSpline(knots);

    // GameObject *cylinder =
    //     static_cast<Camera *>(scene.FindObjectByName("Camera"));

    GameObject *cylinder = scene.FindObjectByName("Cylinder");

    if (cylinder) {
      TimeTrain tt(cylinder->GetAnimationProperty_Pos(), cmr_Spl, 0.0, 1.0);

      AnimationTrack catmullRom;
      catmullRom.Insert_Slow(tt);
      scene.AddAnimationTrack(std::move(catmullRom));
      auto &animTrack0 = scene.GetAnimationTrack(3);

      animTrack0.Play(7.5, [&](AnimationTrack *) {
        PC_PRINT("ANIMATION 0 FINISHED YAYY!", TagType::Print, "")
      });
    }

    animTrack1.Play(2.5, [&](AnimationTrack *) {
      PC_PRINT("ANIMATION 1 FINISHED YAYY!", TagType::Print, "")

      animTrack2.Play(2.5, [&](AnimationTrack *) {
        PC_PRINT("ANIMATION 2 FINISHED YAYY!", TagType::Print, "")

        animTrack3.Play(2.5, [&](AnimationTrack *) {
          PC_PRINT("ANIMATION 3 FINISHED YAYY!", TagType::Print, "")
        });
      });
    });

    // Time::Get()->PrintSubscribers();
  }

  virtual void OnDetach() override {
    // Popcorn::Context::DisposeScene(scene);
  }

  virtual void OnUpdate(TimeEvent &e) override {
    // building->RotateEuler<Axes::Y>(glm::radians(20.f) * e.GetDeltaS());
    //
    // std::cout << building->GetPosition().x << "," <<
    // building->GetPosition().y
    //           << "," << building->GetPosition().z << "\n";
  }

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
