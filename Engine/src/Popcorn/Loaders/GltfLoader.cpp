#include "GltfLoader.h"
#include "Assert.h"
#include "Base.h"
#include "BufferObjects.h"
#include "Camera.h"
#include "Empty.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Helpers.h"
#include "Light.h"
#include "LoadersDefs.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// --- PUBLIC METHODS ----------------------------------------------------------
//
bool GltfLoader::LoadFromFile(const std::string &filename,
                              tinygltf::Model &model) {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  bool ret = false;
  if (filename.ends_with(".gltf")) {
    ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
  } else if (filename.ends_with(".glb")) {
    ret = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
  } else {
    PC_ERROR("Unsupported file extension: " + filename, "GltfLoader");
    return false;
  }

  if (!warn.empty()) {
    PC_WARN(warn);
  }

  if (!err.empty()) {
    PC_ERROR(err, "GltfLoader");
  }

  if (!ret) {
    std::cerr << "Failed to parse glTF" << std::endl;
  }

  return ret;
}

// void GltfLoader::ExtractModelData(const tinygltf::Model &model,
//                                   std::vector<GameObject *> &gameObjects) {
//   for (auto &node : model.nodes) {
//     GameObject *gameObject = ConvertGltfNodeToGameObject(model, node);
//     gameObjects.push_back(gameObject);
//   }
// };

void GltfLoader::ExtractModelData(const tinygltf::Model &model,
                                  std::vector<GameObject *> &gameObjects) {
  // pick the default scene (or fall back to scene 0)
  int sceneIndex = model.defaultScene >= 0 ? model.defaultScene : 0;
  const tinygltf::Scene &scene = model.scenes[sceneIndex];

  // only iterate root nodes
  for (int nodeIndex : scene.nodes) {
    GameObject *gameObject =
        ConvertGltfNodeToGameObject(model, model.nodes[nodeIndex]);
    gameObjects.push_back(gameObject);
  }
}

//
//
//
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// --- CONVERSION UTILS --------------------------------------------------------
//
GameObject *
GltfLoader::ConvertGltfNodeToGameObject(const tinygltf::Model &model,
                                        const tinygltf::Node &gltfNode) {
  GameObject *gameObject = CreateGameObjectByType(model, gltfNode);
  gameObject->name = gltfNode.name;

  SetTransformData(gltfNode, *gameObject);

  for (int child : gltfNode.children) {
    GameObject *childGameObj =
        ConvertGltfNodeToGameObject(model, model.nodes[child]);
    gameObject->AddChild(childGameObj);
  }

  return gameObject;
}

void GltfLoader::SetTransformData(const tinygltf::Node &node,
                                  GameObject &gameObject) {

  // TODO: Decompose matrix to TRS for animations
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
    gameObject.ScaleByValue(scale);
  }
}

GameObject *GltfLoader::CreateGameObjectByType(const tinygltf::Model &model,
                                               const tinygltf::Node &node) {
  if (node.mesh >= 0 && node.mesh < model.meshes.size()) {
    Mesh *mesh = new Mesh();
    ExtractMeshData(model, node, *mesh);
    return mesh;
  } else if (node.camera >= 0) {
    // Make this abstract
    Camera *camera = new Camera();
    // ExtractCameraData(model, node, camera);
    // TODO: Handle camera later
    return camera;
  } else if (node.extensions.find("KHR_lights_punctual") !=
             node.extensions.end()) {
    // Light node (requires KHR_lights_punctual extension)
    Light *light = new Light();
    ExtractLightsData(model, node, light);
    return light;

    // } else if (
    //     // Is Node referenced in any skin.joints array (typically write a
    //     function
    //     // to resolve the bool)
    //     false) {
    //   // Bone/Joint node (part of skeletal hierarchy)
    //   // TYPE = Bone
    //   // TYPE = Joint
    // } else if (node.extensions.find("EXT_mesh_gpu_instancing") !=
    //            node.extensions.end()) {
    //   // GPU-instanced mesh node
    //   // TYPE = Mesh but instanced
    // } else if (!node.translation.empty() || !node.rotation.empty() ||
    //            !node.scale.empty() || !node.matrix.empty()) {
    //   // Transform (empty) node (only affects hierarchy)
    //   // TYPE = Transform (Empty)
    // } else if (!node.extensions.empty()) {
    //   // Node with unspecified extensions (vendor-specific)
    //   // NO TYPE -- HANDLE THIS
  } else {
    // Undefined node (no properties, rare but valid)
    // NO TYPE -- HANDLE THIS
    return new Empty();
  }
};

