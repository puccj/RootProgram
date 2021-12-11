#include "particleType.h"
#include "resonanceType.h"
#include "particle.h"

#include <TH1.h>
#include <TRandom.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TFile.h>

int main() {
  gRandom->SetSeed();
  gStyle->SetHistFillColor(kBlue);
  gStyle->SetHistLineColor(kBlue);
  gStyle->SetOptStat(111111);

  const int EV = 100000; //number of events (each event is made of 100 particle)

  Particle::addParticleType("pione+", 0.13957, 1);    // 0  p+
  Particle::addParticleType("pione-", 0.13957, -1);   // 1  p-
  Particle::addParticleType("kaone+", 0.49367, 1);    // 2  k+
  Particle::addParticleType("kaone-", 0.49367, -1);   // 3  k-
  Particle::addParticleType("protone+", 0.93827, 1);  // 4  pr+
  Particle::addParticleType("protone-", 0.93827, -1); // 5  pr-
  Particle::addParticleType("K*", 0.89166, 0, 0.05);  // 6  K*

  TFile* file = new TFile("data.root", "RECREATE");

  const int BIN = 1000;

  TH1F* types = new TH1F("types", "Distribution of particle type", 7, -0.5, 6.5);
  TH1F* phiDist = new TH1F("phi", "Distribution of azimut angle", BIN, 0, 2*M_PI);
  TH1F* thetaDist = new TH1F("theta", "Distribution of polar angle", BIN, 0, M_PI);
  TH1F* impulseDist = new TH1F("impulse", "Distribution of impulse module", BIN, 0, 6);
  TH1F* impulseXYDist = new TH1F("impulseXY", "Distribution of transverse imp. module", BIN, 0, 6);
  TH1F* energyDist = new TH1F("energy", "Distribution of particle energy", BIN, 0, 4);
  TH1F* invMass = new TH1F("invMass", "Invariant mass", BIN, 0, 6);                                              //all
  TH1F* dInvMass = new TH1F("discordInvMass", "Inv. mass of discord charge particle", BIN, 0, 6);                //segno discorde
  TH1F* cInvMass = new TH1F("concordInvMass", "Inv. mass of concord charge particle", BIN, 0, 6);                //segno concorde
  TH1F* dPartInvMass = new TH1F("dParticleInvMass", "Inv. mass p+ K- / p- K+", BIN, 0, 6);                       //p+ k- / p- k+
  TH1F* cPartInvMass = new TH1F("cParticleInvMass", "Inv. mass p+ K+ / p- K-", BIN, 0, 6);                       //p+ k+ / p- k-
  TH1F* decadedInvMass = new TH1F("decadedInvMass", "Inv. mass of particle gen. from K* decay", BIN, 0.6, 1.2);   //generated from K* decay

  for (int k = 0; k < EV; k++) {
    const int N = 120;
    Particle particella[N];
    int kCounter = 0;     //number of particle producted by decaiment of K*

    for (int i = 0; i < 100; i++) {
      //Generating random impulse vector
      double phi = gRandom->Uniform(0, 2*M_PI);
      double theta = gRandom->Uniform(0, M_PI);
      double module = gRandom->Exp(1);

      double modTimesSinTheta = module*sin(theta);
      double px = modTimesSinTheta*cos(phi);
      double py = modTimesSinTheta*sin(phi);
      double pz = module*cos(theta);
      
      particella[i].setImpulse(px, py, pz);

      //Setting index with the correct probability
      double uniform = gRandom->Uniform(0,1);

      if (uniform < 0.4) {        //pione+
        types->Fill(0);
        particella[i].setIndex(0);
      }
      else if (uniform < 0.8) {   //pione-
        types->Fill(1);
        particella[i].setIndex(1);
      }
      else if (uniform < 0.85) {  //kaone+
        types->Fill(2);
        particella[i].setIndex(2);
      }
      else if (uniform < 0.9) {   //kaone-
        types->Fill(3);
        particella[i].setIndex(3);
      }
      else if (uniform < 0.945) { //protone+
        types->Fill(4);
        particella[i].setIndex(4);
      }
      else if (uniform < 0.99) {  //protone-
        types->Fill(5);
        particella[i].setIndex(5);
      }
      else{                      //K*
        types->Fill(6);
        particella[i].setIndex(6);
        
        //Decading of K*        
        if (kCounter >= 20) {
          std::cerr << "\nERROR: Array too small, too many K particle decaided\n";
          return -1;
        }

        Particle pione;
        Particle kaone;
        int indexP;
        int indexK;

        //if (gRandom->Uniform(0,1) < 0.5) {
        if (uniform < 0.995) {                //faster
          indexP = 0; //p+
          indexK = 3; //k-
        }
        else {
          indexP = 1; //p-
          indexK = 2; //k+
        }

        pione.setIndex(indexP);
        kaone.setIndex(indexK);
        
        int error = particella[i].decay2body(pione, kaone);
        if (error > 0) {
          std::cerr << "Error during the decayment of particle\n";
          return -1;
        }
        
        particella[100+kCounter] = pione;
        particella[101+kCounter] = kaone;
        kCounter += 2;

        types->Fill(indexP);
        types->Fill(indexK);
      
      } //end of generation

      //Filling histos
      double pxSquarePySquare = px*px + py*py;
      
      phiDist->Fill(phi);
      thetaDist->Fill(theta);
      impulseXYDist->Fill(sqrt(pxSquarePySquare));
      impulseDist->Fill(sqrt(pxSquarePySquare + pz*pz));
      energyDist->Fill(particella[i].energy());
    }

    //Calculating invariant mass beetween all particle and add it to histos
    for (int i = 0; i < 100+kCounter; i++) {
      int iIndex = particella[i].index();
      
      if (iIndex == 6)    //6 = K*
        continue;         //Let's consider K* like it's not there, ignore them

      for (int j = i+1; j < 100+kCounter; j++) {
        int jIndex = particella[j].index();
        
        if (jIndex == 6)  //Same as before
          continue;

        double invariantMass = particella[i].invMass(particella[j]);
        invMass->Fill(invariantMass);           //all
      
        switch (iIndex)
        {
        case 0:   //i = p+
          switch (jIndex)   // The absence of break sometimes is voluntary
          {
          case 2: //j = k+
            cPartInvMass->Fill(invariantMass);  //p+ / k+   -   Concord sign, different particle
          case 0: //j = p+
          case 4: //j = pr+
            cInvMass->Fill(invariantMass);      //concord charge
            break;
          case 3: // j = k-
            dPartInvMass->Fill(invariantMass);  //p+ / k-
          case 1: //j = p-
          case 5: //j = pr-
            dInvMass->Fill(invariantMass);      //discord charge
            break;
          default:
            std::cerr << "\nSTRANGE ERROR: particella[" << j << "] (j) has an indefined index\n";
          }
          break;

        case 1:   //i = p-
          switch (jIndex)
          {
          case 3: //j = k-
            cPartInvMass->Fill(invariantMass);  //p- / k-
          case 1: //j = p-
          case 5: //j = pr-
            cInvMass->Fill(invariantMass);      //concord charge
            break;
          case 2: // j = k+
            dPartInvMass->Fill(invariantMass);  //p- / k+
          case 0: //j = p+
          case 4: //j = pr+
            dInvMass->Fill(invariantMass);      //discord charge
            break;
          default:
            std::cerr << "\nSTRANGE ERROR: particella[" << j << "] (j) has an indefined index\n";
          }
          break;

        case 2:   //i = k+
          switch (jIndex)
          {
          case 0: //j = p+
            cPartInvMass->Fill(invariantMass);  //p+ / k+
          case 2: //j = k+
          case 4: //j = pr+
            cInvMass->Fill(invariantMass);      //concord charge
            break;
          case 1: // j = p-
            dPartInvMass->Fill(invariantMass);  //p- /k+
          case 3: //j = k-
          case 5: //j = pr-
            dInvMass->Fill(invariantMass);      //discord charge
            break;
          default:
            std::cerr << "\nSTRANGE ERROR: particella[" << j << "] (j) has an indefined index\n";
          }
          break;

        case 3:   //i = k-
          switch (jIndex)
          {
          case 1: //j = p-
            cPartInvMass->Fill(invariantMass);  //p- / k-
          case 3: //j = k-
          case 5: //j = pr-
            cInvMass->Fill(invariantMass);      //concord charge
            break;
          case 0: // j = p+
            dPartInvMass->Fill(invariantMass);  //p+ / k-
          case 2: //j = k+
          case 4: //j = pr+
            dInvMass->Fill(invariantMass);      //discord charge
            break;
          default:
            std::cerr << "\nSTRANGE ERROR: particella[" << j << "] (j) has an indefined index\n";
          }
          break;

        case 4:   //i = pr+
          switch (jIndex)
          {
          case 0: //j = p+
          case 2: //j = k+
          case 4: //j = pr+
            cInvMass->Fill(invariantMass);      //concord charge
            break;
          case 1: //j = p-
          case 3: //j = k-
          case 5: //j = pr-
            dInvMass->Fill(invariantMass);      //discord charge
            break;
          default:
            std::cerr << "\nSTRANGE ERROR: particella[" << j << "] (j) has an indefined index\n";
          }
          break;

        case 5:   //i = pr-
          switch (jIndex)
          {
          case 1: //j = p-
          case 3: //j = k-
          case 5: //j = pr-
            cInvMass->Fill(invariantMass);      //concord charge
            break;
          case 0: //j = p+
          case 2: //j = k+
          case 4: //j = pr+
            dInvMass->Fill(invariantMass);      //discord charge
            break;
          default:
            std::cerr << "\nSTRANGE ERROR: particella[" << j << "] (j) has an indefined index\n";
          }
          break;
        default:
          std::cerr << "\nSTRANGE ERROR: particella[" << i << "] (i) has an indefined index\n";
          break;
        }
      }
    }

    //Calculating invariant mass for particle generated by K* decay and adding it to histo
    for (int i = 100; i < 100+kCounter; i += 2) {
      decadedInvMass->Fill(particella[i].invMass(particella[i+1]));
    }

  }   //end of event loop

  //Drawing all the histos
  phiDist->SetMinimum(0);
  thetaDist->SetMinimum(0);

  TCanvas* typesCanvas = new TCanvas("typesCanvas", "Particle type distribution");
  TCanvas* impulseCanvas = new TCanvas("impulseCanvas", "Impulse distribution");
  impulseCanvas->Divide(2,2);
  TCanvas* energyCanvas = new TCanvas("energyCanvas", "Energy distribution");
  TCanvas* invMassCanvas = new TCanvas("invariantCanvas", "Invariant mass distributions");
  invMassCanvas->Divide(3,2);

  typesCanvas->cd();    types->DrawCopy();
  impulseCanvas->cd(1); phiDist->DrawCopy();
  impulseCanvas->cd(2); thetaDist->DrawCopy();
  impulseCanvas->cd(3); impulseDist->DrawCopy();
  impulseCanvas->cd(4); impulseXYDist->DrawCopy();
  energyCanvas->cd();   energyDist->DrawCopy();
  invMassCanvas->cd(1); invMass->DrawCopy();
  invMassCanvas->cd(2); dInvMass->DrawCopy();
  invMassCanvas->cd(3); cInvMass->DrawCopy();
  invMassCanvas->cd(4); dPartInvMass->DrawCopy();
  invMassCanvas->cd(5); cPartInvMass->DrawCopy();
  invMassCanvas->cd(6); decadedInvMass->DrawCopy();

  //Saving all the histos in a ROOT file
  file->Write();
  file->Close();
  std::cout << "ROOT file saved.\n";
}
