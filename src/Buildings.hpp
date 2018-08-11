#ifndef _BUILDINGS_HPP_
  #define _BUILDINGS_HPP_

#include <string>

class World;
struct Tile;

namespace Building {

class BaseBuilding {
protected:
  std::string name = "";
  std::string description = "";
public:
  BaseBuilding(std::string name, std::string desc);

  virtual void Tick(World *world) {};
  virtual bool CanPlaceOn(Tile *tile) { return false; };
};

class Biodome : public BaseBuilding {
public:
  Biodome();
};

class OxygenTank : public BaseBuilding {
public:
  OxygenTank();
};

class HarvestStation : public BaseBuilding {
public:
  HarvestStation();
};

class Refinery : public BaseBuilding {
public:
  Refinery();
};

class ScienceLab : public BaseBuilding {
public:
  ScienceLab();
};

};

#endif