void GltfLoader::ExtractLightsData(const tinygltf::Model &model,
                                   const tinygltf::Node &node, Light *light) {
  // Safety check
  if (node.extensions.find("KHR_lights_punctual") == node.extensions.end())
    return;

  const auto &ext = node.extensions.at("KHR_lights_punctual");

  if (!ext.Has("light"))
    return; // no valid light index

  int lightIndex = ext.Get("light").Get<int>();
  if (lightIndex < 0 || lightIndex >= model.lights.size())
    return;

  const auto &gltfLight = model.lights[lightIndex];
  LightData data{};

  // Color
  if (!gltfLight.color.empty() && gltfLight.color.size() == 3) {
    data.color = glm::vec3(static_cast<float>(gltfLight.color[0]),
                           static_cast<float>(gltfLight.color[1]),
                           static_cast<float>(gltfLight.color[2]));
  }

  // Intensity
  data.intensity = static_cast<float>(gltfLight.intensity); // default is 1.0

  // Range
  if (gltfLight.range >= 0.0) {
    data.range = static_cast<float>(gltfLight.range);
  }

  // Spot angles (only if spot light)
  if (data.type == Lights::SpotLight) {
    const auto &spot = gltfLight.spot;
    data.innerConeAngle = spot.innerConeAngle >= 0.0
                              ? static_cast<float>(spot.innerConeAngle)
                              : glm::radians(15.0f);
    data.outerConeAngle = spot.outerConeAngle >= 0.0
                              ? static_cast<float>(spot.outerConeAngle)
                              : glm::radians(30.0f);
  }

  // Type
  if (gltfLight.type == "point") {
    constexpr float PC_POINT_LIGHTS_BLENDER_POWER_TO_INTENSITY_FACTOR =
        54.3514f;
    // 1.0f;
    // TEMP_DEBUG -
    data.intensity /= PC_POINT_LIGHTS_BLENDER_POWER_TO_INTENSITY_FACTOR;
    data.type = Lights::PointLight;
    data.range = 50.0f;
    PC_WARN("POINT INTENSITY " << data.intensity)
    PC_WARN("POINT RANGE" << data.range << " " << gltfLight.range)
    PC_WARN("POINT LIGHT POS " << light->GetPosition().x << ", "
                               << light->GetPosition().y << ", "
                               << light->GetPosition().z << "\n ")
  } else if (gltfLight.type == "spot") {
    data.type = Lights::SpotLight;
  } else if (gltfLight.type == "directional") {
    data.type = Lights::DirectionalLight;
    // TEMP_DEBUG -
    constexpr float PC_DIR_LIGHTS_BLENDER_POWER_TO_INTENSITY_FACTOR =
        // 100.0f * 6.83f;
        100.0f;
    // 1.0f;
    data.intensity /= PC_DIR_LIGHTS_BLENDER_POWER_TO_INTENSITY_FACTOR;
    PC_WARN("DIR LIGHT INTENSITY " << data.intensity)
    PC_WARN("DIR LIGHT POS " << light->GetPosition().x << ", "
                             << light->GetPosition().y << ", "
                             << light->GetPosition().z << "\n ")
  } else {
    PC_WARN("Unknown light type: " << gltfLight.type);
    data.type = Lights::PointLight; // default fallback
  }

  light->SetLightData(data);
}

