#include <Popcorn.h>

using namespace Popcorn;

class GameLayer : public Layer {
public:
  GameLayer() { PC_PRINT("CREATED", TagType::Constr, "GAME-LAYER") };
  ~GameLayer() { PC_PRINT("DESTROYED", TagType::Destr, "GAME-LAYER") };

  virtual void OnAttach() override {};
  virtual void OnDetach() override {};
  virtual void OnUpdate() override {};
  virtual void OnEvent(Event &e) override {};
};

int main(int argc, char **argv) {

  // START THE APPLICATION WITH WINDOW
  Popcorn::Application::Init();

  auto gameLayer = new GameLayer();
  Application::AddLayer(gameLayer);

  Popcorn::Application::StartGameLoop();
  Popcorn::Application::Stop();

  return 0;
}
