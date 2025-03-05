#include "MHead.h"
#include "TLegend.h"

typedef struct StrGausFit {
  TH1D *hMean;
  TH1D *hSigma;
} StrGausFit;

StrGausFit AddFit(TH2 *h2d, double limit_low, double limit_high) {
  // TF1 *f1 = new TF1("f1", "gaus"); change it to double
  TF1 *f1 = new TF1("f1", "gaus");
  f1->SetRange(limit_low, limit_high);
  TObjArray aSlices_Electron;

  h2d->FitSlicesY(f1, 0, -1, 0, "QNR", &aSlices_Electron);
  // lustre/alice/users/szhu/work/test/AnalysisFramework/output/
  // aSlices_Electron.SetOwner(0);

  //--- Three objects: Mean, Sigma, Chi2
  TH1D *hMean = (TH1D *)aSlices_Electron.At(1)->Clone();
  TH1D *hSigma = (TH1D *)aSlices_Electron.At(2)->Clone();

  hMean->SetLineColor(kRed);
  hSigma->SetLineColor(kBlue);

  h2d->GetListOfFunctions()->Add(hMean, "same");
  h2d->GetListOfFunctions()->Add(hSigma, "same");

  h2d->Write();

  delete f1;
  return {hMean, hSigma};
}

void Comparison();

