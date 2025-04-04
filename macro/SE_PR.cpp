#define MRDF
#include "MALICE.h"
#include "MHead.h"
#include "MHist.h"
#include "MRootIO.h"
#include "TApplication.h"

funcWithJson(void, SE_PR)(TString path_config = "../config.json") {
  SetUpJson(path_config.Data());
  Configurable<string> config_pathInputFile(
      "path_input",
      "/home/szhu/work/alice/analysis/data/pairflow/O2dqflowvecd.root");
  Configurable<string> config_pathOutputFile(
      "path_output",
      "/home/szhu/work/alice/analysis/PairFlow/output/SE_PR.root");

  TFile *file_event = TFile::Open(config_pathInputFile.data.c_str());
  TFile *fOutput = new TFile(config_pathOutputFile.data.c_str(), "RECREATE");

  TTree *tree_event = (TTree *)file_event->Get("O2dqflowvecd");

  vector<RResultHandle> gRResultHandlesFast;
  ROOT::RDataFrame rdf(*tree_event);

  /* #region rdf_all definition */
  auto rdf_all = rdf.Define("DeltaPhi",
                            [](const ROOT::RVec<float> &phi,
                               const ROOT::RVec<float> &phi_ref) {
                              ROOT::RVec<float> delta_phi;
                              for (size_t i = 0; i < phi.size(); ++i)
                                for (size_t j = 0; j < phi_ref.size(); ++j) {
                                  double delta = phi[i] - phi_ref[j];
                                  while (delta > 1.5 * M_PI)
                                    delta -= 2 * M_PI;
                                  while (delta < -0.5 * M_PI)
                                    delta += 2 * M_PI;
                                  delta_phi.emplace_back(delta);
                                }
                              return delta_phi;
                            },
                            {"fPhi", "fPhiREF"})
                     .Define("DeltaEta",
                             [](const ROOT::RVec<float> &eta,
                                const ROOT::RVec<float> &eta_ref) {
                               ROOT::RVec<float> delta_eta;
                               for (size_t i = 0; i < eta.size(); ++i)
                                 for (size_t j = 0; j < eta_ref.size(); ++j) {
                                   double delta = eta[i] - eta_ref[j];
                                   delta_eta.emplace_back(delta);
                                 }
                               return delta_eta;
                             },
                             {"fEta", "fEtaREF"});
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

  /* #endregion */
  vector<StrVar4Hist> vec_vars = {var_DeltaEta, var_DeltaPhi};

  /* #region macro definition */
#define RHistDefine2DLoop(df, v1, v2, cond)                                    \
  TString title = Form("%s_%s%s;%s (%s);%s (%s)", v1.fName.Data(),             \
                       v2.fName.Data(), cond.Data(), v1.fTitle.Data(),         \
                       v1.fUnit.Data(), v2.fTitle.Data(), v2.fUnit.Data());    \
  TH2DModel h2_mult(                                                           \
      Form("%s_%s%s", v1.fName.Data(), v2.fName.Data(), cond.Data()), title,   \
      v1.fNbins, v1.fBins.data(), v2.fNbins, v2.fBins.data());                 \
  gRResultHandlesFast.push_back(df.Histo2D(h2_mult, v1.fName, v2.fName));
  /* #endregion */

  RHistDefine2DLoop(rdf_all, vec_vars[0], vec_vars[1], gEmptyString);

  RunGraphs(gRResultHandlesFast);

  fOutput->cd();
  RResultWrite(gRResultHandlesFast);
  fOutput->Close();
}

int main(int argc, char **argv) {
  if (argc == 1)
    SE_PR();
  else
    SE_PR(argv[1]);
  return 0;
}