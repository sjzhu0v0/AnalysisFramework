#define MRDF
#include "MALICE.h"
#include "MDefinition.h"
#include "MHead.h"
#include "MHist.h"
#include "MRootIO.h"
#include "TApplication.h"
funcWithJson(void, SE_RR)(TString path_config = "../config.json") {
  SetUpJson(path_config.Data());
  Configurable<string> config_pathInputFile(
      "path_input",
      "/home/szhu/work/alice/analysis/data/pairflow/O2dqflowvecd.root");
  Configurable<string> config_pathOutputFile(
      "path_output",
      "/home/szhu/work/alice/analysis/PairFlow/output/SE_RR.root");

  TFile *file_event = TFile::Open(config_pathInputFile.data.c_str());
  TFile *fOutput = new TFile(config_pathOutputFile.data.c_str(), "RECREATE");

  TTree *tree_event = (TTree *)file_event->Get("O2dqflowvecd");

  vector<RResultHandle> gRResultHandlesFast;
  ROOT::RDataFrame rdf(*tree_event);

  /* #region rdf_all definition */
  auto rdf_all = rdf.Define("DeltaPhi",
                            [](const ROOT::RVec<float> &phi_ref) {
                              ROOT::RVec<float> delta_phi;
                              for (size_t i = 0; i < phi_ref.size(); ++i)
                                for (size_t j = 0; j < phi_ref.size(); ++j) {
                                  if (i == j)
                                    continue;
                                  double delta = phi_ref[i] - phi_ref[j];
                                  int n = 0;
                                  while (delta > 1.5 * M_PI && n < 10) {
                                    n++;
                                    delta -= 2 * M_PI;
                                  }
                                  while (delta < -0.5 * M_PI && n < 10) {
                                    n++;
                                    delta += 2 * M_PI;
                                  }
                                  if (n >= 10)
                                    delta = -999.;
                                  delta_phi.emplace_back(delta);
                                }
                              return delta_phi;
                            },
                            {"fPhiREF"})
                     .Define("DeltaEta",
                             [](const ROOT::RVec<float> &eta) {
                               ROOT::RVec<float> delta_eta;
                               for (size_t i = 0; i < eta.size(); ++i)
                                 for (size_t j = 0; j < eta.size(); ++j) {
                                   if (i == j)
                                     continue;
                                   double delta = eta[i] - eta[j];
                                   delta_eta.emplace_back(delta);
                                 }
                               return delta_eta;
                             },
                             {"fEtaREF"});
  ROOT::RDF::Experimental::AddProgressBar(rdf_all);
  /* #endregion */

  /* #region histo4qa definition */
  auto h_DeltaPhi = rdf_all.Histo1D("DeltaPhi");
  gRResultHandlesFast.push_back(h_DeltaPhi);
  auto h_DeltaEta = rdf_all.Histo1D("DeltaEta");
  gRResultHandlesFast.push_back(h_DeltaEta);
  /* #endregion */

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
  TString title_3d = Form(                                                     \
      "%s_%s_%s%s;%s (%s);%s (%s);%s (%s)", v1.fName.Data(), v2.fName.Data(),  \
      v3.fName.Data(), cond.Data(), v1.fTitle.Data(), v1.fUnit.Data(),         \
      v2.fTitle.Data(), v2.fUnit.Data(), v3.fTitle.Data(), v3.fUnit.Data());   \
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
    SE_RR();
  else
    SE_RR(argv[1]);
  return 0;
}