void PidSeparation() {
  TFile *file_NN = new TFile("/home/szhu/work/alice/pid_study/data/NN.root");
  TFile *file_BB = new TFile("/home/szhu/work/alice/pid_study/data/BB.root");
  TFile *file_output = new TFile(
      "/home/szhu/work/alice/pid_study/output/PidSeparation.root", "RECREATE");

#define h2d_fit(NNorBB, particle, v1, v2, ncls, high, low)                     \
  TH2D *h##v1##_##v2##_##particle##ncls##NNorBB =                              \
      (TH2D *)file_##NNorBB->Get(#v1 "_" #v2 "_" #particle #ncls);             \
  h##v1##_##v2##_##particle##ncls##NNorBB->SetName(                            \
      Form("%s_%s_%s%d%s", #v1, #v2, #particle, ncls, #NNorBB));               \
  h##v1##_##v2##_##particle##ncls##NNorBB->SetTitle(Form(                      \
      "%s_%s", #NNorBB, h##v1##_##v2##_##particle##ncls##NNorBB->GetTitle())); \
  StrGausFit str_##v1##_##v2##_##particle##ncls##NNorBB =                      \
      AddFit(h##v1##_##v2##_##particle##ncls##NNorBB, low, high);

  file_output->cd();
  // fTgl, fFt0Occ: fTPCSignal, DeltaDeDx, fNSigTPC
  h2d_fit(NN, Electron, fTgl, fTPCSignal, 0, 60, 100);
  h2d_fit(NN, Electron, fTgl, fTPCSignal, 1, 60, 100);
  h2d_fit(NN, Pion, fTgl, fTPCSignal, 0, 40, 60);
  h2d_fit(NN, Pion, fTgl, fTPCSignal, 1, 40, 60);
  h2d_fit(NN, Electron, fFt0Occ, fTPCSignal, 0, 60, 100);
  h2d_fit(NN, Electron, fFt0Occ, fTPCSignal, 1, 60, 100);
  h2d_fit(NN, Pion, fFt0Occ, fTPCSignal, 0, 40, 60);
  h2d_fit(NN, Pion, fFt0Occ, fTPCSignal, 1, 40, 60);
  h2d_fit(BB, Electron, fTgl, fTPCSignal, 0, 60, 100);
  h2d_fit(BB, Electron, fTgl, fTPCSignal, 1, 60, 100);
  h2d_fit(BB, Pion, fTgl, fTPCSignal, 0, 40, 60);
  h2d_fit(BB, Pion, fTgl, fTPCSignal, 1, 40, 60);
  h2d_fit(BB, Electron, fFt0Occ, fTPCSignal, 0, 60, 100);
  h2d_fit(BB, Electron, fFt0Occ, fTPCSignal, 1, 60, 100);
  h2d_fit(BB, Pion, fFt0Occ, fTPCSignal, 0, 40, 60);
  h2d_fit(BB, Pion, fFt0Occ, fTPCSignal, 1, 40, 60);

  h2d_fit(NN, Electron, fTgl, DeltaDeDx, 0, -20, 20);
  h2d_fit(NN, Electron, fTgl, DeltaDeDx, 1, -20, 20);
  h2d_fit(NN, Pion, fTgl, DeltaDeDx, 0, -20, 20);
  h2d_fit(NN, Pion, fTgl, DeltaDeDx, 1, -20, 20);
  h2d_fit(NN, Electron, fFt0Occ, DeltaDeDx, 0, -20, 20);
  h2d_fit(NN, Electron, fFt0Occ, DeltaDeDx, 1, -20, 20);
  h2d_fit(NN, Pion, fFt0Occ, DeltaDeDx, 0, -20, 20);
  h2d_fit(NN, Pion, fFt0Occ, DeltaDeDx, 1, -20, 20);
  h2d_fit(BB, Electron, fTgl, DeltaDeDx, 0, -20, 20);
  h2d_fit(BB, Electron, fTgl, DeltaDeDx, 1, -20, 20);
  h2d_fit(BB, Pion, fTgl, DeltaDeDx, 0, -20, 20);
  h2d_fit(BB, Pion, fTgl, DeltaDeDx, 1, -20, 20);
  h2d_fit(BB, Electron, fFt0Occ, DeltaDeDx, 0, -20, 20);
  h2d_fit(BB, Electron, fFt0Occ, DeltaDeDx, 1, -20, 20);
  h2d_fit(BB, Pion, fFt0Occ, DeltaDeDx, 0, -20, 20);
  h2d_fit(BB, Pion, fFt0Occ, DeltaDeDx, 1, -20, 20);
  file_output->Close();
  Comparison();
}

void Comparison() {
  TFile *file =
      new TFile("/home/szhu/work/alice/pid_study/output/PidSeparation.root");
  TFile *file_output = new TFile(
      "/home/szhu/work/alice/pid_study/output/Comparison.root", "RECREATE");

#define h2d_compare(particle, v1, v2, ncls)                                    \
  TCanvas *c =                                                                 \
      new TCanvas(Form("c_%s_%s_%s%d", v1, v2, particle, ncls),                \
                  Form("%s_%s_%s%d", v1, v2, particle, ncls), 800, 600);       \
  c->Divide(2, 2);                                                             \
  TH2D *hNN = (TH2D *)file->Get(Form("%s_%s_%s%dNN", v1, v2, particle, ncls)); \
  TH2D *hBB = (TH2D *)file->Get(Form("%s_%s_%s%dBB", v1, v2, particle, ncls)); \
  TH1D *mean_nn = (TH1D *)hNN->GetListOfFunctions()->At(0)->Clone();           \
  TH1D *mean_bb = (TH1D *)hBB->GetListOfFunctions()->At(0)->Clone();           \
  TH1D *sigma_nn = (TH1D *)hNN->GetListOfFunctions()->At(1)->Clone();          \
  TH1D *sigma_bb = (TH1D *)hBB->GetListOfFunctions()->At(1)->Clone();          \
  c->cd(1);                                                                    \
  hNN->Draw("colz");                                                           \
  c->cd(2);                                                                    \
  hBB->Draw("colz");                                                           \
  c->cd(3);                                                                    \
  mean_nn->SetLineColor(kRed);                                                 \
  mean_bb->SetLineColor(kBlue);                                                \
  mean_nn->Draw();                                                             \
  mean_bb->Draw("same");                                                       \
  c->cd(4);                                                                    \
  sigma_nn->SetLineColor(kRed);                                                \
  sigma_bb->SetLineColor(kBlue);                                               \
  sigma_nn->Draw("same");                                                      \
  sigma_bb->Draw("same");                                                      \
  TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9);                              \
  leg->AddEntry(mean_nn, "NN", "l");                                           \
  leg->AddEntry(mean_bb, "BB", "l");                                           \
  leg->Draw();                                                                 \
  c->SaveAs(                                                                   \
      Form("/home/szhu/work/alice/pid_study/output/plotting/%s_%s_%s%d.pdf",   \
           v1, v2, particle, ncls));

  vector<const char *> particles = {"Electron", "Pion"};
  vector<const char *> v1s = {"fTgl", "fFt0Occ"};
  vector<const char *> v2s = {"fTPCSignal", "DeltaDeDx"};
  vector<int> nclss = {0, 1};

  for (auto particle : particles) {
    for (auto v1 : v1s) {
      for (auto v2 : v2s) {
        for (auto ncls : nclss) {
          h2d_compare(particle, v1, v2, ncls);
        }
      }
    }
  }
}
