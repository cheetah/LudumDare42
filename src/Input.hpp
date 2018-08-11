#ifndef _INPUT_HPP_
  #define _INPUT_HPP_

#include "MouseInput.hpp"
#include "KeyboardInput.hpp"

class Input {
private:
  MouseInput mouseInput;
  KeyboardInput keyboardInput;
public:
  void Update();

  MouseInput* Mouse() { return &mouseInput; }
  KeyboardInput* Keyboard() { return &keyboardInput; }
};

#endif
