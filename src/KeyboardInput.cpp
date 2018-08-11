#include "KeyboardInput.hpp"

KeyboardInput::KeyboardInput() {
  memset(previousInput, 0, sizeof(Uint8) * SDL_NUM_SCANCODES);
  memset(currentInput, 0, sizeof(Uint8) * SDL_NUM_SCANCODES);
}

void KeyboardInput::Update() {
  memcpy(previousInput, currentInput, sizeof(Uint8) * SDL_NUM_SCANCODES);
  memcpy(currentInput, SDL_GetKeyboardState(nullptr), sizeof(Uint8) * SDL_NUM_SCANCODES);
}

bool KeyboardInput::KeyTriggered(const SDL_Scancode key) const {
  return (currentInput[key] == 1 && previousInput[key] == 0);
}

bool KeyboardInput::KeyPressed(const SDL_Scancode key) const {
  return (currentInput[key] == 1);
}

bool KeyboardInput::KeyReleased(const SDL_Scancode key) const {
  return (currentInput[key] == 0 && previousInput[key] == 1);
}
