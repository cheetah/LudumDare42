#include <iostream>
#include "World.hpp"
#include "Resources.hpp"

namespace Resource {

void Peoples::Tick(World *world) {
  if(world->Every(10)) {
    world->GetResource<Food>()->Sub(amount);
    std::cout << "Food eaten: " << amount << std::endl;

    if(world->GetResource<Food>()->GetAmount() < amount) {
      int foodDeficit = amount - world->GetResource<Food>()->GetAmount();
      int deadPeople = world->Rand(foodDeficit);
      Sub(deadPeople);

      std::cout << "Peoples died: " << deadPeople << std::endl;
    }
  }
}

};
