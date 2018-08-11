#include "MouseInput.hpp"

void MouseInput::Update() {
  previousX = currentX;
  previousY = currentY;
  previousMouseState = currentMouseState;

  wheelX = 0;
  wheelY = 0;

  currentMouseState = SDL_GetMouseState(&currentX, &currentY);
}

void MouseInput::ReceiveEvent(const SDL_Event &event) {
  switch(event.type) {
  case SDL_MOUSEWHEEL:
    wheelX = event.wheel.x;
    wheelY = event.wheel.y;

    break;
  }
}

SDL2pp::Point MouseInput::GetPosition() const {
  return SDL2pp::Point(currentX, currentY);
}

SDL2pp::Point MouseInput::GetDifference() const {
  return SDL2pp::Point(currentX - previousX, currentY - previousY);
}

int MouseInput::GetWheelX() const {
  return wheelX;
}

int MouseInput::GetWheelY() const {
  return wheelY;
}

bool MouseInput::ButtonTriggered(const Uint32 button) const {
  return ((SDL_BUTTON(button) & currentMouseState) != 0) &&
         ((SDL_BUTTON(button) & previousMouseState) == 0);
}

bool MouseInput::ButtonPressed(const Uint32 button) const {
  return (SDL_BUTTON(button) & currentMouseState) != 0;
}

bool MouseInput::ButtonReleased(const Uint32 button) const {
  return ((SDL_BUTTON(button) & currentMouseState) == 0) &&
         ((SDL_BUTTON(button) & previousMouseState) != 0);
}
