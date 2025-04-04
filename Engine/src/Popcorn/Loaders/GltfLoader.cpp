#include "Popcorn/Loaders/GltfLoader.h"
#include "Assert.h"
#include "Base.h"
#include "BufferObjects.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Helpers.h"
#include "Material.h"
#include "Mesh.h"
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

void GltfLoader::ExtractMeshData(const tinygltf::Model &model,
                                 const tinygltf::Node &gltfNode, Mesh &mesh) {
  // MESH TYPE
  const auto &gltfMesh = model.meshes[gltfNode.mesh];

  // gltfMesh.primitives are submeshes
  for (const auto &primitive : gltfMesh.primitives) {
    VertexBuffer *vbo;
    IndexBuffer<uint16_t> *ibo;
    MaterialData *matData;
    Material *mat;

    // Extract vertex buffer
    vbo = ExtractVertexBuffer(model, primitive);

    // Extract index buffer
    ibo = ExtractIndexBuffer(model, primitive);

    // Extract material data
    if (primitive.material >= 0) {
      ExtractMaterialData(model, model.materials[primitive.material], matData);
      // TODO: Create material
    }

    mesh.AddSubmesh(vbo, ibo, mat);
  }
};

void GltfLoader::ExtractModelData(const tinygltf::Model &model,
                                  std::vector<GameObject *> &gameObjects) {
  for (const auto &node : model.nodes) {
    GameObject *gameObject = ConvertGltfNodeToGameObject(model, node);
    gameObjects.push_back(gameObject);
  }
};

GameObject *GltfLoader::ProcessNodeByType(const tinygltf::Model &model,
                                          const tinygltf::Node &node) {
  if (node.mesh >= 0 && node.mesh < model.meshes.size()) {
    Mesh *mesh = new Mesh();
    ExtractMeshData(model, node, *mesh);
    return mesh;
  } else if (node.camera >= 0) {
    // Camera node
    // TYPE = Camera
  } else if (node.extensions.find("KHR_lights_punctual") !=
             node.extensions.end()) {
    // Light node (requires KHR_lights_punctual extension)
    // TYPE = Light
  } else if (
      // Is Node referenced in any skin.joints array (typically write a function
      // to resolve the bool)
      false) {
    // Bone/Joint node (part of skeletal hierarchy)
    // TYPE = Bone
    // TYPE = Joint
  } else if (node.extensions.find("EXT_mesh_gpu_instancing") !=
             node.extensions.end()) {
    // GPU-instanced mesh node
    // TYPE = Mesh but instanced
  } else if (!node.translation.empty() || !node.rotation.empty() ||
             !node.scale.empty() || !node.matrix.empty()) {
    // Transform (empty) node (only affects hierarchy)
    // TYPE = Transform (Empty)
  } else if (!node.extensions.empty()) {
    // Node with unspecified extensions (vendor-specific)
    // NO TYPE -- HANDLE THIS
  } else {
    // Undefined node (no properties, rare but valid)
    // NO TYPE -- HANDLE THIS
  }
};

// TODO: Decompose matrix to TRS for animations
void GltfLoader::SetTransformData(const tinygltf::Node &node,
                                  GameObject &gameObject) {
  auto outMatrix = glm::mat4(1.0f);

  // Node has an explicit transformation matrix, set it directly.
  if (!node.matrix.empty()) {
    // TODO: Warning --- in this cases (usually on shear/uneven scale
    // operations, Blender exports a nodeMatrix instead of transformations.
    // Handle this later by decomposition. Or just don't use such objects for
    // animations.
    outMatrix = glm::make_mat4(node.matrix.data());
    gameObject.SetLocalMatrix(outMatrix);
  } else {
    // Apply TRS components separately
    glm::vec3 translation(0.0f);
    glm::vec3 scale(1.0f);
    glm::vec3 eulerAngles(0.0f); // Euler angles in radians

    if (!node.translation.empty()) {
      translation = glm::vec3(node.translation[0], node.translation[1],
                              node.translation[2]);
    }

    if (!node.rotation.empty()) {
      glm::quat rotationQuat(node.rotation[3], node.rotation[0],
                             node.rotation[1], node.rotation[2]);
      eulerAngles =
          glm::eulerAngles(rotationQuat); // Convert quaternion to Euler angles
    }

    if (!node.scale.empty()) {
      scale = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
    }

    // Store TRS in the GameObject --- internally constructs a localMatrix
    gameObject.SetPosition(translation);
    gameObject.SetRotationEuler(eulerAngles);
    gameObject.SetScale(scale);
  }
}

GameObject *
GltfLoader::ConvertGltfNodeToGameObject(const tinygltf::Model &model,
                                        const tinygltf::Node &gltfNode) {
  GameObject *gameObject = ProcessNodeByType(model, gltfNode);
  SetTransformData(gltfNode, *gameObject);

  for (int child : gltfNode.children) {
    GameObject *childGameObj =
        ConvertGltfNodeToGameObject(model, model.nodes[child]);
    gameObject->AddChild(childGameObj);
  }

  return gameObject;
}

void GltfLoader::ExtractMaterialData(const tinygltf::Model &model,
                                     const tinygltf::Material &material,
                                     MaterialData *materialData) {
  //
  // Random thought: Valora is a sick name for a videogame character
  //
  // Base color factor
  const auto &pbrMetallicRoughness = material.pbrMetallicRoughness;

  // Determine if the material is PBR-based
  bool isPBR = pbrMetallicRoughness.metallicFactor > 0.0f ||
               pbrMetallicRoughness.roughnessFactor > 0.0f ||
               material.normalTexture.index != -1 ||
               material.occlusionTexture.index != -1;

  // struct BasicMaterialData : public MaterialData {
  //   glm::vec4 baseColorFactor = glm::vec4(1.0f);
  // };

  // struct PbrMaterialData : public MaterialData {
  //   bool hasBaseColorTexture = false;
  //   bool hasNormalTexture = false;
  //   bool hasMetallicRoughnessTexture = false;
  //   float metallicFactor = 1.0f;
  //   float roughnessFactor = 1.0f;
  //   float alphaCutoff = 0.5f;
  //   glm::vec4 baseColorFactor = glm::vec4(1.0f);
  // };

  // Reinterpret cast for MaterialData accordingly based on material's values
};

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
