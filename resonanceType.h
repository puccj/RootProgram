#ifndef RESONANCE_H
#define RESONANCE_H

#include "particleType.h"

class ResonanceType : public ParticleType {
 public:
  ResonanceType(ParticleType const& particleType, double width) : ParticleType(particleType), width_{width} {};
  ResonanceType(std::string const& name, double mass, int charge, double width) : ParticleType(name, mass, charge), width_{width} {};
  
  double width() { return width_; };
  void print();

 private:
  const double width_;
};

#endif //RESONANCE_H