#include "Popcorn/Loaders/GltfLoader.h"
#include "Assert.h"
#include "Base.h"
#include "BufferObjects.h"
#include "GlobalMacros.h"
#include "Helpers.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

bool GltfLoader::LoadFromFile(const std::string &filename,
                              tinygltf::Model &model) {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename) ||
             loader.LoadBinaryFromFile(&model, &err, &warn, filename);

  if (!warn.empty()) {
    PC_WARN(warn)
  }

  if (!err.empty()) {
    PC_ERROR(err, "GltfLoader")
  }

  if (!ret) {
    std::cerr << "Failed to parse glTF" << std::endl;
  }

  return ret;
};

std::vector<Mesh *>
GltfLoader::ExtractGltfMeshes(const tinygltf::Model &model) {
  std::vector<Mesh *> meshes;

  for (const auto &node : model.nodes) {
    ProcessGLTFNode(model, node, glm::mat4(1.0f), meshes);
  }

  return meshes;
};

void GltfLoader::ProcessGLTFNode(const tinygltf::Model &model,
                                 const tinygltf::Node &node,
                                 const glm::mat4 &parentMatrix,
                                 std::vector<Mesh *> &meshes) {
  // Calculate current node's transformation matrix
  glm::mat4 nodeMatrix = glm::mat4(1.0f);

  if (!node.matrix.empty()) {
    // Node has an explicit matrix
    nodeMatrix = glm::make_mat4(node.matrix.data());
  } else {
    // Apply TRS components
    if (!node.translation.empty()) {
      nodeMatrix = glm::translate(nodeMatrix, glm::vec3(node.translation[0],
                                                        node.translation[1],
                                                        node.translation[2]));
    }

    if (!node.rotation.empty()) {
      glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1],
                  node.rotation[2]);
      nodeMatrix *= glm::mat4_cast(q);
    }

    if (!node.scale.empty()) {
      nodeMatrix = glm::scale(
          nodeMatrix, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
    }
  }

  glm::mat4 worldMatrix = parentMatrix * nodeMatrix;

  // Process mesh if this node has one
  if (node.mesh >= 0 && node.mesh < model.meshes.size()) {
    const auto &gltfMesh = model.meshes[node.mesh];

    // gltfMesh.primitives are submeshes
    for (const auto &primitive : gltfMesh.primitives) {
      VertexBuffer *vertexBuffer;
      IndexBuffer<uint16_t> *indexBuffer;
      Mesh::Uniforms uniform;

      // Extract vertex data
      // TODO: for tomorrow: transfer the vertexbuffer ownership to Mesh &
      // handle cleanup logic in the Mesh
      vertexBuffer = ExtractVertexBuffer(model, primitive);

      // Extract index data
      indexBuffer = ExtractIndexBuffer(model, primitive);

      // Extract material
      // if (primitive.material >= 0) {
      //   extractMaterialData(model, model.materials[primitive.material],
      //                       extractedMesh.material);
      // }
      Mesh *mesh = new Mesh(vertexBuffer, indexBuffer);
      mesh->SetModelMatrix(worldMatrix);

      meshes.push_back(mesh);
    }
  }

  // Process child nodes
  for (int child : node.children) {
    ProcessGLTFNode(model, model.nodes[child], worldMatrix, meshes);
  }
}

