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

// read a list from a txt file
vector<TString> ReadListFromFile(TString path_txt) {
  vector<TString> vec_string;
  ifstream infile(path_txt.Data());
  string line;
  while (getline(infile, line)) {
    if (line.empty())
      continue;
    vec_string.push_back(TString(line.c_str()));
  }
  return vec_string;
}

// get the intersection of two lists
vector<TString> GetIntersectionOfTwoLists(vector<TString> list1,
                                          vector<TString> list2) {
  vector<TString> intersection;
  for (auto &item1 : list1) {
    for (auto &item2 : list2) {
      if (item1 == item2) {
        intersection.push_back(item1);
        break;
      }
    }
  }
  return intersection;
}

#endif