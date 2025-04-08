#define MRDF
#include "MALICE.h"
#include "MHead.h"
#include "MHist.h"
#include "MRootIO.h"
#include "TApplication.h"
#include "MShare.h"
funcWithJson(void, ME_PR_thn)(TString path_config = "../config.json") {
  SetUpJson(path_config.Data());
  Configurable<string> config_pathInputFile(
      "path_input", " /home/szhu/work/alice/analysis/PairFlow/data/pairflow/"
                    "O2dqflowpairpr.root");
  Configurable<string> config_pathOutputFile(
      "path_output",
      "/home/szhu/work/alice/analysis/PairFlow/output/ME_PR.root");

  TFile *file_event = TFile::Open(config_pathInputFile.data.c_str());
  TFile *fOutput = new TFile(config_pathOutputFile.data.c_str(), "RECREATE");

  TTree *tree_event = (TTree *)file_event->Get("O2dqflowpairpr");

  vector<RResultHandle> gRResultHandlesFast;
  ROOT::RDataFrame rdf(*tree_event);

  /* #region rdf_all definition */
  auto rdf_all = rdf.Define("DeltaPhi",
                            [](float phi, float phi_ref) {
                              double delta = phi - phi_ref;
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
                                return -999.;
                              else
                                return delta;
                            },
                            {"fPhi", "fPhi1"})
                     .Define("DeltaEta",
                             [](float eta, float eta_ref) {
                               double delta = eta - eta_ref;
                               return delta;
                             },
                             {"fEta", "fEta1"});
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
  Int_t nbins[] = {var_DeltaEta.fNbins, var_DeltaPhi.fNbins, var_VtxZ.fNbins,
                   var_Mass.fNbins, var_Pt.fNbins};
  Double_t xmin[] = {var_DeltaEta.fBins.front(), var_DeltaPhi.fBins.front(),
                     var_VtxZ.fBins.front(), var_Mass.fBins.front(),
                     var_Pt.fBins.front()};
  Double_t xmax[] = {var_DeltaEta.fBins.back(), var_DeltaPhi.fBins.back(),
                     var_VtxZ.fBins.back(), var_Mass.fBins.back(),
                     var_Pt.fBins.back()};

  string name_hist_info = "DeltaPhi_DeltaEta_VtxZ_Mass_Pt";
  string name_hist_title = "DeltaPhi_DeltaEta_VtxZ_Mass_Pt"
                           ";#Delta #eta;#Delta #phi;V_{Z} (cm);"
                           "Mass (GeV/c^{2});p_{T} (GeV/c)";

  THnDModel h_multinfo(name_hist_info.c_str(), name_hist_title.c_str(), 5,
                       nbins, xmin, xmax);
  ColumnNames_t colnames_info = {
      {"DeltaEta", "DeltaPhi", "fVtxZ", "fMass", "fPT"}};
  // Set axis titles
  RHistDefine2DLoop(rdf_all, vec_vars[0], vec_vars[1], gEmptyString);

  rdf_all.HistoND(h_multinfo, colnames_info);
  RunGraphs(gRResultHandlesFast);

  fOutput->cd();
  RResultWrite(gRResultHandlesFast);
  fOutput->Close();
}

int main(int argc, char **argv) {
  if (argc == 1)
    ME_PR_thn();
  else
    ME_PR_thn(argv[1]);
  return 0;
}