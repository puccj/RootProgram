#ifndef PARTICLETYPE_H
#define PARTICLETYPE_H

#include <iostream>

class ParticleType {
 public:
  ParticleType(std::string const& name, double mass, int charge);

  const std::string name() { return name_; };
  double mass() { return mass_; };
  int charge() { return charge_; };
  virtual double width() { return 0; };

  virtual void print();

 private:
  const std::string name_;
  const double mass_;
  const int charge_;
};

#endif //PARTICLETYPE_H