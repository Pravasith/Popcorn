#include <BufferObjects.h>
#include <GameObject.h>
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
    // Popcorn::Context::ConvertGltfToScene("../assets/models/light2.gltf",
    // scene);
    Popcorn::Context::ConvertGltfToScene("../assets/models/planet-scene.gltf",
                                         scene);
    // Popcorn::Context::ConvertGltfToScene("../assets/models/light-test.gltf",
    //                                      scene);
    Popcorn::Context::RegisterScene(scene);
    building = scene.FindObjectByName("building");
    // building->Translate(100.f, Axes::Z);
    // building = scene.FindObjectByName("Point.003");
    //

    //
    // --- BEFORE GAME LOOP ---------------------------------------------------
    // --- BEFORE GAME LOOP ---------------------------------------------------
    //
    // --- time rail/track ---
    // TimeRail powerJumpRail {
    //     1000.0f, // duration (ms)
    // };
    //
    // AnimationRoute powerJumpAnimRoute {
    //     1000.0f, // t 'duration' (ms)
    //     BezierCurve{{.1,.2}, {.2,.4}, {.5,.8}, {1.,1.}}, // Or a spline
    // };
    //
    // AnimationRoute::AddStations(0.2f, 0.5f);
    //

    // --- INSIDE GAME LOOP ---------------------------------------------------
    // --- INSIDE GAME LOOP ---------------------------------------------------
    //
    // --- time trains (somewhere in an event handler)
    // OnCharacterJump() {
    //     ...
    //     TimeTrain powerJumpTrain{
    //          powerJumpBegin,
    //          powerJumpMiddle,
    //          powerJumpEnd
    //     };
    //
    //     AnimationProperty characterPos {
    //          character.position,   // obj to animate
    //          vec3(.2f,.2f,.2f),    // start value
    //          vec3(10.f,10.f,12.f), // end value
    //     };
    //
    //     AnimationProperty mainCameraRotY {
    //          camera01.rotation.y,   // obj to animate
    //          vec3(.2f,.2f,.2f),    // start value
    //          vec3(10.f,10.f,12.f), // end value
    //     };
    //
    //     --- Two animation property objs board at the 'powerJumpBegin'
    //     --- station
    //     powerJumpTrain.SchedulePassengers(
    //          {characterPos, 0.1, 0.8}, // (vec3, station)
    //          {mainCameraRotY, 0.7, 0.8} // (float, station)
    //     );
    //
    //     --- An animation property gets off at a station (explicit)
    //     powerJumpTrain.GetOff(mainCameraRotY, powerJumpMiddle);
    //
    //     powerJumpTrain.StartJourney();
    //
    //     --- you can end prematurely too like this:
    //     --- powerJumpTrain.EndJourney(0.5f);
    //     --- all trains end journey after reaching the end station
    // }
  };

  virtual void OnDetach() override {
    // Popcorn::Context::DisposeScene(scene);
  };
  virtual void OnUpdate(TimeEvent &e) override {
    building->RotateEuler<Axes::Y>(glm::radians(20.f) * e.GetDeltaS());
  };
  virtual void OnRender() override {
    // Draws all scenes
    Popcorn::Context::RenderScenes(scene);
  };
  virtual bool OnEvent(Event &e) override { return false; };

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
