#include <fmt/format.h>
#include "World.hpp"

World::World() {
  Initialize();
}

void World::Initialize() {
  tick = 0;
  elapsedFromTick = 0.0;

  currentEventStep = 0;
  Event::Info *currentEvent = nullptr;

  resources[Resource::Peoples]  = 50;
  resources[Resource::Food]     = 50;
  resources[Resource::Oxygen]   = 100;
  resources[Resource::Minerals] = 30;
  resources[Resource::Gas]      = 0;
  resources[Resource::Science]  = 0;
  resources[Resource::DaysUntilEvacuation] = 10;

  buildings.clear();
  worldLog.clear();

  Generate();
  EmitEvent(Event::Type::Start);
  AddLog("Game has been started");
}

void World::Generate() {
  for(auto& tileRow : foundation) {
    for(auto& tile : tileRow) {
      tile = static_cast<Tile::Type>(Rand(3));
    }
  }
}

void World::CheckWinLose() {
  if(GetResource(Resource::DaysUntilEvacuation) <= 0) {
    EmitEvent(Event::Type::Win);
  }

  if(GetResource(Resource::Peoples) <= 0) {
    EmitEvent(Event::Type::Lose);
  }
}

void World::Update(float elapsed) {
  if(currentEvent != nullptr) {
    return;
  }

  elapsedFromTick += elapsed;
  if(elapsedFromTick >= 1000) {
    elapsedFromTick = 0.0;
    tick += 1;

    Tick();
  }
}

void World::Tick() {
  CheckWinLose();

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

int World::GetResource(Resource res) { return resources[res]; }
std::string World::GetResourceName(Resource res) { return resourceNames[res]; }
void World::SetResource(Resource res, int amount) {
  resources[res] = amount;
  if(resources[res] < 0) {
    resources[res] = 0;
  }
}

bool World::TryToBuild(Building::Type building, int x, int y) {
  if(x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
    return false;
  }

  foundation[x][y] = Building::Tiles[building];
  return true;
}

void World::EmitEvent(Event::Type type) {
  currentEventStep = 0;
  currentEvent = eventInfos[type];
}

void World::HandleStepEvent(int step) {
  if(currentEvent == nullptr) {
    return;
  }

  switch(currentEvent->type) {
  case Event::Type::Start:
    currentEvent = nullptr;
    break;
  case Event::Type::Win:
  case Event::Type::Lose:
    Initialize();
    break;
  default:
    break;
  }
}

std::vector<std::string>& World::GetLog() { return worldLog; }
void World::AddLog(const std::string &str) {
  worldLog.resize(15);
  worldLog.insert(worldLog.begin(), str);
}
