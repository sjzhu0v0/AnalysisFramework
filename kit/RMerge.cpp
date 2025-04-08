#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THn.h"
#include <TFile.h>
#include <TKey.h>
#include <TROOT.h>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

TFile *outputFile = nullptr;

class MDoubleR {
public:
  double fValue;
  double fError;

  MDoubleR(double value = 0.0, double error = INFINITY)
      : fValue(value), fError(error) {}

  MDoubleR &operator+=(const MDoubleR &other) {
    if (fError == 0.0) {
      *this = other;
      return *this;
    }
    if (other.fError == 0.0) {
      return *this;
    }

    double w1 = 1.0 / (fError * fError);
    double w2 = 1.0 / (other.fError * other.fError);

    fValue = (w1 * fValue + w2 * other.fValue) / (w1 + w2);
    fError = std::sqrt(1.0 / (w1 + w2));

    return *this;
  }
};

void MergeTH2D(TH2D *target, const std::vector<TH2D *> &sources) {
  TH2D *hist0 = sources[0];
  for (int binx = 0; binx <= target->GetNbinsX() + 1; ++binx) {
    for (int biny = 0; biny <= target->GetNbinsY() + 1; ++biny) {
      //  remove the first histogram from the sources
      MDoubleR combined(hist0->GetBinContent(binx, biny),
                        hist0->GetBinError(
                            binx, biny)); // Initialize with the first histogram
      for (size_t i = 1; i < sources.size(); ++i) {
        TH2D *hist = sources[i];
        double value = hist->GetBinContent(binx, biny);
        double error = hist->GetBinError(binx, biny);
        combined += MDoubleR(value, error);
      }
      target->SetBinContent(binx, biny, combined.fValue);
      target->SetBinError(binx, biny, combined.fError);
    }
  }
}

void HistMerge(std::vector<TFile *> inputFilesPtr, TString name_hist) {
  std::vector<TH2D *> histograms;
  for (const auto &inputFile : inputFilesPtr) {
    TH2D *hist = (TH2D *)inputFile->Get(name_hist);
    if (hist) {
      histograms.push_back(hist);
    }
  }
  outputFile->cd();
  TH2D *outputHist = (TH2D *)histograms[0]->Clone(name_hist);
  outputHist->SetDirectory(outputFile);

  MergeTH2D(outputHist, histograms);
  outputHist->Write();
}

void RMerge() {
  // 输入文件列表
  std::vector<const char *> inputFiles = {
      "/home/szhu/work/alice/analysis/data/pairflow/grid/r_cluster1.root",
      "/home/szhu/work/alice/analysis/data/pairflow/grid/r_cluster10.root",
      "/home/szhu/work/alice/analysis/data/pairflow/grid/r_cluster11.root",
      "/home/szhu/work/alice/analysis/data/pairflow/grid/r_cluster12.root",
      "/home/szhu/work/alice/analysis/data/pairflow/grid/r_cluster13.root",
      "/home/szhu/work/alice/analysis/data/pairflow/grid/r_cluster2.root",
      "/home/szhu/work/alice/analysis/data/pairflow/grid/r_cluster3.root",
      "/home/szhu/work/alice/analysis/data/pairflow/grid/r_cluster4.root",
      "/home/szhu/work/alice/analysis/data/pairflow/grid/r_cluster5.root",
      "/home/szhu/work/alice/analysis/data/pairflow/grid/r_cluster6.root",
      "/home/szhu/work/alice/analysis/data/pairflow/grid/r_cluster7.root",
      "/home/szhu/work/alice/analysis/data/pairflow/grid/r_cluster8.root",
      "/home/szhu/work/alice/analysis/data/pairflow/grid/r_cluster9.root"};
  std::vector<TFile *> inputFilesPtr;

  for (const auto &inputFile : inputFiles) {
    TFile *file = new TFile(inputFile);
    inputFilesPtr.push_back(file);
  }

  outputFile = new TFile("output.root", "RECREATE");

  std::vector<TString> histNames;

  TList *keyList = inputFilesPtr[0]->GetListOfKeys();
  TIter next(keyList);
  while (TKey *key = (TKey *)next()) {
    TString histName = key->GetName();
    histNames.push_back(histName);
  }

  for (const auto &histName : histNames) {
    HistMerge(inputFilesPtr, histName);
  }

  for (auto &file : inputFilesPtr) {
    file->Close();
    delete file;
  }

  outputFile->Close();
}