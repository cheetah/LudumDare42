#include "Buildings.hpp"

namespace Building {

// Base

BaseBuilding::BaseBuilding(std::string name, std::string desc) :
  name(name),
  description(desc) {
}

// Biodome

Biodome::Biodome() :
  BaseBuilding(
    "Biodome",
    "Can be placed on any tile. Produces 25 food and 500 oxygen per day."
  ) {
}

// Oxygen Tank

OxygenTank::OxygenTank() :
  BaseBuilding(
    "Oxygen Tank",
    "Can be placed on any tile. Stores 1000 oxygen inside."
  ) {
}

// HarvestStation

HarvestStation::HarvestStation() :
  BaseBuilding(
    "Harvest Station",
    "Can be placed only on mineral tile. Mines 100 minerals per day."
  ) {
}

// Refinery

Refinery::Refinery() :
  BaseBuilding(
    "Refinery",
    "Can be placed only on geyser tile. Refines 50 gas per day."
  ) {
}

// Scince Lab
ScienceLab::ScienceLab() :
  BaseBuilding(
    "Science Laboratory",
    "Can be placed on any tile. Produces 50 scientific data per day."
  ) {
}

};
