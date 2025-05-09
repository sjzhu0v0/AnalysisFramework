#include "MRootIO.h"
#include "TApplication.h"
#include "TBrowser.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THn.h"
#include "TRint.h"
#include "string"
#include <TFile.h>
#include <TKey.h>
#include <TROOT.h>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0]
              << " output_file input_file1 [input_file2 ...]" << std::endl;
    return 1;
  }

  TString string_chain = argv[1];
  std::vector<const char *> inputFiles;

  // divide the string_chain by ","
  std::vector<TString> string_chain_split;
  TString delimiter = ",";
  size_t pos = 0;
  while ((pos = string_chain.Index(delimiter)) != kNPOS) {
    string_chain_split.push_back(string_chain(0, pos));
    string_chain.Remove(0, pos + delimiter.Length());
  }
  for (auto i : string_chain_split)
    cout << i.Data() << endl;

  for (int i = 2; i < argc; ++i) {
    inputFiles.push_back(argv[i]);
  }

  TRint *app = new TRint("app", &argc, argv);
  // TChain *chain = new TChain(string_chain_split[0]);
  vector<TChain *> chains;
  vector<TFile *> files;
  for (auto &inputFile : inputFiles) {
    TFile *file = new TFile(inputFile);
    if (!file || file->IsZombie()) {
      std::cerr << "Error: Could not open file " << inputFile << std::endl;
      return 1;
    }
    files.push_back(file);
  }

  for (const auto &chainName : string_chain_split) {
    TChain *chain = new TChain(chainName);
    chain->SetDirectory(0);
    for (auto file : files) {
      TChain *tmpChain = MRootIO::OpenChain(file, chainName);
      if (tmpChain) {
        chain->Add(tmpChain);
        delete tmpChain;
      } else {
        std::cerr << "Error: Could not open chain " << chainName << " in file "
                  << file->GetName() << std::endl;
        return 1;
      }
    }
    chains.push_back(chain);
  }

  if (chains.empty()) {
    std::cerr << "Error: No chains found" << std::endl;
    return 1;
  }

  if (chains.size() > 1) {
    for (size_t i = 1; i < chains.size(); ++i) {
      cout << i << endl;
      chains[0]->AddFriend(chains[i]);
    }
  }

  cout << "start" << endl;
  cout << "TChain* chain = " << "(TChain*)" << chains[0] << ";" << endl;
  app->Run(true);
}