#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THn.h"
#include "TStatistic.h"
#include <TFile.h>
#include <TKey.h>
#include <TROOT.h>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

TFile *outputFile = nullptr;

void MergeTH1D(TH1D *target, const std::vector<TH1D *> &sources) {
  TH1D *hist0 = sources[0];
  for (int bin = 1; bin <= target->GetNbinsX() + 1; ++bin) {
    TStatistic stat;
    double content = hist0->GetBinContent(bin);
    if (content != 0)
      stat.Fill(content);
    for (size_t i = 1; i < sources.size(); ++i) {
      TH1D *hist = sources[i];
      double content1 = hist->GetBinContent(bin);
      if (content1 != 0)
        stat.Fill(content1);
    }
    target->SetBinContent(bin, stat.GetMean());
    target->SetBinError(bin, stat.GetRMS());
  }
}

void MergeTH2D(TH2D *target, const std::vector<TH2D *> &sources) {
  TH2D *hist0 = sources[0];
  for (int binx = 1; binx <= target->GetNbinsX() + 1; ++binx) {
    for (int biny = 1; biny <= target->GetNbinsY() + 1; ++biny) {
      TStatistic stat;
      double content = hist0->GetBinContent(binx, biny);
      if (content != 0)
        stat.Fill(content);
      for (size_t i = 1; i < sources.size(); ++i) {
        TH2D *hist = sources[i];
        double content1 = hist->GetBinContent(binx, biny);
        if (content1 != 0)
          stat.Fill(content1);
      }
      target->SetBinContent(binx, biny, stat.GetMean());
      target->SetBinError(binx, biny, stat.GetRMS());
    }
  }
}

void MergeTH3D(TH3D *target, const std::vector<TH3D *> &sources) {
  TH3D *hist0 = sources[0];
  for (int binx = 1; binx <= target->GetNbinsX() + 1; ++binx) {
    for (int biny = 1; biny <= target->GetNbinsY() + 1; ++biny) {
      for (int binz = 1; binz <= target->GetNbinsZ() + 1; ++binz) {
        TStatistic stat;
        double content = hist0->GetBinContent(binx, biny, binz);
        if (content != 0)
          stat.Fill(content);
        for (size_t i = 1; i < sources.size(); ++i) {
          TH3D *hist = sources[i];
          double content1 = hist->GetBinContent(binx, biny, binz);
          if (content1 != 0)
            stat.Fill(content1);
        }
        target->SetBinContent(binx, biny, binz, stat.GetMean());
        target->SetBinError(binx, biny, binz, stat.GetRMS());
      }
    }
  }
}

void HistMerge(std::vector<TFile *> inputFilesPtr, TString name_hist) {
  // std::vector<TH2D *> histograms;
  // for (const auto &inputFile : inputFilesPtr) {
  //   TH2D *hist = (TH2D *)inputFile->Get(name_hist);
  //   if (hist) {
  //     histograms.push_back(hist);
  //   }
  // }
  // outputFile->cd();
  // TH2D *outputHist = (TH2D *)histograms[0]->Clone(name_hist);
  // outputHist->SetDirectory(outputFile);

  // MergeTH2D(outputHist, histograms);
  // outputHist->Write();

  // judge the type of histogram
  TH1 *hist = (TH1 *)inputFilesPtr[0]->Get(name_hist);
  int dim = hist->GetDimension();
  if (dim == 1) {
    TH1D *outputHist = (TH1D *)hist->Clone(name_hist);
    outputHist->SetDirectory(outputFile);
    std::vector<TH1D *> histograms;
    for (const auto &inputFile : inputFilesPtr) {
      TH1D *hist = (TH1D *)inputFile->Get(name_hist);
      if (hist) {
        histograms.push_back(hist);
      }
    }
    MergeTH1D(outputHist, histograms);
    for (const auto &hist : histograms)
      hist->Delete();
    outputHist->Write();
    outputHist->Delete();
  } else if (dim == 2) {
    TH2D *outputHist = (TH2D *)hist->Clone(name_hist);
    outputHist->SetDirectory(outputFile);
    std::vector<TH2D *> histograms;
    for (const auto &inputFile : inputFilesPtr) {
      TH2D *hist = (TH2D *)inputFile->Get(name_hist);
      if (hist) {
        histograms.push_back(hist);
      }
    }
    MergeTH2D(outputHist, histograms);
    for (const auto &hist : histograms)
      hist->Delete();
    outputHist->Write();
    outputHist->Delete();
  } else if (dim == 3) {
    TH3D *outputHist = (TH3D *)hist->Clone(name_hist);
    outputHist->SetDirectory(outputFile);
    std::vector<TH3D *> histograms;
    for (const auto &inputFile : inputFilesPtr) {
      TH3D *hist = (TH3D *)inputFile->Get(name_hist);
      if (hist) {
        histograms.push_back(hist);
      }
    }
    MergeTH3D(outputHist, histograms);
    for (const auto &hist : histograms)
      hist->Delete();
    outputHist->Write();
    outputHist->Delete();
  } else if (dim > 3) {
    std::cerr << "Error: Histogram dimension is greater than 3." << std::endl;
  }
}

void BSMerge(TString name_output, std::vector<const char *> inputFiles) {
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

  BSMerge(name_output, inputFiles);

  return 0;
}