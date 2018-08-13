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

  foundation[Rand(6)][Rand(6)] = Tile::Type::Biodome;
  foundation[Rand(6)][Rand(6)] = Tile::Type::OxygenTank;
  buildings.push_back(Building::Type::Biodome);
  buildings.push_back(Building::Type::OxygenTank);
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

  for(const auto& buildingType : buildings) {
    for(const auto& prod : buildingInfos[buildingType]->production) {
      if(Every(prod.second)) UpdateResource(prod.first, 5);
    }
  }

  if(Every(10) && Rand(10) > 8) {
    int randomX = Rand(8) - 1;
    int randomY = Rand(8) - 1;
    auto tile = foundation[randomX][randomY];
    if(Building::ReverseTiles[tile] != Building::Type::Null) {
      buildings.erase(std::find(buildings.begin(), buildings.end(), Building::ReverseTiles[tile]));
      if(Building::ReverseTiles[tile] == Building::Type::OxygenTank) {
        SetResource(Resource::Oxygen, GetResource(Resource::Oxygen));
      }
    }

    foundation[randomX][randomY] = Tile::Type::Null;
    AddLog("Oh no, another one piece of ground has been fall");
  }

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
void World::UpdateResource(Resource res, int amount) { SetResource(res, GetResource(res) + amount); }
void World::SetResource(Resource res, int amount) {
  resources[res] = amount;
  if(resources[res] < 0) {
    resources[res] = 0;
  }

  if(res == Resource::Oxygen) {
    int maxOxygen = 1000 * std::count (buildings.begin(), buildings.end(), Building::Type::OxygenTank);
    if(resources[Resource::Oxygen] > maxOxygen) {
      resources[Resource::Oxygen] = maxOxygen;
    }
  }
}

bool World::TryToBuild(Building::Type building, int x, int y) {
  if(x < 0 || x >= SIZE || y < 0 || y >= SIZE) return false;
  if(foundation[x][y] == Tile::Type::Null) return false;

  auto tiles = buildingInfos[building]->placementTiles;
  if(!tiles.empty()) {
    auto found = std::find(std::begin(tiles), std::end(tiles), foundation[x][y]);
    if(found == std::end(tiles)) {
      AddLog("Can't build on this tile");
      return false;
    }
  }

  auto cost = buildingInfos[building]->cost;
  for(const auto& res : cost) {
    if(GetResource(res.first) < res.second) {
      AddLog(fmt::format("Insufficient {}", GetResourceName(res.first)));
      return false;
    }
  }

  for(const auto& res : cost) UpdateResource(res.first, -res.second);
  buildings.push_back(building);
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

std::string World::GetStatus() {
  return fmt::format(
    "{}:00, Day {}, {} days until evacuation",
    std::floor((tick % DAY_DURATION) / (DAY_DURATION / 24.0)),
    tick / DAY_DURATION + 1,
    GetResource(Resource::DaysUntilEvacuation)
  );
}

std::vector<std::string>& World::GetLog() { return worldLog; }
void World::AddLog(const std::string &str) {
  worldLog.resize(15);
  worldLog.insert(worldLog.begin(), str);
}
