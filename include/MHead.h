#ifndef __MHead_h__
#define __MHead_h__

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TKey.h"
#include "iostream"
#include "TF1.h"
#include "TH2.h"
#include "TH1.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TH3.h"
#include "TLatex.h"
#include "TLine.h"
#include "TList.h"
#include "THashList.h"

using namespace std;

int GenerateUID() {
  static int uid = 0;
  return uid++;
}



#endif