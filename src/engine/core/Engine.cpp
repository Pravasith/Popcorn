#include "Engine.h"
#include <iostream>

Engine *Engine::instance = nullptr;

Engine *Engine::Get() { return instance; }

void Engine::Start() {
  if (!instance) {
    instance = new Engine();
  }
}

void Engine::Stop() {
  if (instance) {
    delete instance;
    instance = nullptr;
  }
}

Engine::Engine() { std::cout << "ENGINE STARTED" << "\n"; };
Engine::~Engine() { std::cout << "ENGINE STOPPED" << "\n"; };
