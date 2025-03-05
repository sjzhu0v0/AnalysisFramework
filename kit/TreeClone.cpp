#include "MRootIO.h"

void TreeClone(TString path_file_input,
                  TString path_file_output = "./DiElec.root",
                  TString name_tree = "O2rtdiElectron") {
  TChain *chain = MRootIO::OpenChain(path_file_input, name_tree);
  TFile *file_output = new TFile(path_file_output, "RECREATE");

  TTree *tree_output = chain->CloneTree();
  tree_output->Write();
  file_output->Close();
}

int main(int argc, char **argv) {
  // input, output, tree name
  if (argc != 4) {
    cout << "Usage: " << argv[0] << " <input> <output> <tree name>" << endl;
    return 1;
  }
  TreeClone(argv[1], argv[2], argv[3]);
  return 0;
}
