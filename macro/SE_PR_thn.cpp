#define MRDF
#include "MALICE.h"
#include "MCalibration.h"
#include "MHead.h"
#include "MHist.h"
#include "MRootIO.h"
#include "MShare.h"
#include "TApplication.h"

funcWithJson(void, SE_PR_thn)(TString path_config = "../config.json") {
  SetUpJson(path_config.Data());
  Configurable<string> config_pathInputFile(
      "path_input", " /home/szhu/work/alice/analysis/PairFlow/data/pairflow/"
                    "O2dqflowpairpr.root");
  Configurable<string> config_pathOutputFile(
      "path_output",
      "/home/szhu/work/alice/analysis/PairFlow/output/ME_PR.root");
  Configurable<string> config_pathCaliFile_vtxz("path_cali_vtxz",
                                                "path/to/cali.root:aaa");
  Configurable<string> config_pathCaliFile_run("path_cali_run",
                                               "path/to/cali.root:aaa");
  Configurable<int> config_runNumber("run_number", 0);

  TFile *file_event = TFile::Open(config_pathInputFile.data.c_str());
  TFile *fOutput = new TFile(config_pathOutputFile.data.c_str(), "RECREATE");

  Calib_NumContrib_fPosZ_Run::GetHistCali(config_pathCaliFile_vtxz.data,
                                          config_pathCaliFile_run.data,
                                          config_runNumber.data);

  TTree *tree_event = (TTree *)file_event->Get("O2dqflowvecd");

  vector<RResultHandle> gRResultHandlesFast;
  ROOT::RDataFrame rdf(*tree_event);

  // turn off the multithreading
  // ROOT::RDF::RDataFrame::SetDefaultNThreads(1);

  /* #region rdf_all definition */
  auto rdf_all =
      rdf.Define("DeltaPhi",
                 [](const ROOT::RVec<float> &phi,
                    const ROOT::RVec<float> &phi_ref) {
                   ROOT::RVec<float> delta_phi;
                   for (size_t i = 0; i < phi.size(); ++i)
                     for (size_t j = 0; j < phi_ref.size(); ++j) {
                       double delta = phi[i] - phi_ref[j];
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
                  {"fEta", "fEtaREF"})
          .DefineSlot("isntSameBunchPileup", MALICE::IsntSameBunchPileup,
                      {"fSelection"})
          .DefineSlot("fNumContribCalibrated",
                      Calib_NumContrib_fPosZ_Run::NumContribCalibratedFloat,
                      {"fMultVtxContri", "fVtxZ"})
          .Define("NumContribCalibrated_extended",
                  [](const double &numContrib, const RVec<float> &delta_phi) {
                    ROOT::RVec<double> numContrib_extended;
                    for (size_t i = 0; i < delta_phi.size(); ++i) {
                      numContrib_extended.emplace_back(numContrib);
                    }
                    return numContrib_extended;
                  },
                  {"fNumContribCalibrated", "DeltaPhi"})
          .Define("NumContribCalibrated_extended2",
                  [](const double &numContrib, const RVec<float> &delta_phi) {
                    ROOT::RVec<double> numContrib_extended;
                    for (size_t i = 0; i < delta_phi.size(); ++i) {
                      numContrib_extended.emplace_back(numContrib);
                    }
                    return numContrib_extended;
                  },
                  {"fNumContribCalibrated", "fMass"})
          .Define("fVtxZ_extended",
                  [](const float &vtxZ, const RVec<float> &delta_phi) {
                    ROOT::RVec<double> vtxZ_extended;
                    for (size_t i = 0; i < delta_phi.size(); ++i) {
                      vtxZ_extended.emplace_back(vtxZ);
                    }
                    return vtxZ_extended;
                  },
                  {"fVtxZ", "fMass"})
          .Define("fVtxZ_extended2",
                  [](const float &vtxZ, const RVec<float> &delta_phi) {
                    ROOT::RVec<double> vtxZ_extended;
                    for (size_t i = 0; i < delta_phi.size(); ++i) {
                      vtxZ_extended.emplace_back(vtxZ);
                    }
                    return vtxZ_extended;
                  },
                  {"fVtxZ", "fMass"})
          .Define("fMass_extended",
                  [](const ROOT::RVec<float> &mass,
                     const ROOT::RVec<float> &phi_ref) {
                    ROOT::RVec<float> mass_extended;
                    for (size_t i = 0; i < mass.size(); ++i)
                      for (size_t j = 0; j < phi_ref.size(); ++j) {
                        mass_extended.emplace_back(mass[i]);
                      }
                    return mass_extended;
                  },
                  {"fMass", "fPhiREF"})
          .Define("fPT_extended",
                  [](const ROOT::RVec<float> &pt,
                     const ROOT::RVec<float> &phi_ref) {
                    ROOT::RVec<float> pt_extended;
                    for (size_t i = 0; i < pt.size(); ++i)
                      for (size_t j = 0; j < phi_ref.size(); ++j) {
                        pt_extended.emplace_back(pt[i]);
                      }
                    return pt_extended;
                  },
                  {"fPT", "fPhiREF"});

  auto rdf_noPileup =
      rdf_all.Filter("isntSameBunchPileup", "no same bunch pileup");
  auto rdf_Pileup = rdf_all.Filter("!isntSameBunchPileup", "same bunch pileup");
  ROOT::RDF::Experimental::AddProgressBar(rdf_all);
  /* #endregion */

  /* #region histo4qa definition */
  auto h_DeltaPhi = rdf_all.Histo1D("DeltaPhi");
  gRResultHandlesFast.push_back(h_DeltaPhi);
  auto h_DeltaEta = rdf_all.Histo1D("DeltaEta");
  gRResultHandlesFast.push_back(h_DeltaEta);
  auto h_NumContrib = rdf_all.Histo1D("fNumContribCalibrated");
  gRResultHandlesFast.push_back(h_NumContrib);
  auto h_NumContrib_Pileup = rdf_Pileup.Histo1D("fNumContribCalibrated");
  gRResultHandlesFast.push_back(h_NumContrib_Pileup);
  auto h_NumContrib_noPileup = rdf_noPileup.Histo1D("fNumContribCalibrated");
  gRResultHandlesFast.push_back(h_NumContrib_noPileup);
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
  Int_t nbins[] = {var_DeltaEta.fNbins, var_DeltaPhi.fNbins,
                   var_VtxZ.fNbins,     var_Mass.fNbins,
                   var_Pt.fNbins,       var_NumContrib.fNbins};
  vector<vector<double>> vec_bins = {var_DeltaEta.fBins, var_DeltaPhi.fBins,
                                     var_VtxZ.fBins,     var_Mass.fBins,
                                     var_Pt.fBins,       var_NumContrib.fBins};

  string name_hist_info = "DeltaPhi_DeltaEta_VtxZ_Mass_Pt_NumContribCalib";
  string name_hist_title = "DeltaPhi_DeltaEta_VtxZ_Mass_Pt_NumContribCalib;"
                           ";#Delta #eta;#Delta #phi;V_{Z} (cm);"
                           "Mass (GeV/c^{2});p_{T} (GeV/c); NumContrib Calib";

  THnDModel h_multinfo(name_hist_info.c_str(), name_hist_title.c_str(), 6,
                       nbins, vec_bins);
  ColumnNames_t colnames_info = {
      "DeltaEta", "DeltaPhi", "fVtxZ", "fMass", "fPT", "fNumContribCalibrated"};

  Int_t nbins_triggered[] = {var_VtxZ.fNbins, var_Mass.fNbins, var_Pt.fNbins,
                             var_NumContrib.fNbins};
  vector<vector<double>> vec_bins_triggered = {
      var_VtxZ.fBins, var_Mass.fBins, var_Pt.fBins, var_NumContrib.fBins};
  string name_hist_triggered = "VtxZ_Mass_Pt_NumContribCalib";
  string name_hist_title_triggered =
      "VtxZ_Mass_Pt_NumContribCalib;V_{Z} (cm);Mass (GeV/c^{2});p_{T} (GeV/c);"
      "NumContrib Calib";
  THnDModel h_multinfo_triggered(name_hist_triggered.c_str(),
                                 name_hist_title_triggered.c_str(), 4,
                                 nbins_triggered, vec_bins_triggered);
  ColumnNames_t colnames_info_triggered = {"fVtxZ", "fMass", "fPT",
                                           "fNumContribCalibrated"};

  // auto info_multDim = rdf_noPileup.HistoND(h_multinfo, colnames_info);
  // gRResultHandlesFast.push_back(info_multDim);
  auto info_triggered =
      rdf_noPileup.HistoND(h_multinfo_triggered, colnames_info_triggered);
  gRResultHandlesFast.push_back(info_triggered);
  RunGraphs(gRResultHandlesFast);

  fOutput->cd();
  RResultWrite(gRResultHandlesFast);
  // info_multDim->Write();
  fOutput->Close();
}

int main(int argc, char **argv) {
  if (argc == 1)
    SE_PR_thn();
  else
    SE_PR_thn(argv[1]);
  return 0;
}