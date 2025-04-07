#define MRDF
#include "MALICE.h"
#include "MHead.h"
#include "MHist.h"
#include "MRootIO.h"
#include "TApplication.h"

funcWithJson(void, ME_RR)(TString path_config = "../config.json") {
  SetUpJson(path_config.Data());
  Configurable<string> config_pathInputFile(
      "path_input", " /home/szhu/work/alice/analysis/PairFlow/data/pairflow/"
                    "O2dqflowpairpr.root");
  Configurable<string> config_pathOutputFile(
      "path_output",
      "/home/szhu/work/alice/analysis/PairFlow/output/ME_RR.root");

  TFile *file_event = TFile::Open(config_pathInputFile.data.c_str());
  TFile *fOutput = new TFile(config_pathOutputFile.data.c_str(), "RECREATE");

  TTree *tree_event = (TTree *)file_event->Get("O2dqflowpairrr");

  vector<RResultHandle> gRResultHandlesFast;
  ROOT::RDataFrame rdf(*tree_event);

  /* #region rdf_all definition */
  auto rdf_all = rdf.Define("DeltaPhi",
                            [](float phi, float phi_ref) {
                              double delta = phi - phi_ref;
                              while (delta > 1.5 * M_PI)
                                delta -= 2 * M_PI;
                              while (delta < -0.5 * M_PI)
                                delta += 2 * M_PI;
                              return delta;
                            },
                            {"fPhi1", "fPhi2"})
                     .Define("DeltaEta",
                             [](float eta, float eta_ref) {
                               double delta = eta - eta_ref;
                               return delta;
                             },
                             {"fEta1", "fEta2"});
  /* #endregion */

  /* #region histo4qa definition */
  auto h_DeltaPhi = rdf_all.Histo1D("DeltaPhi");
  gRResultHandlesFast.push_back(h_DeltaPhi);
  auto h_DeltaEta = rdf_all.Histo1D("DeltaEta");
  gRResultHandlesFast.push_back(h_DeltaEta);
  /* #endregion */

  /* #region axis definition */
  StrVar4Hist var_DeltaEta("DeltaEta", "#Delta #eta", "rapidity", 20,
                           {-4., 4.});
  StrVar4Hist var_DeltaPhi("DeltaPhi", "#Delta #phi", "rad", 30,
                           {-M_PI_2, M_PI + M_PI_2});
  StrVar4Hist var_VtxZ("fVtxZ", "V_{Z}", "cm", 20, {8, -10, 10});

  /* #endregion */
  vector<StrVar4Hist> vec_vars = {var_DeltaEta, var_DeltaPhi, var_VtxZ};

  /* #region macro definition */
#define RHistDefine2DLoop(df, v1, v2, cond)                                    \
  TString title = Form("%s_%s%s;%s (%s);%s (%s)", v1.fName.Data(),             \
                       v2.fName.Data(), cond.Data(), v1.fTitle.Data(),         \
                       v1.fUnit.Data(), v2.fTitle.Data(), v2.fUnit.Data());    \
  TH2DModel h2_mult(                                                           \
      Form("%s_%s%s", v1.fName.Data(), v2.fName.Data(), cond.Data()), title,   \
      v1.fNbins, v1.fBins.data(), v2.fNbins, v2.fBins.data());                 \
  gRResultHandlesFast.push_back(df.Histo2D(h2_mult, v1.fName, v2.fName));
  // RHistDefine3DLoop
#define RHistDefine3DLoop(df, v1, v2, v3, cond)                                \
  TString title = Form("%s_%s_%s%s;%s (%s);%s (%s);%s (%s)", v1.fName.Data(),  \
                       v2.fName.Data(), v3.fName.Data(), cond.Data(),          \
                       v1.fTitle.Data(), v1.fUnit.Data(), v2.fTitle.Data(),    \
                       v2.fUnit.Data(), v3.fTitle.Data(), v3.fUnit.Data());    \
  TH3DModel h3_mult(Form("%s_%s_%s%s", v1.fName.Data(), v2.fName.Data(),       \
                         v3.fName.Data(), cond.Data()),                        \
                    title, v1.fNbins, v1.fBins.data(), v2.fNbins,              \
                    v2.fBins.data(), v3.fNbins, v3.fBins.data());              \
  gRResultHandlesFast.push_back(                                               \
      df.Histo3D(h3_mult, v1.fName, v2.fName, v3.fName));

  /* #endregion */

  RHistDefine2DLoop(rdf_all, vec_vars[0], vec_vars[1], gEmptyString);
  RHistDefine3DLoop(rdf_all, vec_vars[0], vec_vars[1], vec_vars[2],
                    gEmptyString);

  RunGraphs(gRResultHandlesFast);

  fOutput->cd();
  RResultWrite(gRResultHandlesFast);
  fOutput->Close();
}

int main(int argc, char **argv) {
  if (argc == 1)
    ME_RR();
  else
    ME_RR(argv[1]);
  return 0;
}