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
#include <vector>

using namespace Popcorn;

class GameLayer : public Layer {
  class TriangleScene : public Scene {};

public:
  GameLayer() { PC_PRINT("CREATED", TagType::Constr, "GAME-LAYER") };
  ~GameLayer() { PC_PRINT("DESTROYED", TagType::Destr, "GAME-LAYER") };

  virtual void OnAttach() override {
    struct Vertex {
      glm::vec3 pos;
      glm::vec3 color;
      std::string Print() {
        std::stringstream ss;
        ss << pos.x << ", " << pos.y << "; " << color.r << ", " << color.g
           << ", " << color.b;
        return ss.str();
      };
    };

    // Create shared buffers
    vertexBuffer = VertexBuffer::Create();
    indexBuffer = new IndexBuffer<uint16_t>();

    vertexBuffer->SetLayout<BufferDefs::AttrTypes::Float3,
                            BufferDefs::AttrTypes::Float3>();

    // Single quad data (shared by all cube faces)
    vertexBuffer->Fill<Vertex>({{{-0.5f, -0.5f, .0f}, {1.0f, 0.0f, 0.0f}},
                                {{0.5f, -0.5f, .0f}, {0.0f, 1.0f, 0.0f}},
                                {{0.5f, 0.5f, .0f}, {0.0f, 0.0f, 1.0f}},
                                {{-0.5f, 0.5f, .0f}, {1.0f, 1.0f, 1.0f}}});

    indexBuffer->Fill({0, 1, 2, 2, 3, 0});

    // Material setup
    std::vector shaderFiles{"shaders/tri_vert.spv", "shaders/tri_frag.spv"};
    MaterialData matData{(ShaderStages::VertexBit | ShaderStages::FragmentBit),
                         shaderFiles};
    triMat = new BasicMaterial(matData);

    mesh = new Mesh{*vertexBuffer, indexBuffer, *triMat};

    mesh->SetPosition({.0f, 1.f, 0.f});
    mesh->RotateX(glm::radians(-90.f));

    triScene.AddNode(mesh);

    auto &renderer = Popcorn::Context::GetRenderer();
    renderer.AddScene(&triScene);
  };

  virtual void OnDetach() override {
    delete triMat;
    triMat = nullptr;

    delete indexBuffer;
    indexBuffer = nullptr;

    VertexBuffer::Destroy(vertexBuffer);
    delete mesh;
  };

  virtual void OnUpdate(TimeEvent &e) override {
    mesh->RotateZ(glm::radians(90.f) * e.GetDeltaS());
    triScene.Update();
  };

  virtual void OnRender() override { Renderer::Get().DrawFrame(triScene); };

  virtual bool OnEvent(Event &e) override { return false; };

private:
  VertexBuffer *vertexBuffer;
  IndexBuffer<uint16_t> *indexBuffer;
  TriangleScene triScene;
  Mesh *mesh;
  Material *triMat;
};

int main(int argc, char **argv) {
  Popcorn::Context::Begin();

  auto gameLayer = new GameLayer();
  Application::AddLayer(gameLayer);

  Popcorn::Context::RunGame();

  Popcorn::Context::End();
  return 0;
}