VertexBuffer *
GltfLoader::ExtractVertexBuffer(const tinygltf::Model &model,
                                const tinygltf::Primitive &primitive) {
  const byte_t *posData = nullptr;
  const byte_t *normData = nullptr;
  const byte_t *uvData = nullptr;

  size_t posStride = 0;
  size_t normStride = 0;
  size_t uvStride = 0;

  size_t posByteSize = 0;
  size_t normByteSize = 0;
  size_t uvByteSize = 0;

  size_t vertexCount = 0;

  //
  // --- POSITION ------------------------------------------------------
  PC_ASSERT(primitive.attributes.count("POSITION"),
            "Mesh primitive missing POSITION attribute");

  const auto &posAccessor =
      model.accessors[primitive.attributes.at("POSITION")];
  const auto &posView = model.bufferViews[posAccessor.bufferView];
  const auto &posBuffer = model.buffers[posView.buffer];

  posData = reinterpret_cast<const byte_t *>(
      posBuffer.data.data() + posAccessor.byteOffset + posView.byteOffset);
  posStride = posView.byteStride ? posView.byteStride : 3 * sizeof(float);
  posByteSize = tinygltf::GetComponentSizeInBytes(posAccessor.componentType) *
                tinygltf::GetNumComponentsInType(posAccessor.type);

  //
  // --- NORMAL --------------------------------------------------------
  if (!primitive.attributes.count("NORMAL")) {
    PC_WARN("Warning: Mesh missing NORMALs. Flat shading will be used.")
    normByteSize = 3 * sizeof(float);
  } else {
    const auto &normAccessor =
        model.accessors[primitive.attributes.at("NORMAL")];
    const auto &normView =
        model.bufferViews[normAccessor.bufferView]; // normView is HERE
    const auto &normBuffer = model.buffers[normView.buffer];

    normData = reinterpret_cast<const byte_t *>(
        normBuffer.data.data() + normAccessor.byteOffset + normView.byteOffset);
    normStride = normView.byteStride ? normView.byteStride : 3 * sizeof(float);
    normByteSize =
        (normData)
            ? tinygltf::GetComponentSizeInBytes(normAccessor.componentType) *
                  tinygltf::GetNumComponentsInType(normAccessor.type)
            : 3 * sizeof(float); // Default normal size
  };

  //
  // --- TEXCOORD_0 ----------------------------------------------------
  if (!primitive.attributes.count("TEXCOORD_0")) {
    PC_WARN("Warning: Mesh missing TEXCOORD_0. Default UVs will be used.")
    uvByteSize = 2 * sizeof(float);
  } else {
    const auto &uvAccessor =
        model.accessors[primitive.attributes.at("TEXCOORD_0")];
    const auto &uvView =
        model.bufferViews[uvAccessor.bufferView]; // uvView is HERE
    const auto &uvBuffer = model.buffers[uvView.buffer];

    uvData = reinterpret_cast<const byte_t *>(
        uvBuffer.data.data() + uvAccessor.byteOffset + uvView.byteOffset);
    uvStride = uvView.byteStride ? uvView.byteStride : 2 * sizeof(float);
    uvByteSize =
        (uvData) ? tinygltf::GetComponentSizeInBytes(uvAccessor.componentType) *
                       tinygltf::GetNumComponentsInType(uvAccessor.type)
                 : 2 * sizeof(float); // Default UV size
  }

  //
  // --- INTERLEAVE  ---------------------------------------------------
  vertexCount = posAccessor.count;
  size_t totalAttrByteSize = (posByteSize + normByteSize + uvByteSize);

  // Destroyed when mesh is destroyed
  VertexBuffer *vertices = VertexBuffer::Create();
  vertices->Allocate(vertexCount * totalAttrByteSize);
  {
    using BufferDefs::AttrTypes;
    vertices->SetLayout<AttrTypes::Float3,    // Position
                        AttrTypes::Float3,    // Normal
                        AttrTypes::Float2>(); // UV
  }

  for (size_t i = 0; i < vertexCount; ++i) {
    auto *verticesBuffer = vertices->GetBufferData() + i * totalAttrByteSize;
    size_t cpyOffset = 0;

    const byte_t *pos =
        reinterpret_cast<const byte_t *>(posData + i * posStride);
    memcpy(verticesBuffer + cpyOffset, pos, posByteSize);
    cpyOffset += posByteSize;

    if (normData) {
      const byte_t *norm =
          reinterpret_cast<const byte_t *>(normData + i * normStride);
      memcpy(verticesBuffer + cpyOffset, norm, normByteSize);
      cpyOffset += normByteSize;
    } else {
      const float defaultNormal[3] = {0, 0, 0};
      memcpy(verticesBuffer + cpyOffset, defaultNormal, normByteSize);
    }

    if (uvData) {
      const byte_t *uv =
          reinterpret_cast<const byte_t *>(uvData + i * uvStride);
      memcpy(verticesBuffer + cpyOffset, uv, uvByteSize);
      cpyOffset += uvByteSize;
    } else {
      const float defaultUV[2] = {0, 0};
      memcpy(verticesBuffer + cpyOffset, defaultUV, uvByteSize);
    }
  }

  return vertices;
}

IndexBuffer<uint16_t> *
GltfLoader::ExtractIndexBuffer(const tinygltf::Model &model,
                               const tinygltf::Primitive &primitive) {
  PC_ASSERT(primitive.indices >= 0, "Primitive has no indices");

  const auto &indexAccessor = model.accessors[primitive.indices];
  const size_t indexCount = indexAccessor.count;

  // Allocate index buffer
  IndexBuffer<uint16_t> *indices = new IndexBuffer<uint16_t>();
  indices->Allocate(indexCount);

  const auto &indexView = model.bufferViews[indexAccessor.bufferView];
  const auto &gltfIndexBuffer = model.buffers[indexView.buffer];

  const byte_t *srcData = gltfIndexBuffer.data.data() + indexView.byteOffset +
                          indexAccessor.byteOffset;

  // Handle stride (distance between indices in bytes)
  size_t stride = indexView.byteStride;
  if (stride == 0) {
    // Default stride based on component type
    switch (indexAccessor.componentType) {
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
      stride = sizeof(uint16_t);
      break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
      stride = sizeof(uint32_t);
      break;
    default:
      PC_ERROR("Unsupported index type", "GltfLoader");
      delete indices;
      return nullptr;
    }
  }

  // Convert indices to 16-bit
  for (size_t i = 0; i < indexCount; ++i) {
    const byte_t *src = srcData + i * stride;
    uint32_t index;

    switch (indexAccessor.componentType) {
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
      memcpy(&index, src, sizeof(uint16_t));
      break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
      memcpy(&index, src, sizeof(uint32_t));
      if (index > 0xFFFF) {
        PC_ERROR("32-bit index exceeds 16-bit limit", "GltfLoader");
        delete indices;
        return nullptr;
      }
      break;
    default:
      delete indices;
      return nullptr;
    }

    indices->GetBufferData()[i] = static_cast<uint16_t>(index);
  }

  return indices;
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
