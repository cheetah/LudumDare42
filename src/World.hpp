#ifndef _WORLD_HPP_
  #define _WORLD_HPP_

#include <string>
#include <array>
#include <vector>
#include <map>

#include <SDL2pp/Rect.hh>
#include <SDL2pp/Color.hh>
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
  Tiles,
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
    Lose,
    Magnetic,
    Toxic,
    Flare,
    Radio,
    SpoiledFood,
    StarStorm,
    GasFood,
    Newcomers,
    Landfall,
  };

  struct Step {
    std::string text;
    std::map<Resource, int> diff;
    bool ignoreCheck;
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

  SDL2pp::Color NORMAL_TEXT   = SDL2pp::Color(0, 0, 0);
  SDL2pp::Color DISABLED_TEXT = SDL2pp::Color(90, 90, 90);

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
    {Resource::Peoples, "people"},
    {Resource::Food, "food"},
    {Resource::Oxygen, "oxygen"},
    {Resource::Minerals, "minerals"},
    {Resource::Gas, "gas"},
    {Resource::Science, "science"},
    {Resource::DaysUntilEvacuation, "days until evacuation"},
    {Resource::Tiles, "tiles"},
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
    .description = "Can be placed only on mineral tile. Extracts minerals.",
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
        .text = "You are the head of research group, based on asteroid belt. Finally you and your group reached perspective asteroid, and prepared to set up camp. Maybe you made a mistake in the calculations or something else happened, but the surface could not stand the landing of you ship. You have a few resource, some scientific equipment, but land is literally falls down. Help will arrive in about 10 days. Can you survive and save your crew and scientific data? You have to build buildings, gather resources and make decisions, but remember, at any moment everything can collapse.\n\n(Use alpha keys to select answer)",
        .diff = std::map<Resource, int>{},
        .choices = std::map<int, std::string>{
          {-1, "Start game"}
        }
      }}
    }
  };

  Event::Info winEvent = Event::Info{
    .type = Event::Type::Win,
    .steps = std::map<int, Event::Step>{
      {0, Event::Step{
        .text = "Congratulations, you won! Help is finally arrived, people grabs their research data and climbs to the rescue drones.\n\n{} days on asteroid\n{} people rescued\n{} minerals extracted\n{} gas refined\n{} scientific data gathered",
        .diff = std::map<Resource, int>{},
        .ignoreCheck = false,
        .choices = std::map<int, std::string>{
          {-1, "Restart game"}
        }
      }}
    }
  };

  Event::Info loseEvent = Event::Info{
    .type = Event::Type::Lose,
    .steps = std::map<int, Event::Step>{
      {0, Event::Step{
        .text = "Sorry, but there are no people left. You lost.\n\n{} days on asteroid\n{} people rescued\n{} minerals extracted\n{} gas refined\n{} scientific data gathered",
        .diff = std::map<Resource, int>{},
        .ignoreCheck = false,
        .choices = std::map<int, std::string>{
          {-1, "Restart game"}
        }
      }}
    }
  };

  Event::Info magneticEvent = Event::Info{
    .type = Event::Type::Magnetic,
    .steps = std::map<int, Event::Step>{
      {
        0,
        Event::Step{
          .text = "You are getting closer to an asteroid with huge magnetic field. There is a threat that an asteroid will destroy part of your colony.",
          .diff = std::map<Resource, int>{},
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {1, "Make calculations and try to neutralize the effect of the magnetic field (-10 science)"},
            {2, "Reinforce buildings (-20 minerals)"},
            {3, "Ignore the threat"},
          }
        }
      }, {
        1,
        Event::Step{
          .text = "Calculations were made correctly and the threat was over. Also, you have a chance to collect some gas. (+20 gas)",
          .diff = std::map<Resource, int>{ {Resource::Science, -10}, {Resource::Gas, 20} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }, {
        2,
        Event::Step{
          .text = "Your team did a good job. Danger has passed.",
          .diff = std::map<Resource, int>{ {Resource::Minerals, -20} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }, {
        3,
        Event::Step{
          .text = "Unfortunately large part of your camp is destroyed.",
          .diff = std::map<Resource, int>{ {Resource::Tiles, -20} },
          .ignoreCheck = true,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }
    }
  };

  Event::Info toxicEvent = Event::Info{
    .type = Event::Type::Toxic,
    .steps = std::map<int, Event::Step>{
      {
        0,
        Event::Step{
          .text = "It looks like toxic fallout starts soon. Buildings and scientific equipment can be damaged.",
          .diff = std::map<Resource, int>{},
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {1, "Protect buildings (-10 minerals -5 science)"},
            {2, "Construct a collector to refine toxic substances (-50 minerals)"},
            {3, "Ignore"}
          }
        }
      }, {
        1,
        Event::Step{
          .text = "It worked. All your stuff are safe.",
          .diff = std::map<Resource, int>{ {Resource::Minerals, -10}, {Resource::Science, -5} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }, {
        2,
        Event::Step{
          .text = "Excellent work, you collected 70 gas.",
          .diff = std::map<Resource, int>{ {Resource::Minerals, -50}, {Resource::Gas, 70} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }, {
        3,
        Event::Step{
          .text = "Toxic fallout caused leak in oxygen tanks. You lost 700 oxygen.",
          .diff = std::map<Resource, int>{ {Resource::Oxygen, -700} },
          .ignoreCheck = true,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }
    }
  };

  Event::Info flareEvent = Event::Info{
    .type = Event::Type::Flare,
    .steps = std::map<int, Event::Step>{
      {
        0,
        Event::Step{
          .text = "Our scientists expecting solar flare. High temperature can be dangerous for our camp.",
          .diff = std::map<Resource, int>{},
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {1, "Construct heat shield (-50 minerals)"},
            {2, "Reconfigure life support systems to compensate high temperature (-10 science)"},
            {3, "Ignore"}
          }
        }
      }, {
        1,
        Event::Step{
          .text = "You construct a heat shield and survive a solar flare without incident.",
          .diff = std::map<Resource, int>{ {Resource::Minerals, -50} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }, {
        2,
        Event::Step{
          .text = "Life support systems compensates high temperature.",
          .diff = std::map<Resource, int>{ {Resource::Science, -10} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }, {
        3,
        Event::Step{
          .text = "High temperature provokes gas explosion. You lost 50 gas and 6 people were killed by the explosion.",
          .diff = std::map<Resource, int>{ {Resource::Gas, -50}, {Resource::Peoples, -6} },
          .ignoreCheck = true,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }
    }
  };

  Event::Info radioEvent = Event::Info{
    .type = Event::Type::Radio,
    .steps = std::map<int, Event::Step>{
      {
        0,
        Event::Step{
          .text = "One of your scientists offers to construct a transmitter. He says that transmitter will help rescue mission to find our position and will reduce their arrival time.",
          .diff = std::map<Resource, int>{},
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {1, "Agree with him (-30 minerals, -30 gas, -15 science)"},
            {2, "Ignore him"},
            {3, "Explain that construction is unable due to lack of resources."}
          }
        }
      }, {
        1,
        Event::Step{
          .text = "You are starting communication session with rescue mission by new transmitter. They are very happy to hear you, and promises to reach you as soon as possible. (-3 days until eacuation)",
          .diff = std::map<Resource, int>{ {Resource::Minerals, -30}, {Resource::Gas, -30}, {Resource::Science, -15}, {Resource::DaysUntilEvacuation, -3} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }, {
        2,
        Event::Step{
          .text = "The scientist is very upset. He leaves your cabinet. Some time later you receiving report that he is missing.",
          .diff = std::map<Resource, int>{ {Resource::Peoples, -1} },
          .ignoreCheck = true,
          .choices = {
            {-1, "Continue"}
          }
        }
      }, {
        3,
        Event::Step{
          .text = "The scientist suggests to fund him some minerals to start work, and to return to construction later.",
          .diff = std::map<Resource, int>{},
          .ignoreCheck = false,
          .choices = {
            {4, "Approve his proposal (-5 minerals)"},
            {2, "Ignore him"},
          }
        }
      }, {
        4,
        Event::Step{
          .text = "You sign his papers. He leaves satisfied.",
          .diff = std::map<Resource, int>{ {Resource::Minerals, -5} },
          .ignoreCheck = false,
          .choices = {
            {-1, "Continue"}
          }
        }
      }
    }
  };

  Event::Info spoiledFoodEvent = Event::Info{
    .type = Event::Type::SpoiledFood,
    .steps = std::map<int, Event::Step>{
      {
        0,
        Event::Step{
          .text = "An unknown mold has affected part of your food. You decide to throw it away and lost 50 food.",
          .diff = std::map<Resource, int>{ {Resource::Food, -50} },
          .ignoreCheck = true,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }
    }
  };

  Event::Info starStormEvent = Event::Info{
    .type = Event::Type::StarStorm,
    .steps = std::map<int, Event::Step>{
      {
        0,
        Event::Step{
          .text = "Star storm was detected by the equipment nearby. We can collect many useful data inside.",
          .diff = std::map<Resource, int>{},
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {1, "Send a research group of 5 people"},
            {2, "Send a research group of 15 people"},
            {3, "Try to remotely scan the anomaly"},
            {4, "Ignore"}
          }
        }
      }, {
        1,
        Event::Step{
          .text = "You receives a message from the expedition. They says that research still not finished, but situation inside becomes dangerous very fast.",
          .diff = std::map<Resource, int>{},
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {5, "Order them return immediately"},
            {6, "Order them to finish research"},
          }
        }
      }, {
        2,
        Event::Step{
          .text = "You receives a message from the expedition. They says that research still not finished, but situation inside becomes dangerous very fast.",
          .diff = std::map<Resource, int>{},
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {5, "Order them return immediately"},
            {7, "Order them to finish research"},
          }
        }
      }, {
        3,
        Event::Step{
          .text = "You remotely scan the storm. This brings a bit of scientific data, but no so much as if you research it from inside. (+30 science)",
          .diff = std::map<Resource, int>{ {Resource::Science, 30} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }, {
        4,
        Event::Step{
          .text = "You decided to focus on survival.",
          .diff = std::map<Resource, int>{},
          .ignoreCheck = true,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }, {
        5,
        Event::Step{
          .text = "Research group returns with a bit of data. Who knows how many unexplored things has left inside the anomaly. (+40 science)",
          .diff = std::map<Resource, int>{ {Resource::Science, 40} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }, {
        6,
        Event::Step{
          .text = "You receiving scientific data for an about hour. After that the transfer is interrupts. You wait for the research team until the end of the day, but no one comes back. (+120 science -5 people)",
          .diff = std::map<Resource, int>{ {Resource::Science, 120}, {Resource::Peoples, -5} },
          .ignoreCheck = true,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }, {
        7,
        Event::Step{
          .text = "You receiving scientific data for an about hour. After that the transfer is interrupts. You wait for the research team until the end of the day, but no one comes back. (+300 science -15 people)",
          .diff = std::map<Resource, int>{ {Resource::Science, 300}, {Resource::Peoples, -15} },
          .ignoreCheck = true,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }
    }
  };

  Event::Info gasFoodEvent = Event::Info{
    .type = Event::Type::GasFood,
    .steps = std::map<int, Event::Step>{
      {
        0,
        Event::Step{
          .text = "Scientists reports that theoretically there is an opportunity to convert food into gas.",
          .diff = std::map<Resource, int>{},
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue without experiments"},
            {1, "Convert 20 food to gas"},
            {2, "Convert 50 food to gas"},
            {3, "Convert 150 food to gas"},
          }
        }
      }, {
        1,
        Event::Step{
          .text = "The experiment was successful. (-20 food +20 gas)",
          .diff = std::map<Resource, int>{ {Resource::Food, -20}, {Resource::Gas, 20} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }, {
        2,
        Event::Step{
          .text = "The experiment was successful. (-50 food +50 gas)",
          .diff = std::map<Resource, int>{ {Resource::Food, -50}, {Resource::Gas, 50} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }, {
        3,
        Event::Step{
          .text = "The experiment was successful. (-150 food +150 gas)",
          .diff = std::map<Resource, int>{ {Resource::Food, -150}, {Resource::Gas, 150} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }
    }
  };

  Event::Info newcomersEvent = Event::Info{
    .type = Event::Type::Newcomers,
    .steps = std::map<int, Event::Step>{
      {
        0,
        Event::Step{
          .text = "A rescue pod landed nearby. It looks like another research team members were able to survive the crash. They're joining the camp. (+12 people)",
          .diff = std::map<Resource, int>{ {Resource::Peoples, 12} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"}
          }
        }
      }
    }
  };

  Event::Info landfallEvent = Event::Info{
    .type = Event::Type::Landfall,
    .steps = std::map<int, Event::Step>{
      {
        0,
        Event::Step{
          .text = "Some tiles are about to collapse.",
          .diff = std::map<Resource, int>{},
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {1, "Use minerals to stop the destruction (-10 minerals)"},
            {2, "Use science to stop the destruction (-3 science)"},
            {3, "Ignore"},
          }
        }
      }, {
        1,
        Event::Step{
          .text = "You prevented destruction with minerals.",
          .diff = std::map<Resource, int>{ {Resource::Minerals, -10} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"},
          }
        }
      }, {
        2,
        Event::Step{
          .text = "You prevented destruction with scientific data.",
          .diff = std::map<Resource, int>{ {Resource::Science, -3} },
          .ignoreCheck = false,
          .choices = std::map<int, std::string>{
            {-1, "Continue"},
          }
        }
      }, {
        3,
        Event::Step{
          .text = "You did nothing and unfortunately, 2 tiles has fallen off.",
          .diff = std::map<Resource, int>{ {Resource::Tiles, -2} },
          .ignoreCheck = true,
          .choices = std::map<int, std::string>{
            {-1, "Continue"},
          }
        }
      }
    }
  };

  std::map<Event::Type, Event::Info*> eventInfos = {
    {Event::Type::Start, &startEvent},
    {Event::Type::Win, &winEvent},
    {Event::Type::Lose, &loseEvent},
    {Event::Type::Magnetic, &magneticEvent},
    {Event::Type::Toxic, &toxicEvent},
    {Event::Type::Flare, &flareEvent},
    {Event::Type::Radio, &radioEvent},
    {Event::Type::SpoiledFood, &spoiledFoodEvent},
    {Event::Type::StarStorm, &starStormEvent},
    {Event::Type::GasFood, &gasFoodEvent},
    {Event::Type::Newcomers, &newcomersEvent},
    {Event::Type::Landfall, &landfallEvent},
  };

  std::vector<Event::Type> randomEvents = {
    Event::Type::Magnetic,
    Event::Type::Toxic,
    Event::Type::Flare,
    Event::Type::Radio,
    Event::Type::SpoiledFood,
    Event::Type::StarStorm,
    Event::Type::GasFood,
    Event::Type::Newcomers,
    Event::Type::Landfall,
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
  void RemoveTile(int count);

  int GetResource(Resource res);
  void SetResource(Resource res, int amount);
  void UpdateResource(Resource res, int amount);
  std::string GetResourceName(Resource res);

  bool TryToBuild(Building::Type building, int x, int y);
  void RemoveBuilding(int x, int y);

  bool HasEvent() const { return (currentEvent != nullptr); }
  void EmitEvent(Event::Type type);
  bool HandleStepEvent(int step);
  bool CheckStepEvent(int step);
  std::vector<std::pair<std::string, SDL2pp::Color>> GetEventText();
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
