#pragma once

#include "Application.h"
#include "GlobalMacros.h"
#include "Popcorn/Loaders/GltfLoader.h"
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

static void StartGame() {
  s_application->ProcessScenes(); // Loads models, converts to game objects
  s_application->StartGameLoop(); // Starts game loop
};

static void EndContext() {
  s_renderer = nullptr;
  Application::Stop();
}

static Renderer &GetRenderer() {
  if (s_renderer == nullptr) {
    PC_WARN("s_renderer is nullptr")
  };
  return *s_renderer;
};

static Application &GetApplication() {
  if (s_renderer == nullptr) {
    PC_WARN("s_s_application is nullptr")
  };
  return *s_application;
};

static void AddGltfToScene(const std::string &filename, Scene &scene) {
  tinygltf::Model model;
  GltfLoader::LoadFromFile(filename, model);
  GltfLoader::ExtractModelData(model, scene.GetGameObjects());
};

CTX_NAMESPACE_END
ENGINE_NAMESPACE_END
