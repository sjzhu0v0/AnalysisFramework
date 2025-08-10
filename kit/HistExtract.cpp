#include "MRootIO.h"

void HistExtract(
    TString path_input = "/home/szhu/test/550819.root",
    TString path_output = "/home/szhu/test/550819_hist.root",
    TString path_hist = "analysis-event-selection/output/Event_BeforeCuts/BC") {
  TFile *file_input = new TFile(path_input);
  TFile *file_output = new TFile(path_output, "RECREATE");
  auto folder_input = MRootIO::GetObjectSingle(file_input, path_hist);

  file_output->cd();
  folder_input->Write();
  file_output->Close();
}

int main(int argc, char **argv) {
  TString path_input = "/home/szhu/test/550819.root";
  TString path_output = "/home/szhu/test/550819_hist.root";
  TString path_hist = "analysis-event-selection/output/Event_BeforeCuts/BC";

  if (argc > 1) {
    path_input = argv[1];
  }
  if (argc > 2) {
    path_output = argv[2];
  }
  if (argc > 3) {
    path_hist = argv[3];
  }

  HistExtract(path_input, path_output, path_hist);
  return 0;
}