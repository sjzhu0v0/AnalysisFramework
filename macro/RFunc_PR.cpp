#define MRDF
#include "MALICE.h"
#include "MHead.h"
#include "MHelper.h"
#include "MRootGraphic.h"
#include "TApplication.h"
#include "TStyle.h"

funcWithJson(void, RFunc_PR)(TString path_config = "../config.json") {
  gStyle->SetOptStat(0);
  SetUpJson(path_config.Data());
  Configurable<string> config_pathInputFile_me(
      "path_input_me",
      " /home/szhu/work/alice/analysis/PairFlow/test/data/group1.root");
  Configurable<string> config_pathInputFile_se(
      "path_input_se",
      "/home/szhu/work/alice/analysis/PairFlow/test/data/group1_bs_1.root");
  Configurable<string> config_pathOutputFile(
      "path_output",
      "/home/szhu/work/alice/analysis/PairFlow/output/RFunc.root");
  Configurable<double> config_deltaEta("delta_eta", 1.);
  Configurable<double> config_nRebin_mult("nRebin_mult", 5.);

  double deltaEta = config_deltaEta.data;
  TFile *file_input_se = TFile::Open(config_pathInputFile_se.data.c_str());
  TFile *file_input_me = TFile::Open(config_pathInputFile_me.data.c_str());
  TFile *file_output =
      new TFile(config_pathOutputFile.data.c_str(), "RECREATE");

  auto hn_se = (THnD *)file_input_se->Get(
      "DeltaEta_DeltaPhi_VtxZ_Mass_Pt_NumContribCalib");
  auto hn_trigger = (THnD *)file_input_se->Get("VtxZ_Mass_Pt_NumContribCalib");
  auto hn_me = (THnD *)file_input_me->Get("DeltaEta_DeltaPhi_VtxZ_Mass_Pt");

  MHnTool *hntool_se = new MHnTool(hn_se);
  MHnTool *hntool_me = new MHnTool(hn_me);
  MHnTool *hntool_trigger = new MHnTool(hn_trigger);

  AssocYeildHelper_v1 assoYeildHelper(hntool_se, hntool_me, hntool_trigger);
  assoYeildHelper.Rebin(gtype_vars::kMass, 2);
  assoYeildHelper.Rebin(gtype_vars::kPt, 2);
  assoYeildHelper.Rebin(gtype_vars::kNumContrib, config_nRebin_mult.data);
  // auto h2 = assoYeildHelper.AssociatedYeild(0, 0, 0);

  // TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
  // StyleFlow::DeltaPhi_DeltaEta(c1, h2);

  file_output->cd();
  int total = glib_vars[kMass].fNbins * glib_vars[kPt].fNbins *
              glib_vars[kNumContrib].fNbins / 4;
  for (int iMass = 1; iMass <= assoYeildHelper.GetNBins(kMass); iMass++) {
    for (int iPt = 1; iPt <= assoYeildHelper.GetNBins(kPt); iPt++) {
      for (int iMult = 1; iMult <= assoYeildHelper.GetNBins(kNumContrib);
           iMult++) {
        // process bar in percentage
        int index = (iMass - 1) * glib_vars[kPt].fNbins *
                        glib_vars[kNumContrib].fNbins / 2 +
                    (iPt - 1) * glib_vars[kNumContrib].fNbins + iMult - 1;
        if (index % 10 == 0) {
          double percent = (double)index / (double)total * 100;
          cout << "\rProcessing: " << percent << "%";
          cout.flush();
        }
        auto h2 = assoYeildHelper.AssociatedYeild(iMass, iPt, iMult);
        auto h1 = assoYeildHelper.AssociatedYeild(deltaEta, iMass, iPt, iMult);
        int bin1 = h2->GetXaxis()->FindBin(-deltaEta) - 1;
        int bin2 = h2->GetXaxis()->FindBin(deltaEta);
        auto h1_sub1 =
            h2->ProjectionY(Form("h1_sub_%d", GenerateUID()), 1, bin1);
        auto h1_sub2 = h2->ProjectionY(Form("h1_sub_%d", GenerateUID()), bin2,
                                       h2->GetNbinsY());
        h1_sub1->Add(h1_sub2);
        h2->SetName(Form("h2_asso_mass%d_pt%d_mult%d", iMass, iPt, iMult));
        h2->Write();
        h2->Delete();
        h1->SetName(Form("h1_asso_mass%d_pt%d_mult%d", iMass, iPt, iMult));
        h1->Write();
        h1->Delete();
        h1_sub1->SetName(Form("h2_int_mass%d_pt%d_mult%d", iMass, iPt, iMult));
        h1_sub1->Write();
        h1_sub1->Delete();
        h1_sub2->Delete();
      }
    }
  }
  cout << "\rProcessing: 100%" << endl;

  file_output->Close();
}
int main(int argc, char **argv) {
  TString path_config = "../config.json";
  if (argc != 1)
    path_config = argv[1];
  // TApplication *app = new TApplication("app", &argc, argv);
  RFunc_PR(path_config);
  // app->Run();
  return 0;
}