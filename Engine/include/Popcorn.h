#pragma once

#include "Application.h"
#include "Assert.h"
#include "GlobalMacros.h"
#include "GltfLoader.h"
#include "Layer.h"
#include "Scene.h"

ENGINE_NAMESPACE_BEGIN
CTX_NAMESPACE_BEGIN

static Renderer *s_renderer = nullptr;
static Application *s_application = nullptr;

using Model = tinygltf::Model;

static void BeginContext() {
  Application::Start();
  s_application = &Application::Get();
  s_renderer = Renderer::Create<RendererType::Vulkan>(
      s_application
          ->GetAppWindow()); // This creates vulkan renderer, creates render
                             // command buffers, and creates renderflows

  s_application->SetRenderer(*s_renderer);
}

static void AddLayer(Layer *layer) {
  PC_ASSERT(layer, "Layer is nullptr");
  Application::AddLayer(layer);
};

static void RegisterScene(Scene &scene) { s_renderer->AddScene(&scene); };
static void DisposeScene(Scene &scene) { s_renderer->RemoveScene(&scene); };

static void StartGame() {
  // TODO: Move the ownership of commandbuffers to Renderflow base class
  s_renderer->CreateRenderFlows();
  s_renderer->PrepareRenderFlows(); // Creates attachments, renderflows,
                                    // framebuffers, and command buffers
  s_renderer->AssignSceneObjectsToRenderFlows(); // Sorts submeshes material
                                                 // wise & adds to renderflows
  s_renderer->CreateRenderFlowResources(); // Renderflow submeshes converted and
                                           // copied to vulkan memory objects
  s_application->StartGameLoop();          // Starts game loop
};

static void RenderScenes(Scene &scene) { s_renderer->DrawFrame(scene); };

static void EndContext() {
  s_renderer->DestroyRenderFlows();
  s_renderer = nullptr;
  Application::Stop();
}

// static Renderer &GetRenderer() {
//   if (s_renderer == nullptr) {
//     PC_WARN("s_renderer is nullptr")
//   };
//   return *s_renderer;
// };
//
// static Application &GetApplication() {
//   if (s_renderer == nullptr) {
//     PC_WARN("s_s_application is nullptr")
//   };
//   return *s_application;
// };

static void ConvertGltfToScene(const std::string &filename, Scene &scene) {
  tinygltf::Model model;
  GltfLoader::LoadFromFile(filename, model);
  GltfLoader::ExtractModelData(model, scene.GetGameObjects());
};

CTX_NAMESPACE_END
ENGINE_NAMESPACE_END
