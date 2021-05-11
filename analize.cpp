#include <iostream>
#include <TFile.h>
#include <TH1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TF1.h>

int main() {
  TFile* file = new TFile("data.root");
  
  TH1F* types = (TH1F*)file->Get("types");
  TH1F* phiDist = (TH1F*)file->Get("phi");
  TH1F* thetaDist = (TH1F*)file->Get("theta");
  TH1F* impulseDist = (TH1F*)file->Get("impulse");
  TH1F* dInvMass = (TH1F*)file->Get("discordInvMass");        //1. segno discorde
  TH1F* cInvMass = (TH1F*)file->Get("concordInvMass");        //2. segno concorde
  TH1F* dPartInvMass = (TH1F*)file->Get("dParticleInvMass");  //3. p+ k- / p- k+
  TH1F* cPartInvMass = (TH1F*)file->Get("cParticleInvMass");  //4. p+ k+ / p- k-
  TH1F* decadedInvMass = (TH1F*)file->Get("decadedInvMass");  //5. generated from K* decay

  types->SetXTitle("Type of particle");
  types->SetYTitle("Occurrences");
  phiDist->SetXTitle("Degree (rad)");
  phiDist->SetYTitle("Occurences");
  thetaDist->SetXTitle("Degree (rad)");
  thetaDist->SetYTitle("Occurences");
  impulseDist->SetXTitle("Impulse (GeV)");
  impulseDist->SetYTitle("Occurences");
  decadedInvMass->SetXTitle("Invarian mass (GeV/c^2)");
  decadedInvMass->SetYTitle("Occurences");

  TCanvas* canvas1 = new TCanvas("First canvas", "Types and impulse");
  canvas1->Divide(2,2);

  if (types->GetBinContent(0) > 0 || types->GetBinContent(8) > 0)
    std::cerr << "\nERROR: It has been generated an anknown particle type\n";
  
  int entries = types->GetEntries();

  std::cout << "\n-Histo types-\n";
  std::cout << "Number of entries: " << entries << '\n';
  for (int i = 0; i < 7; i++) {
    int typeCounter = types->GetBinContent(i+1);
    std::cout << "Entries of type " << i << ": " << typeCounter << " +/- " << types->GetBinError(i+1) << "\t(" << typeCounter*100.0/entries << " % of total)\n";
  } 
  canvas1->cd(1); types->Draw();   //Draw types on canvas 1
  
  std::cout << "\n-Fitting of impulse with an exponential-\n\n";
  canvas1->cd(2); impulseDist->Fit("expo");   //Fit Impulse and Draw on canvas 1

  std::cout << "\n-Fitting of angles with a uniform distribution-\n";
  canvas1->cd(3); phiDist->Fit("pol0");       //Fit angles and Draw on canvas 1
  canvas1->cd(4); thetaDist->Fit("pol0");

  //Invariant mass
  gStyle->SetOptFit(111);
  TCanvas* canvas2 = new TCanvas("Second canvas", "Invariant mass");
  canvas2->Divide(3);

  TH1F* subtraction12 = new TH1F(*dInvMass);
  subtraction12->SetTitle("(different charge sign) minus (same charge sign)");
  subtraction12->SetName("1 minus 2");
  subtraction12->SetXTitle("Impulse (GeV)");
  subtraction12->SetYTitle("Occurences");
  subtraction12->Sumw2(1);
  subtraction12->Add(dInvMass, cInvMass, 1,-1);
  
  TH1F* subtraction34 = new TH1F(*dPartInvMass);
  subtraction34->SetTitle("(p+ k- / p- k+) minus (p+ k+ / p- k-)");
  subtraction34->SetName("3 minus 4");
  subtraction34->SetXTitle("Impulse (GeV)");
  subtraction34->SetYTitle("Occurences");
  subtraction34->Sumw2(1);
  subtraction34->Add(dPartInvMass, cPartInvMass, 1,-1);

  std::cout << "\n-Fitting Decaded invariant mass with a gaussian-\n\n";
  canvas2->cd(1); decadedInvMass->Fit("gaus");

  std::cout << "\n-Fitting (different charge sign) minus (same charge sign)) with a gaussian-\n\n";
  canvas2->cd(2); subtraction12->Fit("gaus");

  std::cout << "\n-Fitting (p+ k- / p- k+) minus (p+ k+ / p- k-) with a gaussian-\n\n";
  canvas2->cd(3); subtraction34->Fit("gaus");
  
  //Saving canvas
  canvas1->Print("Impulse.pdf");
  canvas1->Print("Impulse.C");
  canvas1->Print("Impulse.root");
  canvas2->Print("Subtractions.pdf");
  canvas2->Print("Subtractions.C");
  canvas2->Print("Subtractions.root");
}