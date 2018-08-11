#include <array>

#include <SDL2pp/Texture.hh>
#include <SDL2pp/Exception.hh>

#include "Game.hpp"
#include "Input.hpp"
#include "Presenter.hpp"

#include "World.hpp"

#include "NFont.h"
#include "SDL_FontCache.h"

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif

class Test : Presenter {
private:
  SDL2pp::Renderer &render = Game::Instance()->GetRender();
  SDL2pp::Texture ground = SDL2pp::Texture(render, "./assets/isometric.png");

  SDL2pp::Rect selectedRect;
  std::array<std::array<int, 8>, 8> world = {0};

  NFont font = NFont(render.Get(), "./assets/Fontana.ttf", 20);

  SDL2pp::Point isoPoint(SDL2pp::Point point) {
    return SDL2pp::Point(point.x - point.y, (point.x + point.y) / 2) + SDL2pp::Point(512, 64);
  }

  SDL2pp::Point cartesianPoint(SDL2pp::Point point) {
    return SDL2pp::Point((2 * point.x + point.y) / 2, (2 * point.x - point.y) / 2);
  }
public:
  Test() {
    RandomizeGround();
  }

  void RandomizeGround() {
    for(auto& tileRow : world) {
      for(auto& tile : tileRow) {
        tile = rand() % 4;
      }
    }
  }

  void Interact(Input *input) override {
    selectedRect = SDL2pp::Rect(isoPoint(input->Mouse()->GetPosition()), SDL2pp::Point(64, 64));

    if(input->Keyboard()->KeyTriggered(SDL_SCANCODE_R)) {
      RandomizeGround();
    }
  }

  void Render() override {
    for(const auto& tileRow : world) {
      auto rowIndex = &tileRow - &world[0];
      for(const auto& tile : tileRow) {
        auto colIndex = &tile - &tileRow[0];

        render.Copy(
          ground,
          SDL2pp::Rect(0 + 64 * tile, 0, 64, 64),
          SDL2pp::Rect(isoPoint(SDL2pp::Point(rowIndex * 32, colIndex * 32)), SDL2pp::Point(64, 64))
        );

        render.FillRect(selectedRect);
      }
    }

    // font.draw(render.Get(), 64, 64, "Multi\nline\ntext");
  }
};

#ifdef __EMSCRIPTEN__
void emscriptenloop() {
  Game::Instance()->Step();
}
#endif

int main() {
  try {
    auto *game = Game::Instance();
    Test tt;
    World w;

  #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(emscriptenloop, 0, 1);
  #else
    return game->Loop();
  #endif

  } catch (SDL2pp::Exception& e) {
    std::cout << "Error in: " << e.GetSDLFunction() << std::endl;
    std::cout << "  Reason: " << e.GetSDLError() << std::endl;
    return 1;
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
