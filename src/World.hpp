#ifndef _WORLD_HPP_
  #define _WORLD_HPP_

#include <string>
#include <array>
#include <vector>
#include <map>

#include "SDL.h"
#include "Object.hpp"
#include "Resources.hpp"

class BaseBuilding;

class World : Object {
private:
  static const int SIZE = 8;

  std::array<std::array<int, SIZE>, SIZE> foundation = {0};

  std::map<Resource, int> resources;
  std::vector<BaseBuilding*> buildings;
  std::vector<std::string> worldLog;

  float elapsedFromTick = 0.0;
  Uint32 tick = 0;

  int daysUntilEvacuation = 0;
public:
  World();

  void Update(float elapsed) override;
  void Tick();

  int Rand(int a) { return rand() % a + 1; }
  int GetTick() { return tick; }
  bool Every(int tickPeriod) { return (tick % tickPeriod) == 0; }

  int GetResource(Resource res);
  void SetResource(Resource res, int amount);

  void AddLog(const std::string &str);

  int GetDaysUntilEvacuation();
  std::vector<std::string>& GetLog();
};

#endif
