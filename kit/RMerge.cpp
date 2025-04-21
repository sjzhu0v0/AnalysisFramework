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
  for (int binx = 1; binx <= target->GetNbinsX() + 1; ++binx) {
    for (int biny = 1; biny <= target->GetNbinsY() + 1; ++biny) {
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

void RMerge(TString name_output, std::vector<const char *> inputFiles) {
  std::vector<TFile *> inputFilesPtr;

  for (const auto &inputFile : inputFiles) {
    TFile *file = new TFile(inputFile);
    inputFilesPtr.push_back(file);
  }

  outputFile = new TFile(name_output, "RECREATE");

  std::vector<TString> histNames;

  TList *keyList = inputFilesPtr[0]->GetListOfKeys();
  TIter next(keyList);
  while (TKey *key = (TKey *)next()) {
    if (key->GetClassName() == TString("TDirectoryFile")) {
      continue;
    }
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

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0]
              << " output_file input_file1 [input_file2 ...]" << std::endl;
    return 1;
  }

  TString name_output = argv[1];
  std::vector<const char *> inputFiles;

  for (int i = 2; i < argc; ++i) {
    inputFiles.push_back(argv[i]);
  }

  RMerge(name_output, inputFiles);

  return 0;
}