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

    struct ModelMatrix {
      glm::mat4 model;
    };

    vertexBuffer = VertexBuffer::Create();
    vertexBuffer2 = VertexBuffer::Create();

    indexBuffer = new IndexBuffer<uint16_t>();

    vertexBuffer->Fill<Vertex>({
        {{-.5f, -.5f}, {.8f, .0f, .8f}},
        {{.0f, -.5f}, {.8f, .8f, .0f}},
        {{.0f, .0f}, {.0f, .8f, .8f}},
        {{-.5f, .0f}, {.8f, .0f, .8f}},
    });

    vertexBuffer2->Fill<Vertex>({
        {{.0f, .0f}, {.8f, .0f, .8f}},
        {{.5f, .0f}, {.8f, .8f, .0f}},
        {{.5f, .5f}, {.0f, .8f, .8f}},
        {{.0f, .5f}, {.8f, .0f, .8f}},
    });

    indexBuffer->Fill({3, 0, 1, 1, 2, 3});
    vertexBuffer->SetLayout<BufferDefs::AttrTypes::Float2,
                            BufferDefs::AttrTypes::Float3>();
    vertexBuffer2->SetLayout<BufferDefs::AttrTypes::Float2,
                             BufferDefs::AttrTypes::Float3>();

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
    triMesh = new Mesh{*vertexBuffer, indexBuffer, *triMat};
    triMesh2 = new Mesh{*vertexBuffer2, indexBuffer, *triMat};

    triMesh->SetPosition({.0f, .1f, .0f});
    triMesh2->SetPosition({.0f, -.1f, .0f});

    // ADD MESH TO WORK FLOW -> CREATE PIPELINES
    triScene.Add(triMesh);
    triScene.Add(triMesh2);

    // vertexBuffer->PrintBuffer<Vertex>();
  };

  virtual void OnDetach() override {
    delete triMesh;
    triMesh = nullptr;

    delete triMesh2;
    triMesh2 = nullptr;

    delete triMat;
    triMat = nullptr;

    delete indexBuffer;
    indexBuffer = nullptr;

    VertexBuffer::Destroy(vertexBuffer);
    VertexBuffer::Destroy(vertexBuffer2);
  };

  virtual void OnUpdate(TimeEvent &e) override {
    // Update uniforms here
    triMesh.RotateY(90.f);
    triMesh2.RotateY(90.f);
  };

  virtual void OnRender() override {
    //
    Renderer::Get().DrawFrame(triScene);
  };

  virtual void OnEvent(Event &e) override {};

private:
  VertexBuffer *vertexBuffer;
  VertexBuffer *vertexBuffer2;

  IndexBuffer<uint16_t> *indexBuffer;

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
