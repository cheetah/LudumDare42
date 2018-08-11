#ifndef _RESOURCES_HPP_
  #define _RESOURCES_HPP_

class World;

namespace Resource {

class BaseResource {
protected:
  int amount = 0;
public:
  BaseResource(int amount) : amount(amount) {}

  void Add(int a) { amount += a; }
  void Sub(int a) { amount -= a; if(amount < 0) amount = 0; }

  int GetAmount() { return amount; }

  virtual void Tick(World *world) {};
};

class Peoples : public BaseResource {
private:
public:
  Peoples() : BaseResource(50) {}

  void Tick(World *world);
};

class Food : public BaseResource {
private:
public:
  Food() : BaseResource(100) {}
};

class Oxygen : public BaseResource {
private:
public:
  Oxygen() : BaseResource(500) {}
};

class Minerals : public BaseResource {
private:
public:
  Minerals() : BaseResource(30) {}
};

class Gas : public BaseResource {
private:
public:
  Gas() : BaseResource(0) {}
};

class ScientificData : public BaseResource {
private:
public:
  ScientificData() : BaseResource(0) {}
};

};

#endif
