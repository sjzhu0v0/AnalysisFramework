#include "MRootIO.h"

void TreeClone(TString path_file_input,
               TString path_file_output = "./DiElec.root",
               TString name_tree = "O2rtdielectron") {
  std::vector<TString> string_chain_split;
  TString delimiter = ",";
  size_t pos = 0;
  while ((pos = name_tree.Index(delimiter)) != kNPOS) {
    string_chain_split.push_back(name_tree(0, pos));
    name_tree.Remove(0, pos + delimiter.Length());
  }
  string_chain_split.push_back(name_tree);

  TFile *file_input = TFile::Open(path_file_input);
  TFile *file_output = new TFile(path_file_output, "RECREATE");
  for (auto &name : string_chain_split) {
    TChain *chain = MRootIO::OpenChain(file_input, name_tree);
    TTree *tree_output = chain->CloneTree();
    tree_output->Write();
  }
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
