#include "MHead.h"

typedef struct StrGausFit {
  TH1D *hMean;
  TH1D *hSigma;
} StrGausFit;

StrGausFit AddFit(TH2 *h2d, double limit_low, double limit_high) {
  // TF1 *f1 = new TF1("f1", "gaus"); change it to double
  TF1 *f1 = new TF1("f1", "gaus");
  f1->SetRange(limit_low, limit_high);
  TObjArray aSlices_electron;

  h2d->FitSlicesY(f1, 0, -1, 0, "QNR", &aSlices_electron);
  // lustre/alice/users/szhu/work/test/AnalysisFramework/output/
  // aSlices_electron.SetOwner(0);

  //--- Three objects: Mean, Sigma, Chi2
  TH1D *hMean = (TH1D *)aSlices_electron.At(1)->Clone();
  TH1D *hSigma = (TH1D *)aSlices_electron.At(2)->Clone();

  hMean->Write();
  hSigma->Write();

  delete f1;
  return {hMean, hSigma};
}

void PidSeparation() {
  TFile *file_NN = new TFile("/home/szhu/work/alice/pid_study/data/NN.root");
  TFile *file_BB = new TFile("/home/szhu/work/alice/pid_study/data/BB.root");

#define h2d_fit(NNorBB, particle, v1, v2, ncls, high, low)                     \
  TH2D *v1##_##v2##_##particle##ncls =                                         \
      (TH2D *)NNorBB->Get(#v1 "_" #v2 "_" #particle #ncls);                    \
  StrGausFit str_v1##_##v2##_##particle##ncls =                                    \
      AddFit(v1##_##v2##_##particle##ncls, low, high);
}