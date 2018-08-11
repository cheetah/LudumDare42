#include "LocalCoordinates.hpp"

LocalCoordinates::LocalCoordinates(std::function<SDL2pp::Point(SDL2pp::Point point)> func) : translate(func) {
}

SDL2pp::Point LocalCoordinates::t(SDL2pp::Point point) {
  return translate(point);
}

SDL2pp::Point LocalCoordinates::Isometric(SDL2pp::Point point) {
  return SDL2pp::Point(point.x - point.y, (point.x + point.y) / 2);
}

SDL2pp::Point LocalCoordinates::Cartesian(SDL2pp::Point point) {
  return SDL2pp::Point((2 * point.x + point.y) / 2, (2 * point.x - point.y) / 2);
}
