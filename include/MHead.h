#ifndef __MHead_h__
#define __MHead_h__

#include "TCanvas.h"
#include "TChain.h"
#include "TColor.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THashList.h"
#include "TKey.h"
#include "TLatex.h"
#include "TLine.h"
#include "TList.h"
#include "TROOT.h"
#include "TTree.h"
#include "iostream"
#include "fstream"
#include "string"
#include "nlohmann/json.hpp"

using namespace std;

int GenerateUID() {
  static int uid = 0;
  return uid++;
}

#endif