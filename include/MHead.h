#ifndef __MHead_h__
#define __MHead_h__

#include "TCanvas.h"
#include "TChain.h"
#include "TColor.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THashList.h"
#include "TKey.h"
#include "TLatex.h"
#include "TLine.h"
#include "TList.h"
#include "TProfile.h"
#include "TROOT.h"
#include "TRandom3.h"
#include "TTree.h"
#include "fstream"
#include "iostream"
#include "string"
#include <pthread.h>

using namespace std;

std::mutex mtx;

int GenerateUID() {
  mtx.lock();
  static int uid = 0;
  uid++;
  int local_uid = uid;
  mtx.unlock();
  return local_uid;
}

double gThresholdBootstrap = 0.5;
bool IsInBootstrap(unsigned int n) {
  thread_local TRandom3 rand(0);
  return rand.Uniform(0, 1) < gThresholdBootstrap;
}

TString gEmptyString = "";

#endif