//
//
//
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// --- MESH UTILS --------------------------------------------------------------
//
void GltfLoader::ExtractMeshData(const tinygltf::Model &model,
                                 const tinygltf::Node &gltfNode, Mesh &mesh) {
  // MESH TYPE
  const auto &gltfMesh = model.meshes[gltfNode.mesh];

  // gltfMesh.primitives are submeshes
  for (const auto &primitive : gltfMesh.primitives) {
    VertexBuffer *vbo;
    IndexBuffer<uint32_t> *ibo;

    // Extract vertex buffer
    vbo = ExtractVertexBuffer(model, primitive);

    // Extract index buffer
    ibo = ExtractIndexBuffer(model, primitive);

    // struct VertexTemp {
    //   glm::vec3 pos;
    //   glm::vec3 normal;
    //   glm::vec2 uv;
    //   std::string Print() {
    //     std::stringstream ss;
    //     ss << pos.x << ", " << pos.y << ", " << pos.z << "; " << normal.r
    //        << ", " << normal.g << ", " << normal.b << "; " << uv.x << ", "
    //        << uv.y;
    //     return ss.str();
    //   };
    // };
    //
    // // Print vertex buffer
    // vbo->PrintBuffer<VertexTemp>();

    // const uint32_t *indices =
    //     reinterpret_cast<const uint32_t *>(ibo->GetBufferData());
    // std::cout << "Index Buffer: ";
    // for (size_t i = 0; i < ibo->GetCount(); ++i) {
    //   std::cout << indices[i];
    //   if (i < ibo->GetCount() - 1)
    //     std::cout << ", ";
    // }
    // std::cout << std::endl;

    // Extract material data
    if (primitive.material >= 0) {
      auto &gltfMaterial = model.materials[primitive.material];
      MaterialTypes matType = GetGltfMaterialType(gltfMaterial);

      switch (matType) {
      case MaterialTypes::PbrMat: {
        Material<MaterialTypes::PbrMat> *mat =
            new Material<MaterialTypes::PbrMat>();
        mat->SetData(
            ExtractMaterialData<MaterialTypes::PbrMat>(model, gltfMaterial));
        // mat->SetShader(ShaderStages::VertexBit,
        //                GfxContext::Shaders()
        //                    ->GetInbuiltShader<ShaderFiles::PbrMat_Vert>());
        // mat->SetShader(ShaderStages::FragmentBit,
        //                GfxContext::Shaders()
        //                    ->GetInbuiltShader<ShaderFiles::PbrMat_Frag>());
        mesh.AddSubmesh(vbo, ibo, mat);
        break;
      }
      case MaterialTypes::BasicMat: {
        Material<MaterialTypes::BasicMat> *mat =
            new Material<MaterialTypes::BasicMat>();
        mat->SetData(
            ExtractMaterialData<MaterialTypes::BasicMat>(model, gltfMaterial));
        // mat->SetShader(ShaderStages::VertexBit,
        //                GfxContext::Shaders()
        //                    ->GetInbuiltShader<ShaderFiles::BasicMat_Vert>());
        // mat->SetShader(ShaderStages::FragmentBit,
        //                GfxContext::Shaders()
        //                    ->GetInbuiltShader<ShaderFiles::BasicMat_Frag>());
        mesh.AddSubmesh(vbo, ibo, mat);
        break;
      }
      default:
        PC_ERROR("Wrong material type!", "GltfLoader")
      }
    } else {
      // Fallback to basic material
      Material<MaterialTypes::BasicMat> *mat =
          new Material<MaterialTypes::BasicMat>();
      mesh.AddSubmesh(vbo, ibo, mat);
    };
  }
};

//
//
//
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// --- MATERIAL UTILS ----------------------------------------------------------
//
template <MaterialTypes T>
DeriveMaterialDataType<T>::type
GltfLoader::ExtractMaterialData(const tinygltf::Model &model,
                                const tinygltf::Material &material) {
  typename DeriveMaterialDataType<T>::type materialData;
  const auto &gltfMatData = material.pbrMetallicRoughness;

  materialData.doubleSided = false;

  // Shaders mask
  materialData.enabledShadersMask =
      ShaderStages::VertexBit | ShaderStages::FragmentBit;

  if constexpr (T == MaterialTypes::PbrMat) {
    // Add shader byte code
    materialData.baseColorFactor = glm::vec4(
        gltfMatData.baseColorFactor[0], gltfMatData.baseColorFactor[1],
        gltfMatData.baseColorFactor[2], gltfMatData.baseColorFactor[3]);
    materialData.metallicFactor = gltfMatData.metallicFactor;
    materialData.roughnessFactor = gltfMatData.roughnessFactor;
    materialData.hasNormalTexture = material.normalTexture.index != -1;
    materialData.hasMetallicRoughnessTexture =
        gltfMatData.metallicRoughnessTexture.index != -1;
  } else {
    // DEFAULTS TO BASIC MATERIAL
    // Add shader byte code
    materialData.baseColorFactor = glm::vec4(
        gltfMatData.baseColorFactor[0], gltfMatData.baseColorFactor[1],
        gltfMatData.baseColorFactor[2], gltfMatData.baseColorFactor[3]);
  }

  // // Add shader files based on material type
  // data->shaderFiles =
  //     isPBR ? std::vector<const char *>{"pbr.vert", "pbr.frag"}
  //           : std::vector<const char *>{"basic.vert", "basic.frag"};
  //
  return materialData;
}

