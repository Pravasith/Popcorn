#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "glm/glm.hpp"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Mesh : public GameObject {
public:
  Mesh() : m_vertexBuffer(VertexBuffer::Create()) {
    PC_PRINT("CREATED", TagType::Constr, "MESH");
  };
  ~Mesh() {
    VertexBuffer::Destroy(m_vertexBuffer);
    PC_PRINT("DESTROYED", TagType::Destr, "MESH");
  };

  virtual void OnAttach(const SceneData &) override {
    // TODO: inject vertices from outside this class instead
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

    m_vertexBuffer->Fill<Vertex>({
        {{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    });

    m_vertexBuffer->SetLayout<VertexBuffer::AttrTypes::Float2,
                              VertexBuffer::AttrTypes::Float3>();
  };
  virtual void OnUpdate(const SceneData &) override {};
  virtual void OnRender(const SceneData &) override {
    if (m_material == nullptr) {
      PC_WARN("m_material is nullptr");
      return;
    }

    // FIND MATERIALS & RENDER THEM WITH THE RENDERPASS INFORMATION
  };

private:
  VertexBuffer *m_vertexBuffer = nullptr;
  Material *m_material = nullptr;
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
