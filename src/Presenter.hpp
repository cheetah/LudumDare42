#ifndef _PRESENTER_HPP_
  #define _PRESENTER_HPP_

class Input;

class Presenter {
private:
public:
  Presenter(bool is_composite = false);
  ~Presenter();

  virtual void Interact(Input *input);
  virtual void Render();
};

#endif
