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

    vertexBuffer1 = VertexBuffer::Create();
    vertexBuffer2 = VertexBuffer::Create();

    indexBuffer = new IndexBuffer<uint16_t>(); // this is okay
    vertexBuffer1->SetLayout<BufferDefs::AttrTypes::Float2,
                             BufferDefs::AttrTypes::Float3>();
    vertexBuffer2->SetLayout<BufferDefs::AttrTypes::Float2,
                             BufferDefs::AttrTypes::Float3>();

    vertexBuffer1->Fill<Vertex>({{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                 {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                 {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                 {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}});
    vertexBuffer2->Fill<Vertex>({{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                 {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                 {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                 {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}});

    indexBuffer->Fill({0, 1, 2, 2, 3, 0});

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
    triMesh1 = new Mesh{*vertexBuffer1, indexBuffer, *triMat};
    triMesh2 = new Mesh{*vertexBuffer2, indexBuffer, *triMat};

    triMesh1->SetPosition({.0f, .0f, .5f});
    triMesh2->SetPosition({.0f, .0f, -.5f});

    // ADD MESH TO WORK FLOW -> CREATE PIPELINES
    triScene.Add(triMesh2);
    triScene.Add(triMesh1);

    // vertexBuffer->PrintBuffer<Vertex>();
  };

  virtual void OnDetach() override {
    delete triMesh1;
    triMesh1 = nullptr;

    delete triMesh2;
    triMesh2 = nullptr;

    delete triMat;
    triMat = nullptr;

    delete indexBuffer;
    indexBuffer = nullptr;

    VertexBuffer::Destroy(vertexBuffer1);
    VertexBuffer::Destroy(vertexBuffer2);
  };

  virtual void OnUpdate(TimeEvent &e) override {
    // Update uniforms here
    triMesh1->RotateY(glm::radians(90.f) * e.GetDeltaS());
    triMesh2->RotateY(glm::radians(90.f) * e.GetDeltaS());

    // TODO: Refactor these so user doesn't have to call update
    triScene.Update();
    // e.PrintDebugData();
  };

  virtual void OnRender() override {
    //
    Renderer::Get().DrawFrame(triScene);
  };

  virtual bool OnEvent(Event &e) override { return false; };

private:
  VertexBuffer *vertexBuffer1;
  VertexBuffer *vertexBuffer2;
  VertexBuffer *vertexBuffer3;
  VertexBuffer *vertexBuffer4;
  VertexBuffer *vertexBuffer5;
  VertexBuffer *vertexBuffer6;

  IndexBuffer<uint16_t> *indexBuffer;

  TriangleScene triScene{};

  Mesh *triMesh1;
  Mesh *triMesh2;
  Mesh *triMesh3;
  Mesh *triMesh4;
  Mesh *triMesh5;
  Mesh *triMesh6;

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

  // TODO: Refactor these so user doesn't have to call SceneReady
  // INDICATE SCENE READY
  renderer.SceneReady();

  // GAME LOOP
  Popcorn::Application::StartGameLoop();

  // FREE RESOURCES
  Popcorn::Application::Stop();

  return 0;
}
