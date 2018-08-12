#ifndef _WORLD_HPP_
  #define _WORLD_HPP_

#include <string>
#include <array>
#include <vector>
#include <map>

#include <SDL2pp/Rect.hh>
#include "Object.hpp"

enum class Resource {
  Peoples,
  Food,
  Oxygen,
  Minerals,
  Gas,
  Science,
  DaysUntilEvacuation,
};

namespace Building {
  enum class Type {
    Biodome,
    OxygenTank,
    HarvestStation,
    Refinery,
    ScienceLab
  };

  struct Info {
    Type type;
    std::string name;
    std::string description;
    std::map<Resource, int> cost;
    std::map<Resource, int> production;
    SDL2pp::Rect tileRect;
  };
};

namespace Event {
  enum class Type {
    Start,
    Win,
    Lose
  };

  struct Step {
    std::string text;
    std::map<int, std::string> choices;
  };

  struct Info {
    Type type;
    std::map<int, Step> steps;
  };
}

class World : Object {
private:
  static const int SIZE = 8;
  static const int DAY_DURATION = 120;

  std::array<std::array<int, SIZE>, SIZE> foundation = {0};

  std::map<Resource, int> resources;
  std::vector<Building::Type> buildings;
  std::vector<std::string> worldLog;

  int tick = 0;
  float elapsedFromTick = 0.0;

  int currentEventStep = 0;
  Event::Info *currentEvent = nullptr;

  std::map<Resource, std::string> resourceNames = {
    {Resource::Peoples, "peoples"},
    {Resource::Food, "food"},
    {Resource::Oxygen, "oxygen"},
    {Resource::Minerals, "minerals"},
    {Resource::Gas, "gas"},
    {Resource::Science, "science"},
    {Resource::DaysUntilEvacuation, "days until evacuation"},
  };

  Building::Info biodomeInfo = Building::Info{
    .type = Building::Type::Biodome,
    .name = "Biodome",
    .description = "Can be placed on any tile. Produces food and oxygen.",
    .cost = std::map<Resource, int>{ {Resource::Minerals, 30}, {Resource::Gas, 10} },
    .production = std::map<Resource, int>{ {Resource::Food, 25}, {Resource::Oxygen, 100} },
    .tileRect = SDL2pp::Rect(0 * 64, 0 * 64, 64, 64)
  };

  Building::Info oxygenTankInfo = Building::Info{
    .type = Building::Type::OxygenTank,
    .name = "Oxygen Tank",
    .description = "Can be placed on any tile. Stores oxygen inside.",
    .cost = std::map<Resource, int>{ {Resource::Minerals, 30} },
    .production = std::map<Resource, int>{},
    .tileRect = SDL2pp::Rect(1 * 64, 0 * 64, 64, 64)
  };

  Building::Info harvestStationInfo = Building::Info{
    .type = Building::Type::HarvestStation,
    .name = "Harvest Station",
    .description = "Can be placed only on mineral tile. Mines minerals.",
    .cost = std::map<Resource, int>{ {Resource::Minerals, 10} },
    .production = std::map<Resource, int>{ {Resource::Minerals, 100} },
    .tileRect = SDL2pp::Rect(2 * 64, 0 * 64, 64, 64)
  };

  Building::Info refineryInfo = Building::Info{
    .type = Building::Type::Refinery,
    .name = "Refinery",
    .description = "Can be placed only on geyser tile. Refines gas.",
    .cost = std::map<Resource, int>{ {Resource::Minerals, 50} },
    .production = std::map<Resource, int>{ {Resource::Gas, 50} },
    .tileRect = SDL2pp::Rect(3 * 64, 0 * 64, 64, 64)
  };

  Building::Info scienceLabInfo = Building::Info{
    .type = Building::Type::ScienceLab,
    .name = "Science Laboratory",
    .description = "Can be placed on any tile. Produces scientific data.",
    .cost = std::map<Resource, int>{ {Resource::Minerals, 50}, {Resource::Gas, 20} },
    .production = std::map<Resource, int>{ {Resource::Science, 50} },
    .tileRect = SDL2pp::Rect(4 * 64, 0 * 64, 64, 64)
  };

  std::map<Building::Type, Building::Info> buildingInfos = {
    {Building::Type::Biodome, biodomeInfo},
    {Building::Type::OxygenTank, oxygenTankInfo},
    {Building::Type::HarvestStation, harvestStationInfo},
    {Building::Type::Refinery, refineryInfo},
    {Building::Type::ScienceLab, scienceLabInfo}
  };

  Event::Info startEvent = Event::Info{
    .type = Event::Type::Start,
    .steps = std::map<int, Event::Step>{
      {0, Event::Step{
        .text = "You are head of research.",
        .choices = std::map<int, std::string>{
          {1, "Start game"}
        }
      }}
    }
  };

  Event::Info winEvent = Event::Info{
    .type = Event::Type::Win,
    .steps = std::map<int, Event::Step>{
      {0, Event::Step{
        .text = "Congratulations! You win.",
        .choices = std::map<int, std::string>{
          {1, "Restart game"}
        }
      }}
    }
  };

  Event::Info loseEvent = Event::Info{
    .type = Event::Type::Lose,
    .steps = std::map<int, Event::Step>{
      {0, Event::Step{
        .text = "Sorry but you lost.",
        .choices = std::map<int, std::string>{
          {1, "Restart game"}
        }
      }}
    }
  };

  std::map<Event::Type, Event::Info*> eventInfos = {
    {Event::Type::Start, &startEvent},
    {Event::Type::Win, &winEvent},
    {Event::Type::Lose, &loseEvent},
  };
public:
  World();

  void Initialize();
  void Generate();
  void CheckWinLose();

  void Update(float elapsed) override;
  void Tick();

  std::array<std::array<int, SIZE>, SIZE>& GetFoundation() { return foundation; }

  int GetResource(Resource res);
  void SetResource(Resource res, int amount);
  std::string GetResourceName(Resource res);

  void EmitEvent(Event::Type type);
  void HandleStepEvent(int step);
  int GetCurrentEventStep() const { return currentEventStep; }
  Event::Info* GetCurrentEvent() const { return currentEvent; }

  std::vector<std::string>& GetLog();
  void AddLog(const std::string &str);

  std::map<Building::Type, Building::Info>& GetBuildingInfos() { return buildingInfos; }

  int Rand(int a) const { return rand() % a + 1; }
  bool Every(int tickPeriod) const { return (tick % tickPeriod) == 0; }
};

#endif
