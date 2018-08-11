#include <sstream>
#include "World.hpp"

World::World() {
  daysUntilEvacuation = 10;

  resources[Resource::Peoples]  = 50;
  resources[Resource::Food]     = 50;
  resources[Resource::Oxygen]   = 100;
  resources[Resource::Minerals] = 30;
  resources[Resource::Gas]      = 0;
  resources[Resource::Science]  = 0;

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
  std::ostringstream logItem;

  int currentPeoples = GetResource(Resource::Peoples);
  int currentFood    = GetResource(Resource::Food);

  int newPeoples = currentPeoples;
  int newFood    = currentFood;

  // Advance day
  if(Every(12)) {
    daysUntilEvacuation -= 1;

    logItem.str("");
    logItem << "Another day has been started. Evacuation ETA "
            << daysUntilEvacuation
            << " days";
    AddLog(logItem.str());
  }

  // Food
  if(Every(12)) {
    newFood -= currentPeoples;
    logItem.str("");
    logItem << currentPeoples
            << " food was eaten";
    AddLog(logItem.str());

    if(currentFood < currentPeoples) {
      int foodDeficit = currentPeoples - currentFood;
      int deadPeoples = Rand(foodDeficit);

      newPeoples -= deadPeoples;
      logItem.str("");
      logItem << deadPeoples
              << " peoples died from starvation";
      AddLog(logItem.str());
    }
  }

  SetResource(Resource::Peoples, newPeoples);
  SetResource(Resource::Food, newFood);
}

void World::SetResource(Resource res, int amount) {
  resources[res] = amount;
  if(resources[res] < 0) {
    resources[res] = 0;
  }
}

void World::AddLog(const std::string &str) {
  worldLog.resize(15);
  worldLog.insert(worldLog.begin(), str);
}

int World::GetResource(Resource res) { return resources[res]; }
int World::GetDaysUntilEvacuation() { return daysUntilEvacuation; }
std::vector<std::string>& World::GetLog() { return worldLog; }
