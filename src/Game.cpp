#include "Game.hpp"

Game Game::instance;

Game* Game::Instance() {
  return &Game::instance;
}

Game::Game() :
  sdl(SDL_INIT_VIDEO),
  image(IMG_INIT_PNG),
  window(
    "Asteroid",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    1000,
    700,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
  ),
  render(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
{
  render.SetDrawColor(140, 62, 173);
  lastTime = SDL_GetTicks();
}

int Game::Loop() {
  while(running) {
    Step();
    SDL_Delay(1);
  }

  return EXIT_SUCCESS;
}

void Game::Step() {
  auto timeNow = SDL_GetTicks();
  auto elapsed = timeNow - lastTime;
  lastTime = timeNow;

  Interact();
  Update(elapsed);
  Render();
}

void Game::Interact() {
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    if(event.type == SDL_QUIT) {
      running = false;
      break;
    }
  }

  input.Update();
  for(Presenter *presenter : presenters) {
    presenter->Interact(&input);
  }
}

void Game::Update(float elapsed) {
  for(Object *object : objects) {
    object->Update(elapsed);
  }
}

void Game::Render() {
  render.Clear();
  for(Presenter *presenter : presenters) {
    presenter->Render();
  }
  render.Present();
}

void Game::AddObject(Object *object) {
  objects.push_back(object);
}

void Game::RemoveObject(Object *object) {
  objects.erase(
    std::remove_if(
      std::begin(objects),
      std::end(objects),
      [object](Object *obj) { return  obj == object; }
    ),
    std::end(objects)
  );
}

void Game::AddPresenter(Presenter *presenter) {
  presenters.push_back(presenter);
}

void Game::RemovePresenter(Presenter *presenter) {
  presenters.erase(
    std::remove_if(
      std::begin(presenters),
      std::end(presenters),
      [presenter](Presenter *p) { return  p == presenter; }
    ),
    std::end(presenters)
  );
}
