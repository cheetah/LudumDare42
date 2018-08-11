#ifndef _WORLD_HPP_
  #define _WORLD_HPP_

#include <iostream>
#include <array>
#include <vector>

#include "SDL.h"
#include "Object.hpp"
#include "Resources.hpp"

class BaseBuilding;

class World : Object {
private:
  static const int SIZE = 10;

  std::array<std::array<int, SIZE>, SIZE> foundation = {0};

  std::vector<Resource::BaseResource*> resources;
  std::vector<BaseBuilding*> buildings;

  float elapsedFromTick = 0.0;
  Uint32 tick = 0;

  Resource::Peoples peoples;
  Resource::Food food;
  Resource::Oxygen oxygen;
  Resource::Minerals minerals;
  Resource::Gas gas;
  Resource::ScientificData scientificData;
public:
  World();

  void Update(float elapsed) override;
  void Tick();

  int Rand(int a) { return rand() % a + 1; }
  int GetTick() { return tick; }
  bool Every(int tickPeriod) { return (tick % tickPeriod) == 0; }

  template<class T> void AddResource(T *resource) {
    resources.push_back(resource);
  }

  template<class T> T* GetResource() {
    for(Resource::BaseResource *res : resources) {
      if(typeid(*res) == typeid(T)) {
        return dynamic_cast<T*>(res);
      }
    }

    return nullptr;
  }
};

#endif
