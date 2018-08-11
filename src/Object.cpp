#include "Game.hpp"
#include "Object.hpp"

Object::Object() {
  Game::Instance()->AddObject(this);
}

Object::~Object() {
  Game::Instance()->RemoveObject(this);
}

void Object::Update(float elapsed) {
}
