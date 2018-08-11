#ifndef _OBJECT_HPP_
  #define _OBJECT_HPP_

class Object {
private:
public:
  Object();
  ~Object();

  virtual void Update(float elapsed);
};

#endif
