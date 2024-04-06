#include "Engine.h"
#include <iostream>

Engine *Engine::instance = nullptr;

Engine *Engine::Get() { return instance; }

void Engine::Create(const char *dir) {
  std::cout << "DIR: " << dir << "\n";
  std::cout << "BEFORE:" << instance << "\n";
  if (!instance) {
    instance = new Engine();
  }
  std::cout << "AFTER:" << instance << "\n";
}

void Engine::Destroy() {
  if (instance) {
    delete instance;
    instance = nullptr;
  }
}

Engine::Engine() { std::cout << "ENGINE STARTED" << "\n"; };
Engine::~Engine() { std::cout << "ENGINE STOPPED" << "\n"; };
