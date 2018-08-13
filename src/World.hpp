#ifndef _WORLD_HPP_
  #define _WORLD_HPP_

#include <string>
#include <array>
#include <vector>
#include <map>

#include <SDL2pp/Rect.hh>
#include "Object.hpp"

namespace Tile {
  enum class Type {
    Null,
    Ground,
    Minerals,
    Gas,
    Biodome,
    OxygenTank,
    HarvestStation,
    Refinery,
    ScienceLab
  };

  static std::map<Type, SDL2pp::Rect> Tiles{
    {Type::Null,           SDL2pp::Rect(0, 0, 0, 0)},

    {Type::Ground,         SDL2pp::Rect(0 * 64, 0 * 64, 64, 64)},
    {Type::Minerals,       SDL2pp::Rect(1 * 64, 0 * 64, 64, 64)},
    {Type::Gas,            SDL2pp::Rect(2 * 64, 0 * 64, 64, 64)},

    {Type::Biodome,        SDL2pp::Rect(0 * 64, 1 * 64, 64, 64)},
    {Type::OxygenTank,     SDL2pp::Rect(3 * 64, 1 * 64, 64, 64)},
    {Type::HarvestStation, SDL2pp::Rect(1 * 64, 1 * 64, 64, 64)},
    {Type::Refinery,       SDL2pp::Rect(2 * 64, 1 * 64, 64, 64)},
    {Type::ScienceLab,     SDL2pp::Rect(4 * 64, 1 * 64, 64, 64)},
  };
};

enum class Resource {
  Null,
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
    Null,
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
    std::vector<Tile::Type> placementTiles;
  };

  static std::map<Type, Tile::Type> Tiles{
    {Type::Null,           Tile::Type::Null},
    {Type::Biodome,        Tile::Type::Biodome},
    {Type::OxygenTank,     Tile::Type::OxygenTank},
    {Type::HarvestStation, Tile::Type::HarvestStation},
    {Type::Refinery,       Tile::Type::Refinery},
    {Type::ScienceLab,     Tile::Type::ScienceLab},
  };

  static std::map<Tile::Type, Type> ReverseTiles{
    {Tile::Type::Null,           Type::Null},

    {Tile::Type::Ground,         Type::Null},
    {Tile::Type::Minerals,       Type::Null},
    {Tile::Type::Gas,            Type::Null},

    {Tile::Type::Biodome,        Type::Biodome},
    {Tile::Type::OxygenTank,     Type::OxygenTank},
    {Tile::Type::HarvestStation, Type::HarvestStation},
    {Tile::Type::Refinery,       Type::Refinery},
    {Tile::Type::ScienceLab,     Type::ScienceLab},
  };
};

namespace Event {
  enum class Type {
    Null,
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
  static const int DAY_DURATION = 60;
  static const int OXYGEN_TANK_CAPACITY = 1000;

  std::array<std::array<Tile::Type, SIZE>, SIZE> foundation = {Tile::Type::Null};

  std::map<Resource, int> resources;
  std::map<Resource, int> totalResources;

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
    .production = std::map<Resource, int>{ {Resource::Food, 10}, {Resource::Oxygen, 5} },
    .placementTiles = std::vector<Tile::Type>{}
  };

  Building::Info oxygenTankInfo = Building::Info{
    .type = Building::Type::OxygenTank,
    .name = "Oxygen Tank",
    .description = "Can be placed on any tile. Stores oxygen inside.",
    .cost = std::map<Resource, int>{ {Resource::Minerals, 30} },
    .production = std::map<Resource, int>{},
    .placementTiles = std::vector<Tile::Type>{}
  };

  Building::Info harvestStationInfo = Building::Info{
    .type = Building::Type::HarvestStation,
    .name = "Harvest Station",
    .description = "Can be placed only on mineral tile. Mines minerals.",
    .cost = std::map<Resource, int>{ {Resource::Minerals, 10} },
    .production = std::map<Resource, int>{ {Resource::Minerals, 5} },
    .placementTiles = std::vector<Tile::Type>{ Tile::Type::Minerals }
  };

  Building::Info refineryInfo = Building::Info{
    .type = Building::Type::Refinery,
    .name = "Refinery",
    .description = "Can be placed only on geyser tile. Refines gas.",
    .cost = std::map<Resource, int>{ {Resource::Minerals, 50} },
    .production = std::map<Resource, int>{ {Resource::Gas, 15} },
    .placementTiles = std::vector<Tile::Type>{ Tile::Type::Gas }
  };

  Building::Info scienceLabInfo = Building::Info{
    .type = Building::Type::ScienceLab,
    .name = "Science Laboratory",
    .description = "Can be placed on any tile. Produces scientific data.",
    .cost = std::map<Resource, int>{ {Resource::Minerals, 50}, {Resource::Gas, 20} },
    .production = std::map<Resource, int>{ {Resource::Science, 20} },
    .placementTiles = std::vector<Tile::Type>{}
  };

  std::map<Building::Type, Building::Info*> buildingInfos = {
    {Building::Type::Biodome, &biodomeInfo},
    {Building::Type::OxygenTank, &oxygenTankInfo},
    {Building::Type::HarvestStation, &harvestStationInfo},
    {Building::Type::Refinery, &refineryInfo},
    {Building::Type::ScienceLab, &scienceLabInfo}
  };

  Event::Info startEvent = Event::Info{
    .type = Event::Type::Start,
    .steps = std::map<int, Event::Step>{
      {0, Event::Step{
        .text = "You are the head of research group, based on asteroid belt. Finally you and your group reached perspective asteroid, and prepared to set up camp. Maybe you made a mistake in the calculations or something else happened, but the surface could not stand the landing of you ship. You have a few resource, some scientific equipment, but land is actually falls down. Help will arrive in about 10 days. Can you survive and save your crew and scientific data? You have to build buildings, gather resources and make decisions, but remember, at any moment everything can collapse.\n\n(Use alpha keys to select answer)",
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
        .text = "Congratulations, you won! Help is finally arrived, people grabs their research data and climbs to the rescue drones.\n\n{} days on asteroid\n{} peoples rescued\n{} minerals extracted\n{} gas refined\n{} scientific data gathered",
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
        .text = "Sorry, but there are no people left. You lost.\n\n{} days on asteroid\n{} peoples rescued\n{} minerals extracted\n{} gas refined\n{} scientific data gathered",
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

  SDL2pp::Rect GetTile(Tile::Type type) const { return Tile::Tiles[type]; }
  SDL2pp::Rect GetTile(Building::Type type) const { return Tile::Tiles[Building::Tiles[type]]; }
  std::array<std::array<Tile::Type, SIZE>, SIZE>& GetFoundation() { return foundation; }

  int GetResource(Resource res);
  void SetResource(Resource res, int amount);
  void UpdateResource(Resource res, int amount);
  std::string GetResourceName(Resource res);

  bool TryToBuild(Building::Type building, int x, int y);
  void RemoveBuilding(int x, int y);

  bool HasEvent() const { return (currentEvent != nullptr); }
  void EmitEvent(Event::Type type);
  void HandleStepEvent(int step);
  std::string GetEventText();
  int GetCurrentEventStep() const { return currentEventStep; }
  Event::Info* GetCurrentEvent() const { return currentEvent; }

  std::string GetStatus();
  std::vector<std::string>& GetLog();
  void AddLog(const std::string &str);

  std::map<Building::Type, Building::Info*>& GetBuildingInfos() { return buildingInfos; }

  int Rand(int a) const { return rand() % a + 1; }
  bool Every(int tickPeriod) const { return (tick % tickPeriod) == 0; }
};

#endif
