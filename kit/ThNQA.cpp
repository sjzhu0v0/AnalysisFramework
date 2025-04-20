#include "MHist.h"
#include "THn.h"

void ThNQA(TString hist4check =
               "/home/szhu/work/alice/analysis/PairFlow/test/"
               "group1.root:DeltaEta_DeltaPhi_VtxZ_Mass_Pt_NumContribCalib") {
  TString path_file = hist4check(0, hist4check.Index(":"));
  TString path_hist =
      hist4check(hist4check.Index(":") + 1, hist4check.Length());

  TFile *file = TFile::Open(path_file);
  if (!file || file->IsZombie()) {
    std::cerr << "Error: Could not open file " << path_file << std::endl;
    exit(1);
  }
  auto hist = dynamic_cast<THnD *>(file->Get(path_hist));

  MHnTool *hNTool = new MHnTool(hist);

  // histogram axis print
  hNTool->PrintAllAxis();
}