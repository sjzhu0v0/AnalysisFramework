#include "MALICE.h"
#include "MHead.h"
#include "MHist.h"
#include "MRootGraphic.h"
#include "TApplication.h"
#include "TStyle.h"

void SetUpGStyle(TStyle *style = gStyle) { style->SetOptStat(0); }

funcWithJson(void, plot_PR)(TString path_config = "../config.json") {
  SetUpJson(path_config.Data());
  SetUpGStyle();

  Configurable<string> config_pathSEInputFile(
      "path_input_se",
      "/home/szhu/work/alice/analysis/PairFlow/output/SE_PR.root");
  Configurable<string> config_pathMEInputFile(
      "path_input_me",
      "/home/szhu/work/alice/analysis/PairFlow/output/ME_PR.root");

  // DeltaPhi_DeltaEta
  TFile *file_input = TFile::Open(config_pathSEInputFile.data.c_str());
  TH2D *h_DeltaPhi_DeltaEta_SE = (TH2D *)file_input->Get("DeltaEta_DeltaPhi");
  TFile *file_input_me = TFile::Open(config_pathMEInputFile.data.c_str());
  TH2D *h_DeltaPhi_DeltaEta_ME =
      (TH2D *)file_input_me->Get("DeltaEta_DeltaPhi");

  TH2D *h_se2me = (TH2D *)h_DeltaPhi_DeltaEta_SE->Clone("h_se2me");
  h_se2me->SetTitle("R(#Delta #eta, #Delta #phi)");
  h_se2me->SetZTitle("R(#Delta #eta, #Delta #phi)");
  h_se2me->Divide(h_DeltaPhi_DeltaEta_ME);

  h_se2me->GetXaxis()->SetRangeUser(-2., 2.);
  // h_se2me->Scale(1. / h_se2me->Integral());
  // TCanvas *c_DeltaPhi_DeltaEta =
  //     new TCanvas("c_DeltaPhi_DeltaEta", "c_DeltaPhi_DeltaEta", 600, 600);
  // StyleFlow::DeltaPhi_DeltaEta(c_DeltaPhi_DeltaEta, h_DeltaPhi_DeltaEta);

  TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
  c1->Divide(2, 2);
  // get the first pad
  TPad *pad1 = (TPad *)c1->cd(1);
  StyleFlow::DeltaPhi_DeltaEta(pad1, h_DeltaPhi_DeltaEta_SE);
  TPad *pad2 = (TPad *)c1->cd(2);
  StyleFlow::DeltaPhi_DeltaEta(pad2, h_DeltaPhi_DeltaEta_ME);
  TPad *pad3 = (TPad *)c1->cd(3);
  StyleFlow::DeltaPhi_DeltaEta(pad3, h_se2me);

  TCanvas *c2 = new TCanvas("c2", "c2", 600, 600);
  StyleFlow::DeltaPhi_DeltaEta(c2, h_se2me);
}
int main(int argc, char **argv) {
  TString path_config = "../config.json";
  if (argc != 1)
    path_config = argv[1];
  TApplication *app = new TApplication("app", &argc, argv);
  plot_PR(path_config);
  app->Run();
  return 0;
}