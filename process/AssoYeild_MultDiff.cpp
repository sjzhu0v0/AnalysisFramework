#include "MHelper.h"
#include "TApplication.h"

void AssoYeild_MultDiff(
    TString path_file_input =
        "/home/szhu/data/PairFlow/22pass4_highIR/raw/result_r_bs.root",
    TString path_file_output = "/home/szhu/work/alice/analysis/output/process/"
                               "AssoYeild_MultDiff.root") {
  // TApplication *app = new TApplication("app", 0, nullptr);
  TFile *file_input = new TFile(path_file_input);
  TFile *file_output = new TFile(path_file_output, "RECREATE");
  MHGroupTool1D *hgroupTool1d = new MHGroupTool1D(
      file_input, "h2_int_mass%d_pt%d_mult%d",
      {glib_vars[kMass], glib_vars[kPt], glib_vars[kNumContrib]}, {2, 2, 1});
  // auto h1 = hgroupTool1d->GetHist({1, 1, 1});
  // auto h2 = hgroupTool1d->GetHist({1, 1, 2});
  // h1->Draw();
  // h2->Draw("same");
  AssocYeildSub_v1 *hgroupTool1d_sub = new AssocYeildSub_v1(hgroupTool1d);
  delete hgroupTool1d;
  hgroupTool1d_sub->Rebin(3);
  hgroupTool1d_sub->SetMultBin(1, 10);
  auto h1 = hgroupTool1d_sub->GetAssoYeild_Mass(2,2);
  h1->Draw();
}