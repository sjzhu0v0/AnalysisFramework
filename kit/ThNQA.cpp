#include "MHist.h"
#include "MRootGraphic.h"
#include "THn.h"

void ThNQA(TString hist4check =
               "/home/szhu/work/alice/analysis/PairFlow/test/data/"
               "group1_bs_1.root:VtxZ_Mass_Pt_NumContribCalib") {
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
  vector<int> vec_bins = {2, 2, 2, 2};
  hist->Rebin(2);
  THnD *h = (THnD *)hist->Rebin(vec_bins.data());

  cout << h->GetAxis(0)->GetNbins() << endl;
  cout << hist->GetAxis(1)->GetNbins() << endl;
  cout << hist->GetAxis(2)->GetNbins() << endl;
  // auto h1 = hNTool->Project(1, {0, 0, 0});

  // h1->Draw();
}