#include "LocalCoordinates.hpp"

LocalCoordinates::LocalCoordinates(std::function<SDL2pp::Point(SDL2pp::Point point)> func) : translate(func) {
}

SDL2pp::Point LocalCoordinates::t(SDL2pp::Point point) {
  return translate(point);
}

