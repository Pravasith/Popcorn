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
      glm::vec2 pos;
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

    vertexBuffer->SetLayout<BufferDefs::AttrTypes::Float2,
                            BufferDefs::AttrTypes::Float3>();

    // Single quad data (shared by all cube faces)
    vertexBuffer->Fill<Vertex>({{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}});

    indexBuffer->Fill({0, 1, 2, 2, 3, 0});

    // Material setup
    std::vector shaderFiles{"shaders/tri_vert.spv", "shaders/tri_frag.spv"};
    MaterialData matData{(ShaderStages::VertexBit | ShaderStages::FragmentBit),
                         shaderFiles};
    triMat = new BasicMaterial(matData);

    // Cube face positions and rotations
    const std::vector<glm::vec3> facePositions = {
        {0.0f, 0.0f, 0.5f},  // Front
        {0.0f, 0.0f, -0.5f}, // Back
        {-0.5f, 0.0f, 0.0f}, // Left
        {0.5f, 0.0f, 0.0f},  // Right
        {0.0f, 0.5f, 0.0f},  // Top
        {0.0f, -0.5f, 0.0f}  // Bottom
    };

    const std::vector<std::pair<char, float>> faceRotations = {
        {'Y', 0.0f},                 // Front
        {'Y', glm::radians(180.0f)}, // Back
        {'Y', glm::radians(-90.0f)}, // Left
        {'Y', glm::radians(90.0f)},  // Right
        {'X', glm::radians(-90.0f)}, // Top
        {'X', glm::radians(90.0f)}   // Bottom
    };

    // Create cube faces with proper rotations
    for (size_t i = 0; i < 6; ++i) {
      Mesh *face = new Mesh{*vertexBuffer, indexBuffer, *triMat};
      face->SetPosition(facePositions[i]);

      // Apply rotation based on axis before translation
      if (faceRotations[i].first == 'X') {
        face->RotateX(faceRotations[i].second);
      } else {
        face->RotateY(faceRotations[i].second);
      }

      cubeMeshes.push_back(face);
    }

    // Add to scene in the correct order (no depth buffering)
    triScene.Add(cubeMeshes[0]); // Front
    triScene.Add(cubeMeshes[1]); // Back
    triScene.Add(cubeMeshes[2]); // Left
    triScene.Add(cubeMeshes[3]); // Right
    triScene.Add(cubeMeshes[4]); // Top
    triScene.Add(cubeMeshes[5]); // Bottom
  };

  virtual void OnDetach() override {
    // Cleanup all meshes
    for (auto mesh : cubeMeshes) {
      delete mesh;
    }
    cubeMeshes.clear();

    delete triMat;
    triMat = nullptr;

    delete indexBuffer;
    indexBuffer = nullptr;

    VertexBuffer::Destroy(vertexBuffer);
  };

  virtual void OnUpdate(TimeEvent &e) override {
    // Rotate all cube faces
    // cubeMeshes[0]->RotateY(glm::radians(90.f) * e.GetDeltaS());
    for (auto &mesh : cubeMeshes) {
      mesh->RotateAroundWorldYAxis(glm::radians(90.f) * e.GetDeltaS());
    }
    triScene.Update();
  };

  virtual void OnRender() override { Renderer::Get().DrawFrame(triScene); };

  virtual bool OnEvent(Event &e) override { return false; };

private:
  VertexBuffer *vertexBuffer;
  IndexBuffer<uint16_t> *indexBuffer;
  TriangleScene triScene;
  std::vector<Mesh *> cubeMeshes;
  Material *triMat;
};

int main(int argc, char **argv) {
  Popcorn::Application::Start();
  auto &app = Application::Get();

  auto &renderer =
      *(Renderer::Create<RendererType::Vulkan>(app.GetAppWindow()));
  app.SetRenderer(renderer);

  auto gameLayer = new GameLayer();
  Application::AddLayer(gameLayer);

  renderer.SceneReady();
  Popcorn::Application::StartGameLoop();
  Popcorn::Application::Stop();

  return 0;
}
