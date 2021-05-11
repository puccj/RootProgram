#include "particle.h"
#include <cmath>
#include <cstdlib> //for RAND_MAX

int Particle::typeCounter_ = 0;
ParticleType* Particle::particleTypes_[maxNumParticleType_];

Particle::Particle(std::string const& particleName, Impulse const& impulse) : impulse_{impulse}, index_{findParticle(particleName)} {
  if (index_ == -1) {
    std::cout << "No particle named \"" << particleName << "\" was found in the particleTypes array \n";
    std::cout << "Do you want to add it? (Y/N) ";
    char answer = 'N';
    std::cin >> answer;
    if (answer == 'S' || answer == 's' || answer == 'Y' || answer == 'y') {
      std::cout << "\nThen insert:\nMass: " ;
      double mass;
      std::cin >> mass;

      std::cout << "Charge: ";
      int charge;
      std::cin >> charge;

      std::cout << "Width (insert 0 if you want to ignore width): ";
      double width;
      std::cin >> width;
      std::cout << '\n';

      addParticleType(particleName, mass, charge, width);
    }
  }
}

//constructor has been copied instead of calling, otherwise it won't initialize index_ correctly
Particle::Particle(std::string const& particleName, double px, double py, double pz) : impulse_{px,py,pz}, index_{findParticle(particleName)}{
  if (index_ == -1) {
    std::cout << "No particle named \"" << particleName << "\" was found in the particleTypes array \n";
    std::cout << "Do you want to add it? (Y/N) ";
    char answer = 'N';
    std::cin >> answer;
    if (answer == 'S' || answer == 's' || answer == 'Y' || answer == 'y') {
      std::cout << "\nThen insert:\nMass: " ;
      double mass;
      std::cin >> mass;

      std::cout << "Charge: ";
      int charge;
      std::cin >> charge;

      std::cout << "Width (insert 0 if you want to ignore width): ";
      double width;
      std::cin >> width;

      addParticleType(particleName, mass, charge, width);
    }
  }
}

Particle::Particle(int index) : index_{index} {
  if (index_ >= typeCounter_) {
    std::cerr << "\nERROR: Index " << index << " does not correspond to any particle type.\n";
    std::cerr << "       Set the index again with setIndex function, eventually after added a new particleType with addParticleType static fuction.\n";
    std::cout << "Help: Use the printTypes static function to list all the added types.\n";
    index_ = -1;
  }
}

double Particle::impulse(char direction) {
  switch (direction)
  {
  case 'x':
  case 'X':
    return impulseX();
  case 'y':
  case 'Y':
    return impulseY();
  case 'z':
  case 'Z':
    return impulseZ();
  }
  
  std::cerr << "\nERROR: " << direction << " is an invalid direction. Please select between x, y and z\n";
  return 0;
}

void Particle::setIndex(int index) {
  if (index >= typeCounter_) {
      std::cerr << "\nERROR: Index " << index << " does not correspond to any particle type\n";
      return;
  }
  index_ = index;
}

void Particle::setIndex(std::string const& name) {
  int index = findParticle(name);
  if (index == -1) {
    std::cerr << "\nERROR: Particle " << name << " doesn't correspond to any particle type\n";
    return;
  }
  index_ = index;
}

double Particle::mass() const{
  if (index_ == -1) {
    std::cerr << "\nERROR: Couldn't find the particle mass (nor energy) because the particle has not been added to the particleTypes array.\n";
    std::cerr << "       Use the Particle::addParticleType static function to add it.\n";
    return -1;
  }

  return particleTypes_[index_]->mass();
}

void Particle::print() {
  if (index_ == -1) {
    std::cerr << "\nERROR: Couldn't print particle informations because particle has not been added to the particleTypes array.\n";
    std::cerr << "       Use the Particle::addParticleType static function to add it.\n";
    return;
  }

  std::cout << "Index: " << index_ << " - Name: " << particleTypes_[index_]->name() 
            << " - Impulse: " << impulse_.px << ", " << impulse_.py << ", " << impulse_.pz << '\n';
}

double Particle::energy() {
  double particleMass = mass();
  
  if (particleMass == -1) //error
    return -1;

  return sqrt(particleMass*particleMass + (impulse_.px*impulse_.px + impulse_.py*impulse_.py + impulse_.pz*impulse_.pz));
}

