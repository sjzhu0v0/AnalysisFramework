#include "MALICE.h"
#include "MHead.h"
#include "MHelper.h"
#include "MRootGraphic.h"
#include "TApplication.h"
#include "TProfile.h"
#include "TStyle.h"

void SetGStyle(TStyle *style = gStyle) { style->SetOptStat(0); }

void control_pr_deltaEta_deltaPhi_integral(
    TString path_se_input =
        "/data/PairFlow/22pass4_highIR/sample/se_pr_thn.root",
    TString path_me_input =
        "/data/PairFlow/22pass4_highIR/sample/me_pr_thn.root",
    TString path_output = "./results/control") {
  SetGStyle();

  TFile *file_input_se = TFile::Open(path_se_input);
  TFile *file_input_me = TFile::Open(path_me_input);

  auto hn_se = (THnD *)file_input_se->Get(
      "DeltaEta_DeltaPhi_VtxZ_Mass_Pt_NumContribCalib");
  auto hn_trigger = (THnD *)file_input_se->Get("VtxZ_Mass_Pt_NumContribCalib");
  auto hn_me = (THnD *)file_input_me->Get("DeltaEta_DeltaPhi_VtxZ_Mass_Pt");

  MHnTool *hntool_se = new MHnTool(hn_se);
  MHnTool *hntool_me = new MHnTool(hn_me);
  MHnTool *hntool_trigger = new MHnTool(hn_trigger);

  AssocYeildHelper_v1 assoYeildHelper(hntool_se, hntool_me, hntool_trigger);
  assoYeildHelper.Rebin(gtype_vars::kNumContrib, 5);

  auto h2 = assoYeildHelper.AssociatedYeild(0, 0, 0);
  h2->SetTitle("mult integral");
  h2->GetZaxis()->SetRangeUser(0, 1.5);
  h2->GetXaxis()->SetRangeUser(-1.8, 1.8);
  auto h2_lowMult = assoYeildHelper.AssociatedYeild(0, 0, 1);
  h2_lowMult->SetTitle("low mult");
  h2_lowMult->GetZaxis()->SetRangeUser(0, 1.5);
  h2_lowMult->GetXaxis()->SetRangeUser(-1.8, 1.8);
  auto h2_highMult = assoYeildHelper.AssociatedYeild(0, 0, 2);
  h2_highMult->SetTitle("high mult");
  h2_highMult->GetZaxis()->SetRangeUser(0, 1.5);
  h2_highMult->GetXaxis()->SetRangeUser(-1.8, 1.8);
  auto h2_highOnLow = (TH2D *)h2_highMult->Clone("h2_highOnLow");
  h2_highOnLow->SetTitle("high mult - low mult");
  HistSubstraction2D(h2_highOnLow, h2_highMult, h2_lowMult);

  TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
  c1->Divide(2, 2);
  TPad *pad1 = (TPad *)c1->cd(1);
  StyleFlow::DeltaPhi_DeltaEta(pad1, h2);
  TPad *pad2 = (TPad *)c1->cd(2);
  StyleFlow::DeltaPhi_DeltaEta(pad2, h2_lowMult);
  TPad *pad3 = (TPad *)c1->cd(3);
  StyleFlow::DeltaPhi_DeltaEta(pad3, h2_highMult);
  TPad *pad4 = (TPad *)c1->cd(4);
  StyleFlow::DeltaPhi_DeltaEta(pad4, h2_highOnLow);
  c1->cd(0);
  TLatex *tex = new TLatex(0.5, 0.95, "PR Asso Yield");
  tex->SetNDC();
  tex->SetTextSize(0.055);
  MRootGraphic::SetTextCentral(tex, 0.525);
  tex->Draw();
  c1->SaveAs(path_output + "/2d_asso_yeild.pdf");

  TCanvas *c1_improved = new TCanvas("c1_improved", "c1_improved", 1200, 600);
  c1_improved->Divide(2, 1);
  TPad *pad1_improved = (TPad *)c1_improved->cd(1);
  h2_lowMult->SetTitle("Low multiplicity");
  StyleFlow::DeltaPhi_DeltaEta(pad1_improved, h2_lowMult);
  gPad->SetTopMargin(0.1);
  gPad->SetBottomMargin(0.1);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  TPad *pad2_improved = (TPad *)c1_improved->cd(2);
  h2_highMult->SetTitle("High multiplicity");
  StyleFlow::DeltaPhi_DeltaEta(pad2_improved, h2_highMult);
  gPad->SetTopMargin(0.1);
  gPad->SetBottomMargin(0.1);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);

  TCanvas *c2 = new TCanvas("c2", "c2", 600, 600);
  auto h1 = h2_highOnLow->ProjectionY("h1");
  c2->cd();
  h1->SetTitle("high mult - low mult");
  TF1 *f1 = new TF1("f1", "[0]+[1]*cos(x)+[2]*cos(2*x)+[3]*sin(3*x)",
                    -0.5 * M_PI, 1.5 * M_PI);
  h1->Fit(f1, "R");
  // get parameter
  double a0 = f1->GetParameter(0);
  double a1 = f1->GetParameter(1);
  double a2 = f1->GetParameter(2);
  double a3 = f1->GetParameter(3);

  h1->Draw();
  // draw all order
  TF1 *f2 = new TF1("f2", "[0]+[1]*cos(x)", -0.5 * M_PI, 1.5 * M_PI);
  f2->SetParameter(0, a0);
  f2->SetParameter(1, a1);
  f2->SetLineColor(kBlue);
  f2->Draw("same");
  TF1 *f3 = new TF1("f3", "[0]+[1]*cos(2*x)", -0.5 * M_PI, 1.5 * M_PI);
  f3->SetParameter(0, a0);
  f3->SetParameter(1, a2);
  f3->SetLineColor(kGreen + 2);
  f3->Draw("same");
  TF1 *f4 = new TF1("f4", "[0]+[1]*cos(3*x)", -0.5 * M_PI, 1.5 * M_PI);
  f4->SetParameter(0, a0);
  f4->SetParameter(1, a3);
  f4->SetLineColor(kYellow + 2);
  f4->Draw("same");
  c2->SaveAs(path_output + "/1d_asso_yeild.pdf");

  TCanvas *c3 = new TCanvas("c3", "c3", 600, 600);
  auto h1_deltaeta = h2_highOnLow->ProjectionX("h1_deltaeta");
  c3->cd();
  h1_deltaeta->SetTitle("high mult - low mult");
  h1_deltaeta->Draw();
}