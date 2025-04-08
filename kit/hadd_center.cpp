#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THn.h"
#include <memory>
#include <vector>
#include <cmath>
#include <iostream>
#include <string>
#include <TFile.h>
#include <TROOT.h>
#include <TKey.h>

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

// 合并一维直方图
void MergeTH1D(TH1D *target, const std::vector<TH1D *> &sources) {
  for (int bin = 0; bin <= target->GetNbinsX() + 1; ++bin) {
    MDoubleR combined;

    for (const auto &hist : sources) {
      double value = hist->GetBinContent(bin);
      double error = hist->GetBinError(bin);
      combined += MDoubleR(value, error);
    }

    target->SetBinContent(bin, combined.fValue);
    target->SetBinError(bin, combined.fError);
  }
}

// 合并二维直方图
void MergeTH2D(TH2D *target, const std::vector<TH2D *> &sources) {
  for (int binx = 0; binx <= target->GetNbinsX() + 1; ++binx) {
    for (int biny = 0; biny <= target->GetNbinsY() + 1; ++biny) {
      MDoubleR combined;

      for (const auto &hist : sources) {
        double value = hist->GetBinContent(binx, biny);
        double error = hist->GetBinError(binx, biny);
        combined += MDoubleR(value, error);
      }

      target->SetBinContent(binx, biny, combined.fValue);
      target->SetBinError(binx, biny, combined.fError);
    }
  }
}

// 合并三维直方图
void MergeTH3D(TH3D *target, const std::vector<TH3D *> &sources) {
  for (int binx = 0; binx <= target->GetNbinsX() + 1; ++binx) {
    for (int biny = 0; biny <= target->GetNbinsY() + 1; ++biny) {
      for (int binz = 0; binz <= target->GetNbinsZ() + 1; ++binz) {
        MDoubleR combined;

        for (const auto &hist : sources) {
          double value = hist->GetBinContent(binx, biny, binz);
          double error = hist->GetBinError(binx, biny, binz);
          combined += MDoubleR(value, error);
        }

        target->SetBinContent(binx, biny, binz, combined.fValue);
        target->SetBinError(binx, biny, binz, combined.fError);
      }
    }
  }
}

// 合并N维直方图 (THnD)
void MergeTHnD(THnD *target, const std::vector<THnD *> &sources) {
  // 获取所有bin的全局线性索引范围
  int nBins = target->GetNbins();

  // 创建并填充bin坐标数组
  std::vector<int> coords(target->GetNdimensions());

  for (int linearBin = 0; linearBin <= nBins + 1; ++linearBin) {
    // 将线性索引转换为多维坐标
    target->GetBinContent(linearBin); // 确保bin存在
    target->GetBin(coords.data(), linearBin);

    MDoubleR combined;

    for (const auto &hist : sources) {
      double value = hist->GetBinContent(coords.data());
      double error = hist->GetBinError(coords.data());
      combined += MDoubleR(value, error);
    }

    target->SetBinContent(coords.data(), combined.fValue);
    target->SetBinError(coords.data(), combined.fError);
  }
}

// 通用直方图合并函数
void MergeHistograms(TH1 *target, const std::vector<TH1 *> &sources) {
  if (TH1D *h1d = dynamic_cast<TH1D *>(target)) {
    std::vector<TH1D *> typedSources;
    for (auto hist : sources) {
      typedSources.push_back(dynamic_cast<TH1D *>(hist));
    }
    MergeTH1D(h1d, typedSources);
  } else if (TH2D *h2d = dynamic_cast<TH2D *>(target)) {
    std::vector<TH2D *> typedSources;
    for (auto hist : sources) {
      typedSources.push_back(dynamic_cast<TH2D *>(hist));
    }
    MergeTH2D(h2d, typedSources);
  } else if (TH3D *h3d = dynamic_cast<TH3D *>(target)) {
    std::vector<TH3D *> typedSources;
    for (auto hist : sources) {
      typedSources.push_back(dynamic_cast<TH3D *>(hist));
    }
    MergeTH3D(h3d, typedSources);
  } else if (THnD *hnd = dynamic_cast<THnD *>(target)) {
    std::vector<THnD *> typedSources;
    for (auto hist : sources) {
      typedSources.push_back(dynamic_cast<THnD *>(hist));
    }
    MergeTHnD(hnd, typedSources);
  }
}

// 主合并函数
void WeightedHAdd(const char *outputFilename,
                  const std::vector<const char *> &inputFilenames) {
  TFile *outputFile = new TFile(outputFilename, "RECREATE");

  // 假设所有输入文件有相同的直方图结构
  TFile *firstInputFile = new TFile(inputFilenames[0]);
  TList *keyList = firstInputFile->GetListOfKeys();

  // 遍历所有直方图
  TIter next(keyList);
  TKey *key;
  while ((key = (TKey *)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TH1"))
      continue;

    // 获取直方图名称
    std::string histName = key->GetName();
    TH1 *templateHist = (TH1 *)key->ReadObj();

    // 创建目标直方图
    TH1 *outputHist = (TH1 *)templateHist->Clone();
    outputHist->SetDirectory(outputFile);

    // 收集所有输入直方图
    std::vector<TH1 *> inputHists;
    inputHists.push_back(templateHist);

    for (size_t i = 1; i < inputFilenames.size(); ++i) {
      TFile *inputFile = new TFile(inputFilenames[i]);
      TH1 *hist = (TH1 *)inputFile->Get(histName.c_str());
      if (hist)
        inputHists.push_back(hist);
      inputFile->Close();
    }

    // 合并直方图
    MergeHistograms(outputHist, inputHists);
    outputHist->Write();

    // 清理
    delete templateHist;
    for (size_t i = 1; i < inputHists.size(); ++i) {
      delete inputHists[i];
    }
  }

  outputFile->Close();
  firstInputFile->Close();
}

// 使用示例
int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0]
              << " <output_file> <input_file1> [<input_file2> ...]"
              << std::endl;
    return 1;
  }
  const char *outputFilename = argv[1];
  std::vector<const char *> inputFiles;
  for (int i = 2; i < argc; ++i) {
    inputFiles.push_back(argv[i]);
  }

  WeightedHAdd("output.root", inputFiles);

  return 0;
}