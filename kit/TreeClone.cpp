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
  for (auto &i : string_chain_split) {
    cout << i.Data() << endl;
  }

  TFile *file_input = TFile::Open(path_file_input);
  TFile *file_output = new TFile(path_file_output, "RECREATE");
  for (auto &name : string_chain_split) {
    cout << "Tree name: " << name << endl;
    TChain *chain = MRootIO::OpenChain(file_input, name);
    TTree *tree_output = chain->CloneTree();
    cout << "Tree name: " << chain->GetName() << endl;
    chain->GetListOfLeaves()->ls();
    file_output->cd();
    tree_output->Write();
  }
  file_output->ls();
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
