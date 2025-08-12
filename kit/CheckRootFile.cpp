#include "TFile.h"
#include "iostream"

using namespace std;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cout << "Usage: IsRootFileGood <filenam>" << endl;
    return 1;
  }

  for (int i_arg = 1; i_arg < argc; i_arg++) {
    // add processing bar in ===>| n%
    int percent = (i_arg * 100) / (argc - 1);
    int n_bars = percent / 2; // 50% -> 25 bars
    cout << "\rProcessing: [";
    for (int j = 0; j < n_bars; j++) {
      cout << "=";
    }
    for (int j = n_bars; j < 50; j++) {
      cout << " ";
    }
    cout << "] " << percent << "%" << flush;

    TFile *f = TFile::Open(argv[i_arg]);
    if (f == 0) {
      cout << "File " << argv[i_arg] << " does not exist" << endl;
    }

    if (f->IsZombie()) {
      cout << argv[i_arg] << endl;
    }
    if (f->IsOpen()) {
      f->Close();
    }
  }

  return 1;
}