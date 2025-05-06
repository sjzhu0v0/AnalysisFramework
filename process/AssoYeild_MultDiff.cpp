#include "MHelper.h"
#include "MRootGraphic.h"
#include "TApplication.h"

void AssoYeild_MultDiff(
    TString path_file_input =
        "/home/szhu/data/PairFlow/22pass4_highIR/raw/result_r_bs.root",
    TString path_file_output = "/home/szhu/work/alice/analysis/output/process/"
                               "AssoYeild_MultDiff.pdf") {
  // TApplication *app = new TApplication("app", 0, nullptr);
  TFile *file_input = new TFile(path_file_input);
//   TFile *file_output = new TFile(path_file_output, "RECREATE");
  MHGroupTool1D *hgroupTool1d = new MHGroupTool1D(
      file_input, "h2_int_mass%d_pt%d_mult%d",
      {glib_vars[kMass], glib_vars[kPt], glib_vars[kNumContrib]}, {4, 2, 5});
  // auto h1 = hgroupTool1d->GetHist({1, 1, 1});
  // auto h2 = hgroupTool1d->GetHist({1, 1, 2});
  // h1->Draw();
  // h2->Draw("same");
  AssocYeildSub_v1 *hgroupTool1d_sub = new AssocYeildSub_v1(hgroupTool1d);
  delete hgroupTool1d;
  //   hgroupTool1d_sub->Print();
  hgroupTool1d_sub->Rebin(3);
  hgroupTool1d_sub->SetMultBin(1, 2);
  //   auto h1 = hgroupTool1d_sub->GetAssoYeild_Mass(2, 2);
  //   h1->Draw();
  //   TCanvas *c1 = new TCanvas("c1", "c1", 1500, 600);
  //   c1->Divide(5, 2);
  //   int nbins_phi = hgroupTool1d_sub->GetNBins(0);
  //   for (int iPhi = 1; iPhi <= nbins_phi; iPhi++) {
  //     c1->cd(iPhi);
  //     auto h1 = hgroupTool1d_sub->GetAssoYeild_Mass(iPhi, 2);
  //     h1->SetTitle(Form("Associated Yield: %.2f < #phi < %.2f",
  //                       (double)(iPhi - 1) / nbins_phi * 2 * TMath::Pi() -
  //                       M_PI_2, (double)iPhi / nbins_phi * 2 * TMath::Pi() -
  //                       M_PI_2));
  //     h1->Draw();
  //   }

  gPublisherCanvas = new MPublisherCanvas(path_file_output, 5, 2, 600, 300,
                                          "c1", "Associated Yield");
  for (int ipt = 1; ipt <= 5; ipt++) {
    int nbins_phi = hgroupTool1d_sub->GetNBins(0);
    for (int iPhi = 1; iPhi <= nbins_phi; iPhi++) {
      gPublisherCanvas->NewPad()->cd();
      auto h1 = hgroupTool1d_sub->GetAssoYeild_Mass(iPhi, ipt);
      h1->SetTitle(
          Form("Associated Yield: %.2f < #phi < %.2f",
               (double)(iPhi - 1) / nbins_phi * 2 * TMath::Pi() - M_PI_2,
               (double)iPhi / nbins_phi * 2 * TMath::Pi() - M_PI_2));
      h1->Draw();
    }
    gPublisherCanvas->NextPage();
  }
  gPublisherCanvas->finalize();
}