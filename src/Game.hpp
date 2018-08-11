#ifndef _GAME_HPP_
  #define _GAME_HPP_

#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/SDLImage.hh>
#include <SDL2pp/Window.hh>
#include <SDL2pp/Renderer.hh>

#include "Input.hpp"
#include "Object.hpp"
#include "Presenter.hpp"

class Game {
private:
  static Game instance;

  SDL2pp::SDL sdl;
  SDL2pp::SDLImage image;
  SDL2pp::Window window;
  SDL2pp::Renderer render;

  Uint32 lastTime = 0;
  bool running = true;
  std::vector<Object*> objects;
  std::vector<Presenter*> presenters;

  Input input;

  Game();
  void Interact();
  void Update(float elapsed);
  void Render();
public:
  static Game* Instance();

  int Loop();
  void Step();

  void AddObject(Object *object);
  void RemoveObject(Object *object);

  void AddPresenter(Presenter *presenter);
  void RemovePresenter(Presenter *presenter);

  SDL2pp::Renderer& GetRender() { return render; }
};

#endif
