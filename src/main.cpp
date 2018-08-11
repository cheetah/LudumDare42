#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif

#include <array>

#include <SDL2pp/Texture.hh>
#include <SDL2pp/Exception.hh>

#include <NFont.h>
#include <SDL_FontCache.h>

#include <fmt/format.h>

#include "Game.hpp"
#include "Input.hpp"
#include "Presenter.hpp"
#include "LocalCoordinates.hpp"

#include "World.hpp"

using Rect = SDL2pp::Rect;
using Point = SDL2pp::Point;
using Color = SDL2pp::Color;

class BuildUI : Presenter {
private:
  SDL2pp::Renderer &render = Game::Instance()->GetRender();
  SDL2pp::Texture ground = SDL2pp::Texture(render, "./assets/isometric.png");
  NFont font = NFont(render.Get(), "./assets/Fontana.ttf", 14);

  World *world;
public:
  BuildUI(World *world) : world(world) {
  }

  void RenderCard(int index, const Building::Info *info) {
    LocalCoordinates lc([=] (Point point) {
      return point + Point(32, 32 + index * 120);
    });

    Color oldDrawColor = render.GetDrawColor();

    render.SetDrawColor(Color(0, 0, 0));
    render.DrawRect(Rect(lc.t(Point(0, 0)), Point(320, 112)));
    render.DrawRect(Rect(lc.t(Point(1, 1)), Point(318, 110)));

    render.SetDrawColor(Color(192, 192, 192));
    render.FillRect(Rect(lc.t(Point(2, 2)), Point(316, 108)));

    render.Copy(ground, info->tileRect, Rect(lc.t(Point(16, 16)), Point(64, 64)));

    std::string cost = "";
    for(const auto& res : info->cost) {
      cost.append(fmt::format("{} {} ", res.second, world->GetResourceName(res.first)));
    }
    font.drawBox(render.Get(), Rect(lc.t(Point(96, 16)), Point(216, 14)), "%s", cost.c_str());

    font.drawBox(
      render.Get(),
      Rect(lc.t(Point(96, 40)), Point(216, 58)),
      "%s\n%s",
      info->name.c_str(),
      info->description.c_str()
    );

    render.SetDrawColor(oldDrawColor);
  }

  void Render() override {
    int index = 0;
    for(const auto& el : world->GetBuildingInfos()) {
      RenderCard(index, &el.second);
      index++;
    }
  }
};

class ResourceUI : Presenter {
private:
  SDL2pp::Renderer &render = Game::Instance()->GetRender();
  NFont font = NFont(render.Get(), "./assets/Fontana.ttf", 14);

  World *world;
public:
  ResourceUI(World *world) : world(world) {
  }

  void Render() override {
    LocalCoordinates lc([=] (Point point) {
      return point + Point(400, 392);
    });

    Color oldDrawColor = render.GetDrawColor();

    render.SetDrawColor(Color(0, 0, 0));
    render.DrawRect(Rect(lc.t(Point(0, 0)), Point(568, 232)));
    render.DrawRect(Rect(lc.t(Point(1, 1)), Point(566, 230)));

    render.SetDrawColor(Color(192, 192, 192));
    render.FillRect(Rect(lc.t(Point(2, 2)), Point(564, 228)));

    font.drawBox(
      render.Get(),
      Rect(lc.t(Point(12, 12)), Point(80, 96)),
      "Peoples:\nFood:\nOxygen:\nMinerals:\nGas:\nScience:"
    );

    font.drawBox(
      render.Get(),
      Rect(lc.t(Point(92, 12)), Point(40, 96)),
      "%d\n%d\n%d\n%d\n%d\n%d",
      world->GetResource(Resource::Peoples),
      world->GetResource(Resource::Food),
      world->GetResource(Resource::Oxygen),
      world->GetResource(Resource::Minerals),
      world->GetResource(Resource::Gas),
      world->GetResource(Resource::Science)
    );

    font.drawBox(
      render.Get(),
      Rect(lc.t(Point(144, 12)), Point(160, 96)),
      "%d days until evacuation",
      world->GetResource(Resource::DaysUntilEvacuation)
    );

    std::string fullLog;
    for(const auto& logItem : world->GetLog()) {
      fullLog.append(logItem);
      fullLog.append("\n");
    }

    font.drawBox(
      render.Get(),
      Rect(lc.t(Point(144, 40)), Point(400, 176)),
      "%s",
      fullLog.c_str()
    );

    render.SetDrawColor(oldDrawColor);
  }
};

class Test : Presenter {
private:
  SDL2pp::Renderer &render = Game::Instance()->GetRender();
  SDL2pp::Texture ground = SDL2pp::Texture(render, "./assets/isometric.png");

  SDL2pp::Rect selectedRect;
  std::array<std::array<int, 8>, 8> world = {0};

  NFont font = NFont(render.Get(), "./assets/Fontana.ttf", 20);

  SDL2pp::Point isoPoint(SDL2pp::Point point) {
    return SDL2pp::Point(point.x - point.y, (point.x + point.y) / 2) + SDL2pp::Point(642, 64);
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

        // render.FillRect(selectedRect);
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

    World world;
    Test tt;
    BuildUI ui(&world);
    ResourceUI rui(&world);

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
