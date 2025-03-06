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

    vertexBuffer = VertexBuffer::Create();
    vertexBuffer2 = VertexBuffer::Create();

    vertexBuffer->Fill<Vertex>({
        {{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    });

    vertexBuffer2->Fill<Vertex>({
        {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    });

    vertexBuffer->SetLayout<VertexBuffer::AttrTypes::Float2,
                            VertexBuffer::AttrTypes::Float3>();
    vertexBuffer2->SetLayout<VertexBuffer::AttrTypes::Float2,
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
    triMesh = new Mesh{*vertexBuffer, *triMat};
    triMesh2 = new Mesh{*vertexBuffer2, *triMat};

    // ADD MESH TO WORK FLOW -> CREATE PIPELINES
    triScene.Add(triMesh);
    triScene.Add(triMesh2);

    // AND THEN IN THE RENDER LOOP
    // Renderer.Render(triScene);

    // vertexBuffer->PrintBuffer<Vertex>();
  };

  virtual void OnDetach() override {
    delete triMesh;
    triMesh = nullptr;
    delete triMesh2;
    triMesh2 = nullptr;

    delete triMat;
    triMat = nullptr;

    VertexBuffer::Destroy(vertexBuffer);
    VertexBuffer::Destroy(vertexBuffer2);
  };

  virtual void OnUpdate() override {
    //
    Renderer::Get().DrawFrame(triScene);
  };
  virtual void OnEvent(Event &e) override {};

private:
  VertexBuffer *vertexBuffer;
  VertexBuffer *vertexBuffer2;
  TriangleScene triScene{};
  Mesh *triMesh;
  Mesh *triMesh2;
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

  // CREATE A GAME LAYER
  auto gameLayer = new GameLayer();
  Application::AddLayer(gameLayer);

  // INDICATE SCENE READY
  renderer.SceneReady();

  // GAME LOOP
  Popcorn::Application::StartGameLoop();

  // FREE RESOURCES
  Popcorn::Application::Stop();

  return 0;
}
