#include "particleType.h"

ParticleType::ParticleType(std::string const& name, double mass, int charge) : name_{name}, mass_{mass}, charge_{charge} {
  if(mass_ < 0)
    std::cerr << "WARNING: Particle with negative mass \n";
}

void ParticleType::print() {
  std::cout << "name: " << name_ << " - mass: " << mass_ << " - charge: " << charge_;
}

