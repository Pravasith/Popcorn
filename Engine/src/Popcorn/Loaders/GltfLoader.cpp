#include "Popcorn/Loaders/GltfLoader.h"
#include "Base.h"
#include "GlobalMacros.h"
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

std::vector<GltfMeshType>
GltfLoader::ExtractGltfMeshes(const tinygltf::Model &model) {
  std::vector<GltfMeshType> meshes;

  for (const auto &node : model.nodes) {
    ProcessGLTFNode(model, node, glm::mat4(1.0f), meshes);
  }

  return meshes;
};

void GltfLoader::ProcessGLTFNode(const tinygltf::Model &model,
                                 const tinygltf::Node &node,
                                 const glm::mat4 &parentMatrix,
                                 std::vector<GltfMeshType> &meshes) {
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
    const auto &mesh = model.meshes[node.mesh];

    for (const auto &primitive : mesh.primitives) {
      GltfMeshType extractedMesh;
      extractedMesh.modelMatrix = worldMatrix;

      // Extract vertex data
      ExtractVertexData(model, primitive, extractedMesh);

      // Extract index data
      // extractIndexData(model, primitive, extractedMesh);

      // Extract material
      // if (primitive.material >= 0) {
      //   extractMaterialData(model, model.materials[primitive.material],
      //                       extractedMesh.material);
      // }

      meshes.push_back(extractedMesh);
    }
  }

  // Process child nodes
  for (int child : node.children) {
    ProcessGLTFNode(model, model.nodes[child], worldMatrix, meshes);
  }
}

void GltfLoader::ExtractVertexData(const tinygltf::Model &model,
                                   const tinygltf::Primitive &primitive,
                                   GltfMeshType &outMesh) {
  // Position attribute is required
  const auto &posAccessor =
      model.accessors[primitive.attributes.at("POSITION")];
  const auto &posView = model.bufferViews[posAccessor.bufferView];
  const auto &posBuffer = model.buffers[posView.buffer];
  const float *posData = reinterpret_cast<const float *>(
      &posBuffer.data[posAccessor.byteOffset + posView.byteOffset]);

  // Normals (optional)
  const float *normData = nullptr;
  if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
    const auto &normAccessor =
        model.accessors[primitive.attributes.at("NORMAL")];
    const auto &normView = model.bufferViews[normAccessor.bufferView];
    const auto &normBuffer = model.buffers[normView.buffer];
    normData = reinterpret_cast<const float *>(
        &normBuffer.data[normAccessor.byteOffset + normView.byteOffset]);
  }

  // TexCoords (optional)
  const float *texData = nullptr;
  if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
    const auto &texAccessor =
        model.accessors[primitive.attributes.at("TEXCOORD_0")];
    const auto &texView = model.bufferViews[texAccessor.bufferView];
    const auto &texBuffer = model.buffers[texView.buffer];
    texData = reinterpret_cast<const float *>(
        &texBuffer.data[texAccessor.byteOffset + texView.byteOffset]);
  }

  // Combine all vertex attributes into a single interleaved buffer
  size_t vertexCount = posAccessor.count;
  outMesh.vertices.reserve(vertexCount * 8); // pos(3) + norm(3) + tex(2)

  for (size_t i = 0; i < vertexCount; ++i) {
    // Position
    outMesh.vertices.push_back(posData[i * 3 + 0]);
    outMesh.vertices.push_back(posData[i * 3 + 1]);
    outMesh.vertices.push_back(posData[i * 3 + 2]);

    // Normal
    outMesh.vertices.push_back(normData ? normData[i * 3 + 0] : 0.0f);
    outMesh.vertices.push_back(normData ? normData[i * 3 + 1] : 1.0f);
    outMesh.vertices.push_back(normData ? normData[i * 3 + 2] : 0.0f);

    // TexCoord
    outMesh.vertices.push_back(texData ? texData[i * 2 + 0] : 0.0f);
    outMesh.vertices.push_back(texData ? texData[i * 2 + 1] : 0.0f);
  }
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
