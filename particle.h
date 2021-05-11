#ifndef PARTICLE_H
#define PARTICLE_H

#include "resonanceType.h"

struct Impulse {
  double px;
  double py;
  double pz;
};

class Particle {
 public:
  Particle(std::string const& particleName, Impulse const& impulse);
  Particle(std::string const& particleName, double px = 0, double py = 0, double pz = 0);
  Particle(int index = -1);

  Impulse const impulse() { return impulse_; };
  double impulse(char direction);
  double impulseX() { return impulse_.px; };
  double impulseY() { return impulse_.py; };
  double impulseZ() { return impulse_.pz; };
  void setImpulse(double px, double py, double pz) { impulse_ = {px,py,pz}; };
  void setImpulse(Impulse impulse) { impulse_ = impulse; };

  int index() { return index_; };
  void setIndex(int index);
  void setIndex(std::string const& name);
  double mass() const;
  void print();
  double energy();
  double invMass(Particle & other);

  static void addParticleType(std::string const& name, double mass, int charge, double whidth = 0);
  static void printTypes();

  int decay2body(Particle &dau1, Particle &dau2) const; 
  
 private:
  static const int maxNumParticleType_ = 10;
  static ParticleType* particleTypes_[maxNumParticleType_];
  static int typeCounter_;
  Impulse impulse_;
  int index_;
  
  static int findParticle(std::string const& particleName);
  void boost(double bx, double by, double bz);
};


#endif //PARTICLE_H