#include "resonanceType.h"

void ResonanceType::print() {
  ParticleType::print();
  std::cout << " - width: " << width_;
}
