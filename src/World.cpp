#include <fmt/format.h>
#include "World.hpp"

World::World() {
  resources[Resource::Peoples]  = 50;
  resources[Resource::Food]     = 50;
  resources[Resource::Oxygen]   = 100;
  resources[Resource::Minerals] = 30;
  resources[Resource::Gas]      = 0;
  resources[Resource::Science]  = 0;
  resources[Resource::DaysUntilEvacuation] = 10;

  Generate();

  AddLog("Game has been started");
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
  AddLog(fmt::format("Tick {} started", tick));

  int currentPeoples = GetResource(Resource::Peoples);
  int currentFood    = GetResource(Resource::Food);
  int currentOxygen  = GetResource(Resource::Oxygen);

  int newPeoples = currentPeoples;
  int newFood    = currentFood;
  int newOxygen  = currentOxygen;

  // Advance day
  if(Every(DAY_DURATION)) {
    int currentDays = GetResource(Resource::DaysUntilEvacuation);
    SetResource(Resource::DaysUntilEvacuation, currentDays - 1);
    AddLog(fmt::format("Another day has been started. Evacuation ETA {} days", currentDays));
  }

  // Food
  if(Every(DAY_DURATION)) {
    newFood -= currentPeoples;
    AddLog(fmt::format("{} food was eaten", currentPeoples));

    if(currentFood < currentPeoples) {
      int foodDeficit = currentPeoples - currentFood;
      int deadPeoples = Rand(foodDeficit);

      newPeoples -= deadPeoples;
      AddLog(fmt::format("{} peoples died from starvation", deadPeoples));
    }
  }

  // Oxygen
  if(Every(DAY_DURATION / 10)) {
    newOxygen -= currentPeoples;

    if(currentOxygen < currentPeoples) {
      int oxygenDeficit = currentPeoples - currentOxygen;
      int deadPeoples = Rand(oxygenDeficit);

      newPeoples -= deadPeoples;
      AddLog(fmt::format("{} peoples died from suffocation", deadPeoples));
    }
  }

  SetResource(Resource::Peoples, newPeoples);
  SetResource(Resource::Food, newFood);
  SetResource(Resource::Oxygen, newOxygen);
}

void World::Generate() {
  for(auto& tileRow : foundation) {
    for(auto& tile : tileRow) {
      tile = Rand(6);
    }
  }
}

int World::GetResource(Resource res) { return resources[res]; }
std::string World::GetResourceName(Resource res) { return resourceNames[res]; }
void World::SetResource(Resource res, int amount) {
  resources[res] = amount;
  if(resources[res] < 0) {
    resources[res] = 0;
  }
}

std::vector<std::string>& World::GetLog() { return worldLog; }
void World::AddLog(const std::string &str) {
  worldLog.resize(15);
  worldLog.insert(worldLog.begin(), str);
}
