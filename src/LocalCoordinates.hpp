#ifndef _LOCALCOORDINATES_HPP_
  #define _LOCALCOORDINATES_HPP_

#include <functional>

#include <SDL2pp/Rect.hh>
#include <SDL2pp/Point.hh>

class LocalCoordinates {
private:
  std::function<SDL2pp::Point(SDL2pp::Point point)> translate;
public:
  LocalCoordinates(std::function<SDL2pp::Point(SDL2pp::Point point)> func);

  SDL2pp::Point t(SDL2pp::Point point);

  static SDL2pp::Point Isometric(SDL2pp::Point point);
  static SDL2pp::Point Cartesian(SDL2pp::Point point);
};

#endif
