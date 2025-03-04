#define MRDF
#include "MHead.h"
#include "MHist.h"
#include "MRootIO.h"
#include <ROOT/RDataFrame.hxx>

funcWithJson(void, RDataFrame_PIDSeparationPower)(
    TString path_config = "../config.json") {
  SetUpJson(path_config.Data());
  ROOT::EnableImplicitMT();
  // RDataFrame* rdf = new RDataFrame("T", "path/to/file.root");
  Configurable<string> config_pathInputFile("path_file_input",
                                            "path/to/file.root");
  Configurable<string> config_pathOutputFile("path_file_output", "output.root");
  TChain *chain =
      MRootIO::OpenChain(config_pathInputFile.data.c_str(), "O2tpcskimv0tree");

  RDataFrame rdf_pre(*chain);
  // 152 / (fNormNClustersTPC * fNormNClustersTPC)
  auto rdf = rdf_pre
                 .Define("nCluster",
                         [](float fNormNClustersTPC) {
                           return 152 / (fNormNClustersTPC * fNormNClustersTPC);
                         },
                         {"fNormNClustersTPC"})
                 .Define("DeltaDeDx",
                         [](float fTPCSignal, float fInvDeDxExpTPC) {
                           return fTPCSignal - 1. / fInvDeDxExpTPC;
                         },
                         {"fTPCSignal", "fInvDeDxExpTPC"});

  auto rdf_electron_mip =
      rdf.Filter("fPidIndex == 0")
          .Filter(
              "fTPCInnerParam < 3.5 * 0.139 && fTPCInnerParam > 3. * 0.139");
  auto rdf_pion_mip = rdf.Filter("fPidIndex == 2")
                          .Filter("fBetaGamma < 3.5 && fBetaGamma > 3.");

  double axisOccuFt0[] = {0.,     130.,   1010.,  2740.,  5130., 8070.,
                          11590., 16010., 22030., 31840., 5.e4};
#define axisOccuFt0 10, axisOccuFt0
#define axisTgl 10, -1, 1
#define axis_dEdx 150, 10, 160
#define axis_nsigma 100, -5, 5
#define axis_DeltaDeDx 160, -40, 40

  TFile *fOutput = new TFile(config_pathOutputFile.data.c_str(), "RECREATE");

  vector<string> condition = {"nCluster > 130",
                              "nCluster < 130 && nCluster >80"};
  int index_cls = 0;
  for (auto cond : condition) {
    auto rdf_electron_clusterVar = rdf_electron_mip.Filter(cond);
    auto rdf_pion_clusterVar = rdf_pion_mip.Filter(cond);
    RHistDefine2D(rdf_electron_clusterVar, "fTgl", "fTPCSignal",
                  TString::Format("Tgl_dEdx_Electron%d", index_cls),
                  "Tgl_dEdx_Electron:" + TString(cond), axisTgl, axis_dEdx);
    RHistDefine2D(rdf_pion_clusterVar, "fTgl", "fTPCSignal",
                  TString::Format("Tgl_dEdx_Pion%d", index_cls),
                  "Tgl_dEdx_Pion:" + TString(cond), axisTgl, axis_dEdx);
    RHistDefine2D(rdf_electron_clusterVar, "fFt0Occ", "fTPCSignal",
                  TString::Format("fFt0Occ_fTPCSignal_Electron%d", index_cls),
                  "fFt0Occ_fTPCSignal_Electron:" + TString(cond), axisOccuFt0,
                  axis_dEdx);
    RHistDefine2D(rdf_pion_clusterVar, "fFt0Occ", "fTPCSignal",
                  TString::Format("fFt0Occ_fTPCSignal_Pion%d", index_cls),
                  "fFt0Occ_fTPCSignal_Pion:" + TString(cond), axisOccuFt0,
                  axis_dEdx);
    RHistDefine2D(rdf_electron_clusterVar, "fTgl", "DeltaDeDx",
                  TString::Format("Tgl_DeltaDeDx_Electron%d", index_cls),
                  "Tgl_DeltaDeDx_Electron:" + TString(cond), axisTgl,
                  axis_DeltaDeDx);
    RHistDefine2D(rdf_pion_clusterVar, "fTgl", "DeltaDeDx",
                  TString::Format("Tgl_DeltaDeDx_Pion%d", index_cls),
                  "Tgl_DeltaDeDx_Pion:" + TString(cond), axisTgl,
                  axis_DeltaDeDx);
    RHistDefine2D(rdf_electron_clusterVar, "fFt0Occ", "DeltaDeDx",
                  TString::Format("fFt0Occ_DeltaDeDx_Electron%d", index_cls),
                  "fFt0Occ_DeltaDeDx_Electron:" + TString(cond), axisOccuFt0,
                  axis_DeltaDeDx);
    RHistDefine2D(rdf_pion_clusterVar, "fFt0Occ", "DeltaDeDx",
                  TString::Format("fFt0Occ_DeltaDeDx_Pion%d", index_cls),
                  "fFt0Occ_DeltaDeDx_Pion:" + TString(cond), axisOccuFt0,
                  axis_DeltaDeDx);
    RHistDefine2D(rdf_electron_clusterVar, "fTgl", "fNSigTPC",
                  TString::Format("Tgl_nsigma_Electron%d", index_cls),
                  "Tgl_nsigma_Electron:" + TString(cond), axisTgl, axis_nsigma);
    RHistDefine2D(rdf_pion_clusterVar, "fTgl", "fNSigTPC",
                  TString::Format("Tgl_nsigma_Pion%d", index_cls),
                  "Tgl_nsigma_Pion:" + TString(cond), axisTgl, axis_nsigma);
    RHistDefine2D(rdf_electron_clusterVar, "fFt0Occ", "fNSigTPC",
                  TString::Format("fFt0Occ_nsigma_Electron%d", index_cls),
                  "fFt0Occ_nsigma_Electron:" + TString(cond), axisOccuFt0,
                  axis_nsigma);
    RHistDefine2D(rdf_pion_clusterVar, "fFt0Occ", "fNSigTPC",
                  TString::Format("fFt0Occ_nsigma_Pion%d", index_cls),
                  "fFt0Occ_nsigma_Pion:" + TString(cond), axisOccuFt0,
                  axis_nsigma);
    index_cls++;
  }

  RunGraphs(gRResultHandles);

  fOutput->cd();
  for (auto &handle : gRResultHandles) {
    // 获取直方图
    auto hist = handle.GetPtr<TH2D>();
    hist->Write();
  }
  fOutput->Close();
}

int main(int argc, char **argv) {
  // string path_json = argv[1];
  // RDataFrame_PIDSeparationPower(path_json);
  if (argc == 1)
    RDataFrame_PIDSeparationPower();
  else
    RDataFrame_PIDSeparationPower(argv[1]);
  return 0;
}
