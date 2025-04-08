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

  Configurable<string> config_pathSE_PR_InputFile(
      "path_input_SE_PR",
      "/home/szhu/work/alice/analysis/PairFlow/output/SE_PR.root");
  Configurable<string> config_pathME_PR_InputFile(
      "path_input_ME_PR",
      "/home/szhu/work/alice/analysis/PairFlow/output/ME_PR.root");
  Configurable<string> config_pathSE_RR_InputFile(
      "path_input_SE_RR",
      "/home/szhu/work/alice/analysis/PairFlow/output/SE_RR.root");
  Configurable<string> config_pathME_RR_InputFile(
      "path_input_ME_RR",
      "/home/szhu/work/alice/analysis/PairFlow/output/ME_RR.root");

  // DeltaPhi_DeltaEta
  TFile *file_input_pr = TFile::Open(config_pathSE_PR_InputFile.data.c_str());
  TH2D *h_DeltaPhi_DeltaEta_SE_PR =
      (TH2D *)file_input_pr->Get("DeltaEta_DeltaPhi");
  TFile *file_input_me_pr =
      TFile::Open(config_pathME_PR_InputFile.data.c_str());
  TH2D *h_DeltaPhi_DeltaEta_ME_PR =
      (TH2D *)file_input_me_pr->Get("DeltaEta_DeltaPhi");

  TH2D *h_se2me = (TH2D *)h_DeltaPhi_DeltaEta_SE_PR->Clone("h_se2me");

  h_se2me->SetTitle("R(#Delta #eta, #Delta #phi)");
  h_se2me->SetZTitle("R(#Delta #eta, #Delta #phi)");
  h_se2me->Divide(h_DeltaPhi_DeltaEta_ME_PR);

  h_se2me->GetXaxis()->SetRangeUser(-3, 3);
  h_se2me->Scale(h_DeltaPhi_DeltaEta_ME_PR->Integral() /
                 h_DeltaPhi_DeltaEta_SE_PR->Integral());

  TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
  c1->Divide(2, 2);
  // get the first pad
  TPad *pad1 = (TPad *)c1->cd(1);
  StyleFlow::DeltaPhi_DeltaEta(pad1, h_DeltaPhi_DeltaEta_SE_PR);
  TPad *pad2 = (TPad *)c1->cd(2);
  StyleFlow::DeltaPhi_DeltaEta(pad2, h_DeltaPhi_DeltaEta_ME_PR);
  TPad *pad3 = (TPad *)c1->cd(3);
  StyleFlow::DeltaPhi_DeltaEta(pad3, h_se2me);

  TCanvas *c2 = new TCanvas("c2", "c2", 600, 600);
  StyleFlow::DeltaPhi_DeltaEta(c2, h_se2me);

  TFile *file_input_rr = TFile::Open(config_pathSE_RR_InputFile.data.c_str());
  TH2D *h_DeltaPhi_DeltaEta_SE_RR =
      (TH2D *)file_input_rr->Get("DeltaEta_DeltaPhi");
  TFile *file_input_me_rr =
      TFile::Open(config_pathME_RR_InputFile.data.c_str());
  TH2D *h_DeltaPhi_DeltaEta_ME_RR =
      (TH2D *)file_input_me_rr->Get("DeltaEta_DeltaPhi");
  TH2D *h_se2me_rr = (TH2D *)h_DeltaPhi_DeltaEta_SE_RR->Clone("h_se2me_rr");
  h_se2me_rr->SetTitle("R(#Delta #eta, #Delta #phi)");
  h_se2me_rr->SetZTitle("R(#Delta #eta, #Delta #phi)");
  h_se2me_rr->Divide(h_DeltaPhi_DeltaEta_ME_RR);
  h_se2me_rr->GetXaxis()->SetRangeUser(-1.8, 1.8);
  h_se2me_rr->Scale(h_DeltaPhi_DeltaEta_ME_RR->Integral() /
                    h_DeltaPhi_DeltaEta_SE_RR->Integral());
  TCanvas *c3 = new TCanvas("c3", "c3", 600, 600);
  c3->Divide(2, 2);
  // get the first pad
  TPad *pad4 = (TPad *)c3->cd(1);
  StyleFlow::DeltaPhi_DeltaEta(pad4, h_DeltaPhi_DeltaEta_SE_RR);
  TPad *pad5 = (TPad *)c3->cd(2);
  StyleFlow::DeltaPhi_DeltaEta(pad5, h_DeltaPhi_DeltaEta_ME_RR);
  TPad *pad6 = (TPad *)c3->cd(3);
  StyleFlow::DeltaPhi_DeltaEta(pad6, h_se2me_rr);
  TCanvas *c4 = new TCanvas("c4", "c4", 600, 600);
  StyleFlow::DeltaPhi_DeltaEta(c4, h_se2me_rr);
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