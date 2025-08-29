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
double gScaleBS = 1;

void MergeTH1D(TH1D *target, const std::vector<TH1D *> &sources) {
  TH1D *hist0 = sources[0];
  int n_source = sources.size();
  for (int bin = 1; bin <= target->GetNbinsX() + 1; ++bin) {
    TStatistic stat;
    double eff = 0;
    double content = hist0->GetBinContent(bin);
    if (content != 0)
      stat.Fill(content);
    if (content == 0)
      eff += 1.;
    for (size_t i = 1; i < sources.size(); ++i) {
      TH1D *hist = sources[i];
      double content1 = hist->GetBinContent(bin);
      if (content1 != 0)
        stat.Fill(content1);
    }
    eff = 1;
    // eff = 1 - eff / (double)n_source;
    target->SetBinContent(bin, stat.GetMean());
    if (eff == 0)
      target->SetBinError(bin, -1.);
    else
      target->SetBinError(bin, stat.GetRMS() * gScaleBS * 1 / sqrt(eff));
  }
}

void MergeTH2D(TH2D *target, const std::vector<TH2D *> &sources) {
  TH2D *hist0 = sources[0];
  int n_source = sources.size();
  for (int binx = 1; binx <= target->GetNbinsX() + 1; ++binx) {
    for (int biny = 1; biny <= target->GetNbinsY() + 1; ++biny) {
      TStatistic stat;
      double eff = 0;
      double content = hist0->GetBinContent(binx, biny);
      if (content != 0)
        stat.Fill(content);
      if (content == 0)
        eff += 1.;
      for (size_t i = 1; i < sources.size(); ++i) {
        TH2D *hist = sources[i];
        double content1 = hist->GetBinContent(binx, biny);
        if (content1 != 0)
          stat.Fill(content1);
      }
      eff = 1;
      eff = 1 - eff / (double)n_source;
      target->SetBinContent(binx, biny, stat.GetMean());
      if (eff == 0)
        target->SetBinError(binx, biny, -1.);
      else
        target->SetBinError(binx, biny,
                            stat.GetRMS() * gScaleBS * 1 / sqrt(eff));
    }
  }
}

void MergeTH3D(TH3D *target, const std::vector<TH3D *> &sources) {
  TH3D *hist0 = sources[0];
  int n_source = sources.size();
  for (int binx = 1; binx <= target->GetNbinsX() + 1; ++binx) {
    for (int biny = 1; biny <= target->GetNbinsY() + 1; ++biny) {
      for (int binz = 1; binz <= target->GetNbinsZ() + 1; ++binz) {
        TStatistic stat;
        double eff = 0;
        double content = hist0->GetBinContent(binx, biny, binz);
        if (content != 0)
          stat.Fill(content);
        if (content == 0)
          eff += 1.;
        for (size_t i = 1; i < sources.size(); ++i) {
          TH3D *hist = sources[i];
          double content1 = hist->GetBinContent(binx, biny, binz);
          if (content1 != 0)
            stat.Fill(content1);
        }
        eff = 1;
        // eff = 1 - eff / (double)n_source;
        target->SetBinContent(binx, biny, binz, stat.GetMean());
        if (eff == 0)
          target->SetBinError(binx, biny, binz, -1.);
        else
          target->SetBinError(binx, biny, binz,
                              stat.GetRMS() * gScaleBS * 1 / sqrt(eff));
      }
    }
  }
}

void HistMerge(std::vector<TFile *> inputFilesPtr, TString name_hist) {
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
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0]
              << "fraction output_file input_file1 [input_file2 ...]"
              << std::endl;
    return 1;
  }

  double fraction = std::stod(argv[1]);
  // gScaleBS = fraction / sqrt(1 - fraction);
  // sqrt(fraction * (1. - fraction)) / fraction;
  gScaleBS = fraction / sqrt(fraction * (1. - fraction));

  TString name_output = argv[2];
  std::vector<const char *> inputFiles;

  for (int i = 3; i < argc; ++i) {
    inputFiles.push_back(argv[i]);
  }

  BSMerge(name_output, inputFiles);

  return 0;
}