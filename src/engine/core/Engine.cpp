#include "Engine.h"
#include <iostream>

Engine::Engine() {}
Engine::~Engine() {}

Engine *Engine::Get() { std::cout << "ENGINE ACTIVE" << "\n"; }
