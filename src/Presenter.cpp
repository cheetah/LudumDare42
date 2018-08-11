#include "Game.hpp"
#include "Presenter.hpp"

Presenter::Presenter(bool is_composite) {
  if(!is_composite) {
    Game::Instance()->AddPresenter(this);
  }
}

Presenter::~Presenter() {
  Game::Instance()->RemovePresenter(this);
}

void Presenter::Interact(Input *input) {
}

void Presenter::Render() {
}