MaterialTypes
GltfLoader::GetGltfMaterialType(const tinygltf::Material &material) {
  const auto &pbr = material.pbrMetallicRoughness;

  // Determine material type
  bool isPBR = pbr.metallicFactor > 0.0f || pbr.roughnessFactor > 0.0f ||
               material.normalTexture.index != -1 ||
               material.occlusionTexture.index != -1;

  if (isPBR) {
    PC_PRINT("PBR Mat", TagType::Print, "GltfLoader.cpp")
    return MaterialTypes::PbrMat;
  } else {
    PC_PRINT("Basic Mat", TagType::Print, "GltfLoader.cpp")
    return MaterialTypes::BasicMat;
  }
}

//
// Random thought: Valora is a sick name for a videogame character
//

//
//
//
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// --- PRIMITIVE UTILS ---------------------------------------------------------
//

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
    // vertices->SetLayout<AttrTypes::Float3,    // Position
    //                     AttrTypes::Float3,    // Normal
    //                     AttrTypes::Float2>(); // UV

    vertices->SetLayout(GltfVertexBufferLayout);
  }

  for (size_t i = 0; i < vertexCount; ++i) {
    size_t baseOffset = i * totalAttrByteSize;
    size_t cpyOffset = 0;

    const byte_t *pos =
        reinterpret_cast<const byte_t *>(posData + i * posStride);
    vertices->Fill(pos, posByteSize, baseOffset + cpyOffset);
    cpyOffset += posByteSize;

    if (normData) {
      const byte_t *norm =
          reinterpret_cast<const byte_t *>(normData + i * normStride);
      vertices->Fill(norm, normByteSize, baseOffset + cpyOffset);
      cpyOffset += normByteSize;
    } else {
      const float defaultNormal[3] = {0, 0, 0};
      vertices->Fill(defaultNormal, normByteSize, baseOffset + cpyOffset);
      cpyOffset += normByteSize;
    }

    if (uvData) {
      const byte_t *uv =
          reinterpret_cast<const byte_t *>(uvData + i * uvStride);
      vertices->Fill(uv, uvByteSize, baseOffset + cpyOffset);
      cpyOffset += uvByteSize;
    } else {
      const float defaultUV[2] = {0, 0};
      vertices->Fill(defaultUV, uvByteSize, baseOffset + cpyOffset);
      cpyOffset += uvByteSize;
    }
  }

  return vertices;
}

IndexBuffer<uint32_t> *
GltfLoader::ExtractIndexBuffer(const tinygltf::Model &model,
                               const tinygltf::Primitive &primitive) {
  PC_ASSERT(primitive.indices >= 0, "Primitive has no indices");

  const auto &indexAccessor = model.accessors[primitive.indices];
  const size_t indexCount = indexAccessor.count;

  const auto &indexView = model.bufferViews[indexAccessor.bufferView];
  const auto &gltfIndexBuffer = model.buffers[indexView.buffer];

  const byte_t *srcData = gltfIndexBuffer.data.data() + indexView.byteOffset +
                          indexAccessor.byteOffset;

  // Handle stride (distance between indices in bytes)
  // size_t stride = indexView.byteStride;
  size_t stride = 0;

  // Allocate index buffer
  IndexBuffer<uint32_t> *indices = new IndexBuffer<uint32_t>();
  indices->Allocate(indexCount * sizeof(uint32_t));

  if (stride == 0) {
    // Default stride based on component type
    switch (indexAccessor.componentType) {
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
      stride = sizeof(uint8_t);
      break;
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

  // Convert indices to 32-bit
  for (size_t i = 0; i < indexCount; ++i) {
    const size_t offset = i * stride;
    const byte_t *src = srcData + offset;

    uint32_t indexValue = 0;
    switch (indexAccessor.componentType) {
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
      indexValue = *reinterpret_cast<const uint8_t *>(src);
      break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
      indexValue = *reinterpret_cast<const uint16_t *>(src);
      break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
      indexValue = *reinterpret_cast<const uint32_t *>(src);
      break;
    default:
      PC_ERROR("Unsupported index type - conversion", "GltfLoader");
      delete indices;
      return nullptr;
    }

    uint32_t *dst = reinterpret_cast<uint32_t *>(indices->GetBufferData());
    dst[i] = indexValue;
  }

  return indices;
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
