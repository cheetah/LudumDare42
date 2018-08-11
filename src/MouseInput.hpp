#ifndef _MOUSEINPUT_HPP_
  #define _MOUSEINPUT_HPP_

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_mouse.h>

#include <SDL2pp/Point.hh>

class MouseInput {
private:
  int currentX, currentY, previousX, previousY;
  Uint32 currentMouseState, previousMouseState;
  Sint32 wheelX, wheelY;
public:
  void Update();
  void ReceiveEvent(const SDL_Event &event);

  SDL2pp::Point GetPosition() const;
  SDL2pp::Point GetDifference() const;

  int GetWheelX() const;
  int GetWheelY() const;

  bool ButtonTriggered(const Uint32 button) const;
  bool ButtonPressed(const Uint32 button) const;
  bool ButtonReleased(const Uint32 button) const;
};

#endif