double Particle::invMass(Particle & other) {
  double thisEnergy = energy();
  double otherEnergy = other.energy();
  if (thisEnergy == -1 || otherEnergy == -1) {
    std::cerr << "\nERROR: Can't calculate invMass because at least one of the energy is invalid.\n";
    std::cerr << "       You probably forgot to add the particle to the particleTypes array.\n";
    std::cerr << "       Use the Particle::addParticleType static function to add it.\n";
    return -1;
  }

  double sumOfEnergy = thisEnergy + otherEnergy;
  double impulseX = impulse_.px + other.impulseX();
  double impulseY = impulse_.py + other.impulseY();
  double impulseZ = impulse_.pz + other.impulseZ();
  
  /*if (sumOfEnergy*sumOfEnergy < impulseX*impulseX + impulseY*impulseY + impulseZ*impulseZ;) {
    std::cerr << "\nERROR: Negative term under square root\n";
    return -1;
  }*/

  return sqrt(sumOfEnergy*sumOfEnergy - (impulseX*impulseX + impulseY*impulseY + impulseZ*impulseZ));
}

void Particle::addParticleType(std::string const& name, double mass, int charge, double width) {
  if (typeCounter_ >= maxNumParticleType_) {
    std::cerr << "Maximum number of different types reached (" << maxNumParticleType_ << ")\n";
    return;
  }

  if (findParticle(name) != -1) {
    std::cout << "Particle type '" << name << "' exists already\n";
    return;
  }

  if (width > 0)
    particleTypes_[typeCounter_] = new ResonanceType(name, mass, charge, width);
  else
    particleTypes_[typeCounter_] = new ParticleType(name, mass, charge);    
  
  typeCounter_++; 
}

void Particle::printTypes() {
  for (int i = 0; i < typeCounter_; i++) {
    std::cout << "Index: " << i << " - ";
    particleTypes_[i]->print();
    std::cout << '\n';
  }
}

int Particle::decay2body(Particle &dau1,Particle &dau2) const {
  if(mass() == 0.0){
    std::cerr << "\nERROR: Decayment cannot be preformed if mass is zero\n";
    return 1;
  }
  
  double massMot = mass();
  double massDau1 = dau1.mass();
  double massDau2 = dau2.mass();

  if(index_ > -1){ // add width effect

    // gaussian random numbers

    float x1, x2, w, y1; //y2;  //y2 set but not used
    
    double invnum = 1./RAND_MAX;
    do {
      x1 = 2.0 * rand()*invnum - 1.0;
      x2 = 2.0 * rand()*invnum - 1.0;
      w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );
    
    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    //y2 = x2 * w; 

    massMot += particleTypes_[index_]->width() * y1;
  }

  if(massMot < massDau1 + massDau2){
    std::cerr << ("Decayment cannot be preformed because mass is too low in this channel\n");
    return 2;
  }
  
  double pout = sqrt((massMot*massMot - (massDau1+massDau2)*(massDau1+massDau2))*(massMot*massMot - (massDau1-massDau2)*(massDau1-massDau2)))/massMot*0.5;

  double norm = 2*M_PI/RAND_MAX;

  double phi = rand()*norm;
  double theta = rand()*norm*0.5 - M_PI/2.;
  dau1.setImpulse(pout*sin(theta)*cos(phi),pout*sin(theta)*sin(phi),pout*cos(theta));
  dau2.setImpulse(-pout*sin(theta)*cos(phi),-pout*sin(theta)*sin(phi),-pout*cos(theta));

  double energy = sqrt(impulse_.px*impulse_.px + impulse_.py*impulse_.py + impulse_.pz*impulse_.pz + massMot*massMot);

  double bx = impulse_.px/energy;
  double by = impulse_.py/energy;
  double bz = impulse_.pz/energy;

  dau1.boost(bx,by,bz);
  dau2.boost(bx,by,bz);

  return 0;
}

int Particle::findParticle(std::string const& particleName) {
  for (int i = 0; i < typeCounter_; i++)
    if (particleName == particleTypes_[i]->name())
      return i;
  return -1;
}

void Particle::boost(double bx, double by, double bz)
{
  double energy = Particle::energy();

  //Boost this Lorentz vector
  double b2 = bx*bx + by*by + bz*bz;
  double gamma = 1.0 / sqrt(1.0 - b2);
  double bp = bx*impulse_.px + by*impulse_.py + bz*impulse_.pz;
  double gamma2 = b2 > 0 ? (gamma - 1.0)/b2 : 0.0;

  impulse_.px += gamma2*bp*bx + gamma*bx*energy;
  impulse_.py += gamma2*bp*by + gamma*by*energy;
  impulse_.pz += gamma2*bp*bz + gamma*bz*energy;
}