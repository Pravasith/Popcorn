#include <Base.h>
#include <Material.h>
#include <Popcorn.h>
#include <Popcorn/Core/Application.h>
#include <Popcorn/Graphics/Materials/BasicMaterial.h>
#include <Popcorn/Graphics/Mesh.h>
#include <Popcorn/Graphics/Renderer.h>
#include <Popcorn/Graphics/VertexBuffer.h>
#include <Popcorn/Scene/Scene.h>
#include <Sources.h>
#include <glm/glm.hpp>

using namespace Popcorn;

class GameLayer : public Layer {
  class TriangleScene : public Scene {};

public:
  GameLayer() { PC_PRINT("CREATED", TagType::Constr, "GAME-LAYER") };
  ~GameLayer() { PC_PRINT("DESTROYED", TagType::Destr, "GAME-LAYER") };

  virtual void OnAttach() override {
    // TODO: Move all of this to proper functions
    //
    struct Vertex {
      glm::vec2 pos;
      glm::vec3 color;
      std::string Print() {
        std::stringstream ss;
        ss << pos.x << ", " << pos.y << "; " << color.r << ", " << color.g
           << ", " << color.b;

        return ss.str();
      };
    };

    auto *vertexBuffer = VertexBuffer::Create();

    vertexBuffer->Fill<Vertex>({
        {{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    });

    vertexBuffer->SetLayout<VertexBuffer::AttrTypes::Float2,
                            VertexBuffer::AttrTypes::Float3>();

    std::vector shaderFiles{
        "shaders/tri_vert.spv",
        "shaders/tri_frag.spv",
    };

    // NOTE: Shader files are loaded here -- expensive if not handled
    // properly
    MaterialData matData{(ShaderStages::VertexBit | ShaderStages::FragmentBit),
                         shaderFiles};

    triMat = new BasicMaterial(matData);

    // Mesh triMesh{nullptr, triMat};
    triMesh = new Mesh{vertexBuffer, *triMat};

    triScene.Add(triMesh); // HERE IS THE MATERIAL READY STAGE

    // AND THEN IN THE RENDER LOOP
    // Renderer.Render(triScene);

    vertexBuffer->PrintBuffer<Vertex>();
    VertexBuffer::Destroy(vertexBuffer);
  };

  virtual void OnDetach() override {
    delete triMesh;
    triMesh = nullptr;

    delete triMat;
    triMat = nullptr;
  };

  virtual void OnUpdate() override {
    //
    Renderer::Get().DrawFrame(triScene);
  };
  virtual void OnEvent(Event &e) override {};

private:
  TriangleScene triScene{};
  Mesh *triMesh;
  Material *triMat;
};

int main(int argc, char **argv) {
  // START POPCORN ENGINE
  Popcorn::Application::Start();
  auto &app = Application::Get();

  // CREATE AND SET RENDERER
  auto &renderer =
      *(Renderer::Create<RendererType::Vulkan>(app.GetAppWindow()));
  app.SetRenderer(renderer);

  // // CREATE A GAME LAYER
  auto gameLayer = new GameLayer();
  Application::AddLayer(gameLayer);

  // GAME LOOP
  Popcorn::Application::StartGameLoop();

  // FREE RESOURCES
  Popcorn::Application::Stop();

  return 0;
}
