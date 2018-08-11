#include "World.hpp"

World::World() {
  AddResource<Resource::Peoples>(&peoples);
  AddResource<Resource::Food>(&food);
  AddResource<Resource::Oxygen>(&oxygen);
  AddResource<Resource::Minerals>(&minerals);
  AddResource<Resource::Gas>(&gas);
  AddResource<Resource::ScientificData>(&scientificData);
}

void World::Update(float elapsed) {
  elapsedFromTick += elapsed;
  if(elapsedFromTick >= 1000) {
    elapsedFromTick = 0.0;
    tick += 1;

    Tick();
  }
}

void World::Tick() {
  for(Resource::BaseResource *res : resources) {
    res->Tick(this);
  }

  // std::cout << "Tick was called" << std::endl;
}
