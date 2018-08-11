#ifndef _KEYBOARDINPUT_HPP_
  #define _KEYBOARDINPUT_HPP_

#include <SDL.h>

class KeyboardInput {
private:
  Uint8 previousInput[SDL_NUM_SCANCODES];
  Uint8 currentInput[SDL_NUM_SCANCODES];
public:
  KeyboardInput();

  void Update();

  bool KeyTriggered(const SDL_Scancode key) const;
  bool KeyPressed(const SDL_Scancode key) const;
  bool KeyReleased(const SDL_Scancode key) const;
};

#endif
