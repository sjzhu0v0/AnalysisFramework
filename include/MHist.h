#include "MHead.h"
#include "MSystem.h"
#include "ROOT/RDF/HistoModels.hxx"
#include "THn.h"

#ifndef MHist_h
#define MHist_h

#define BinUsed_Mult MultFt0c
#define BinUsed_Pt pT_JpsiFlow
#define BinUsed_Eta Eta_JpsiFlow
#define BinDefMass 20, 1, 5
#define BinDefEta 18, -0.9, 0.9
#define BinDefVtxZ 20, -10, 10
#define BinDefMult 5, 0, 2000

typedef struct StrVar4Hist {
  TString fName;
  TString fTitle;
  TString fUnit;
  int fNbins;
  vector<double> fBins;
  StrVar4Hist(TString name, TString title, TString unit, int nbins,
              vector<double> bins)
      : fName(name), fTitle(title), fUnit(unit), fNbins(nbins) {
    if (bins.size() != nbins + 1 && bins.size() != 2) {
      cout << "Error: bins size is not correct" << endl;
      exit(1);
    }
    if (bins.size() == 2) {
      for (int i = 0; i <= nbins; i++) {
        fBins.push_back(bins[0] + i * (bins[1] - bins[0]) / (double)nbins);
      }
    }
    if (bins.size() == nbins + 1) {
      fBins = bins;
    }
    fUnit = unit;
  }
  TString CompleteTitle(TString tag = "") {
    return fTitle + "_" + tag + ";" + fTitle + " (" + fUnit + ")";
  }
  void rebin(int n) {
    if (n > fNbins) {
      cout << "Error: rebinning factor is too large" << endl;
      exit(1);
    }
    fNbins /= n;
    double finalbin = fBins.back();
    for (int i = 0; i < fNbins; i++) {
      fBins[i] = fBins[i * n];
    }
    fBins.resize(fNbins);
    fBins.push_back(finalbin);
  }
  int FindBin(double value) {
    for (int i = 0; i < fNbins; i++) {
      if (value >= fBins[i] && value < fBins[i + 1]) {
        return i + 1;
      }
    }
    return -1;
  }

  void SetTitle(TString title) { fTitle = title; }

  TH1DModel GetTH1DModel(TString tag = "") {
    if (tag == "") {
      tag = fName;
    }
    TString title;
    if (this->fUnit != "") {
      title = this->fTitle + "_" + tag + ";" + this->fTitle + " (" +
              this->fUnit + ")";
    } else {
      title = this->fTitle + "_" + tag + ";" + this->fTitle;
    }
    return TH1DModel(Form("%s_%s", fName.Data(), tag.Data()), title, fNbins,
                     fBins.data());
  }
} StrVar4Hist;

TH2DModel GetTH2DModel(StrVar4Hist str1, StrVar4Hist str2, TString tag = "") {
  TString name = str1.fName + "_" + str2.fName;
  if (tag != "") {
    name += "_" + tag;
  }
  TString title1, title2;
  if (str1.fUnit != "") {
    title1 = str1.fTitle + " (" + str1.fUnit + ")";
  } else {
    title1 = str1.fTitle;
  }
  if (str2.fUnit != "") {
    title2 = str2.fTitle + " (" + str2.fUnit + ")";
  } else {
    title2 = str2.fTitle;
  }

  TString title =
      str1.fTitle + "_" + str2.fTitle + "_" + tag + ";" + title1 + ";" + title2;
  return TH2DModel(name, title, str1.fNbins, str1.fBins.data(), str2.fNbins,
                   str2.fBins.data());
}

TH2DModel GetTH2DModelWithTitle(StrVar4Hist str1, StrVar4Hist str2,
                                TString title = "", TString tag = "") {
  TString name = str1.fName + "_" + str2.fName;
  if (tag != "") {
    name += "_" + tag;
  }

  return TH2DModel(name, title, str1.fNbins, str1.fBins.data(), str2.fNbins,
                   str2.fBins.data());
}

TH1DModel GetTH1DModelWithTitle(StrVar4Hist str, TString title = "", TString tag = "") {
  TString name = str.fName;
  if (tag != "") {
    name += "_" + tag;
  }
  if (title == "") {
    title = str.CompleteTitle(tag);
  }
  return TH1DModel(name, title, str.fNbins, str.fBins.data());
}

#include "MDefinition.h"

#define HistDefine(histType, histName, histTitle, ...)                         \
  histType *histName =                                                         \
      new histType(histName, histName + ";" + histTitle, __VA_ARGS__);

#define HistDefine1D(histName, histTitle, ...)                                 \
  HistDefine(TH1D, histName, histTitle, __VA_ARGS__)

#define HistDefine2D(histName, histTitle, ...)                                 \
  HistDefine(TH2D, histName, histTitle, __VA_ARGS__)

int GetBinIndex(double value, int nbins, double *axis) {
  int bin = -1;
  for (int i = 0; i < nbins; i++) {
    if (value >= axis[i] && value < axis[i + 1]) {
      bin = i;
      break;
    }
  }
  return bin;
}

int GetBinIndex(double value, int nbins, double low, double high) {
  if (value < low || value > high) {
    return -1;
  }
  double binWidth = (high - low) / nbins;
  return (value - low) / binWidth;
}

void ScaleHisto2D(TH2D *h, double scale) {
  for (int i = 1; i <= h->GetNbinsX(); i++) {
    for (int j = 1; j <= h->GetNbinsY(); j++) {
      double binContent = h->GetBinContent(i, j);
      double binError = h->GetBinError(i, j);
      h->SetBinContent(i, j, binContent * scale);
      h->SetBinError(i, j, binError * scale);
    }
  }
}

void ScaleHisto1D(TH1D *h, double scale) {
  for (int i = 1; i <= h->GetNbinsX(); i++) {
    double binContent = h->GetBinContent(i);
    double binError = h->GetBinError(i);
    h->SetBinContent(i, binContent * scale);
    h->SetBinError(i, binError * scale);
  }
}

void SliceYTH2D(TH2D *h2, TDirectory *dir,
                void (*func)(TH1D *, double *) = nullptr,
                double *par = nullptr) {
  TString name_h2 = h2->GetName();
  dir->cd();
  dir->mkdir("silceY_" + name_h2);
  dir->cd("silceY_" + name_h2);

  for (int i = 1; i <= h2->GetNbinsX(); i++) {
    TString name_h1 = TString::Format("sliceY_%d", i);
    TH1D *h1 = h2->ProjectionY(name_h1, i, i);
    TString title_h1 = h2->GetYaxis()->GetTitle();
    TString title_axis = h2->GetXaxis()->GetTitle();
    double low = h2->GetXaxis()->GetBinLowEdge(i);
    double up = h2->GetXaxis()->GetBinUpEdge(i);
    h1->SetTitle(title_axis + TString::Format(":[%.2f,%.2f];", low, up) +
                 title_h1);
    if (func)
      func(h1, par);
    h1->SetDirectory(gDirectory);
    h1->SetOption("E1");
    h1->Write();
  }
  dir->cd();
}

void HistDivide2D(TH2D *result, TH2D *h1, TH2D *h2) {
  for (int i = 1; i <= result->GetNbinsX(); i++) {
    for (int j = 1; j <= result->GetNbinsY(); j++) {
      double binContent1 = h1->GetBinContent(i, j);
      double binContent2 = h2->GetBinContent(i, j);
      if (binContent2 != 0) {
        result->SetBinContent(i, j, binContent1 / binContent2);
      } else {
        result->SetBinContent(i, j, 0);
      }
      double binError1 = h1->GetBinError(i, j);
      double binError2 = h2->GetBinError(i, j);
      if (binContent2 != 0) {
        double error = binContent1 / binContent2;
        double error1 = binError1 / binContent2;
        double error2 = binContent1 * binError2 / (binContent2 * binContent2);
        result->SetBinError(i, j,
                            TMath::Sqrt(error1 * error1 + error2 * error2));
      } else {
        result->SetBinError(i, j, 0);
      }
    }
  }
}

void HistDivide1D(TH1D *result, TH1D *h1, TH1D *h2) {
  for (int i = 1; i <= result->GetNbinsX(); i++) {
    double binContent1 = h1->GetBinContent(i);
    double binContent2 = h2->GetBinContent(i);
    if (binContent2 != 0) {
      result->SetBinContent(i, binContent1 / binContent2);
    } else {
      result->SetBinContent(i, 0);
    }
    double binError1 = h1->GetBinError(i);
    double binError2 = h2->GetBinError(i);
    if (binContent2 != 0) {
      double error = binContent1 / binContent2;
      double error1 = binError1 / binContent2;
      double error2 = binContent1 * binError2 / (binContent2 * binContent2);
      result->SetBinError(i, TMath::Sqrt(error1 * error1 + error2 * error2));
    } else {
      result->SetBinError(i, 0);
    }
  }
}

void DensityHisto1DNoWeight(TH1D *h1) {
  double integral = h1->Integral();
  for (int iBin = 1; iBin <= h1->GetNbinsX(); iBin++) {
    double binContent = h1->GetBinContent(iBin);
    double binError = h1->GetBinError(iBin);
    double binWidth = h1->GetBinWidth(iBin);
    h1->SetBinContent(iBin, binContent / integral / binWidth);
    double error = sqrt(binContent * (integral - binContent) / integral) /
                   integral / binWidth;
    h1->SetBinError(iBin, error);
  }
}

void DensityHisto2DNoWeight(TH2D *h2) {
  double integral = h2->Integral();
  if (integral == 0) {
    return;
  }
  for (int iBinX = 1; iBinX <= h2->GetNbinsX(); iBinX++) {
    for (int iBinY = 1; iBinY <= h2->GetNbinsY(); iBinY++) {
      double binContent = h2->GetBinContent(iBinX, iBinY);
      double binError = h2->GetBinError(iBinX, iBinY);
      double binWidthX = h2->GetXaxis()->GetBinWidth(iBinX);
      double binWidthY = h2->GetYaxis()->GetBinWidth(iBinY);
      h2->SetBinContent(iBinX, iBinY,
                        binContent / integral / binWidthX / binWidthY);
      double error = sqrt(binContent * (integral - binContent) / integral) /
                     integral / binWidthX / binWidthY;
      h2->SetBinError(iBinX, iBinY, error);
    }
  }
}

void HistSubstraction1D(TH1D *result, TH1D *h1, TH1D *h2) {
  for (int i = 1; i <= result->GetNbinsX(); i++) {
    double binContent1 = h1->GetBinContent(i);
    double binContent2 = h2->GetBinContent(i);
    result->SetBinContent(i, binContent1 - binContent2);
    double binError1 = h1->GetBinError(i);
    double binError2 = h2->GetBinError(i);
    result->SetBinError(
        i, TMath::Sqrt(binError1 * binError1 + binError2 * binError2));
  }
}

void HistSubstraction2D(TH2D *result, TH2D *h1, TH2D *h2) {
  for (int i = 1; i <= result->GetNbinsX(); i++) {
    for (int j = 1; j <= result->GetNbinsY(); j++) {
      double binContent1 = h1->GetBinContent(i, j);
      double binContent2 = h2->GetBinContent(i, j);
      result->SetBinContent(i, j, binContent1 - binContent2);
      double binError1 = h1->GetBinError(i, j);
      double binError2 = h2->GetBinError(i, j);
      result->SetBinError(
          i, TMath::Sqrt(binError1 * binError1 + binError2 * binError2));
    }
  }
}

class MHnTool {
public:
  THnD *hN = nullptr;
  int fNDimensions = 0;

  MHnTool(THnD *h) { SetHn(h); }
  ~MHnTool() {
    if (hN)
      hN->Delete();
  }

  // information print
  void PrintAllAxis() {
    cout << "Number of dimensions: " << fNDimensions << endl;
    cout << "Name: " << hN->GetName() << endl;
    cout << "Title: " << hN->GetTitle() << endl;
    for (int i = 0; i < fNDimensions; i++) {
      cout << "Axis " << i << ": " << hN->GetAxis(i)->GetName()
           << ", title: " << hN->GetAxis(i)->GetTitle()
           << "  nbins:" << hN->GetAxis(i)->GetNbins() << endl;
    }
  }

  void SetHn(THnD *h) {
    if (hN != h)
      hN = h;

    if (!hN) {
      cerr << "Error: MHnTool::SetHn: hN is null" << endl;
      exit(1);
    }
    fNDimensions = hN->GetNdimensions();
  }

  TH1D *Project(int dimTarget, vector<int> binsTargets) {
    if (binsTargets.size() + 1 != fNDimensions) {
      cerr << "Error: MHnTool::Project: binsTargets.size() + 1 != dimTarget"
           << endl;
      exit(1);
    }

    int binMore = 0;
    for (int i = 0; i < fNDimensions; i++) {
      if (i == dimTarget) {
        binMore++;
        continue;
      }
      double min, max;
      int bin2set = i - binMore;
      int index_bins = binsTargets[bin2set];
      if (index_bins != 0 && index_bins > hN->GetAxis(i)->GetNbins()) {
        cerr << "Error: MHnTool::Project: index_bins is out of range" << endl;
        cerr << "index_bins = " << index_bins
             << ", hN->GetAxis(i)->GetNbins() = " << hN->GetAxis(i)->GetNbins()
             << endl;
        cerr << "hist name: " << hN->GetName() << endl;
        cerr << "axis title: " << hN->GetAxis(bin2set)->GetTitle() << endl;
        exit(1);
      }
      if (index_bins > 0) {
        min = hN->GetAxis(i)->GetBinLowEdge(index_bins);
        max = min + hN->GetAxis(i)->GetBinWidth(index_bins);
      } else {
        min = hN->GetAxis(i)->GetXmin();
        max = hN->GetAxis(i)->GetXmax();
      }
      hN->GetAxis(i)->SetRangeUser(min, max);
    }

    TH1D *h1D = hN->Projection(dimTarget);
    h1D->SetName(Form("%s_%s_%d_%d_%d", hN->GetName(), hN->GetTitle(),
                      dimTarget, binsTargets[0], GenerateUID()));
    return h1D;
  }

  TH2D *Project(int dimTarget1, int dimTarget2, vector<int> binsTargets) {
    if (binsTargets.size() + 2 != fNDimensions) {
      cerr << "Error: MHnTool::Project: binsTargets.size() + 1 != dimTarget"
           << endl;
      exit(1);
    }
    int binMore = 0;
    for (int i = 0; i < fNDimensions; i++) {
      if (i == dimTarget1 || i == dimTarget2) {
        binMore++;
        continue;
      }
      int bin2set = i - binMore;
      int index_bins = binsTargets[bin2set];
      double min, max;
      if (index_bins != 0 && index_bins > hN->GetAxis(i)->GetNbins()) {
        cerr << "Error: MHnTool::Project: index_bins is out of range" << endl;
        cerr << "index_bins = " << index_bins
             << ", hN->GetAxis(i)->GetNbins() = " << hN->GetAxis(i)->GetNbins()
             << endl;
        cerr << "hist name: " << hN->GetName() << endl;
        cerr << "axis title: " << hN->GetTitle() << endl;
        exit(1);
      }

      if (index_bins > 0) {
        min = hN->GetAxis(i)->GetBinLowEdge(index_bins);
        max = min + hN->GetAxis(i)->GetBinWidth(index_bins);
      } else {
        min = hN->GetAxis(i)->GetXmin();
        max = hN->GetAxis(i)->GetXmax();
      }
      hN->GetAxis(i)->SetRangeUser(min, max);
    }

    TH2D *h2D = hN->Projection(dimTarget1, dimTarget2);
    h2D->SetName(Form("%s_%s_%d_%d_%d", hN->GetName(), hN->GetTitle(),
                      dimTarget1, dimTarget2, GenerateUID()));

    return h2D;
  }

  void Rebin(int dimTarget, int n) {
    if (dimTarget < 0 || dimTarget >= fNDimensions) {
      cerr << "Error: MHnTool::Rebin: dimTarget is out of range" << endl;
      exit(1);
    }
    vector<int> vec_rebin(fNDimensions, 1);
    vec_rebin[dimTarget] = n;

    THnD *h_new = (THnD *)hN->Rebin(vec_rebin.data());
    TString name = hN->GetName();
    hN->SetName(name + "_rebin");
    h_new->SetName(name);
    h_new->SetTitle(hN->GetTitle());
    for (int i = 0; i < fNDimensions; i++) {
      h_new->GetAxis(i)->SetTitle(hN->GetAxis(i)->GetTitle());
    }
    hN->Delete();
    hN = h_new;
  }
};

template <typename T> class MHGroupTool {
protected:
  vector<T *> fHistos;
  vector<StrVar4Hist> fStrsVar4Hist;
  vector<int> fNbin_Var;
  vector<int> fN4process;

  vector<int> GetBinIndex(int i) {
    vector<int> vec_index(fNbin_Var.size(), 0);
    for (int j = 0; j < fNbin_Var.size(); j++)
      vec_index[j] = i / fN4process[j] % fNbin_Var[j] + 1;
    return vec_index;
  }

  int GetBinIndex(vector<int> vec_index) {
    if (vec_index.size() != fNbin_Var.size()) {
      cerr << "Error: MHGroupTool::GetBinIndex: vec_index size is not equal to "
              "fNbin_Var size"
           << endl;
      exit(1);
    }
    int index = 0;
    for (int i = 0; i < fNbin_Var.size(); i++) {
      index += (vec_index[i] - 1) * fN4process[i];
    }
    return index;
  }

public:
  MHGroupTool(TFile *file, TString name_tag,
              vector<StrVar4Hist> vec_strVar4Hist, vector<int> vec_rebin = {}) {
    fStrsVar4Hist = vec_strVar4Hist;
    if (vec_rebin.size() != 0) {
      if (vec_rebin.size() != fStrsVar4Hist.size()) {
        cout << "Error: vec_rebin size is not equal to vec_strVar4Hist size"
             << endl;
        exit(1);
      }
      for (int i = 0; i < vec_rebin.size(); i++) {
        fStrsVar4Hist[i].rebin(vec_rebin[i]);
      }
    }

    for (auto &str : fStrsVar4Hist) {
      fNbin_Var.push_back(str.fNbins);
    }

    int nbins_total = 1;
    for (auto &str : fStrsVar4Hist) {
      nbins_total *= str.fNbins;
    }
    int nbins_total_temp = nbins_total;
    for (int i = 0; i < fNbin_Var.size(); i++) {
      fN4process.push_back(nbins_total_temp / fNbin_Var[i]);
      nbins_total_temp /= fNbin_Var[i];
    }
    for (int i = 0; i < nbins_total; i++) {
      vector<int> vec_index = GetBinIndex(i);
      TString name = name_tag;
      for (int j = 0; j < fNbin_Var.size(); j++)
        name.Replace(name.First("%d"), 2, Form("%d", vec_index[j]));
      T *histo = (T *)file->Get(name);
      histo->SetDirectory(0);
      if (!histo) {
        cerr << "Error: MHGroupTool::MHGroupTool: histo is null" << endl;
        exit(1);
      }
      fHistos.push_back(histo);
    }
  };
  MHGroupTool(MHGroupTool *hgroupTool) {
    fStrsVar4Hist = hgroupTool->fStrsVar4Hist;
    fNbin_Var = hgroupTool->fNbin_Var;
    fN4process = hgroupTool->fN4process;
    for (auto h : hgroupTool->fHistos) {
      fHistos.push_back((T *)h->Clone());
    }
  };
  ~MHGroupTool() {
    for (auto h : fHistos) {
      if (h) {
        h->Delete();
      }
    }
  };
  void Print() {
    cout << "Number of histograms: " << fHistos.size() << endl;
    for (int i = 0; i < fHistos.size(); i++) {
      cout << "Histogram " << i << ": " << fHistos[i]->GetName()
           << " with integral: " << fHistos[i]->Integral()
           << " with entries: " << fHistos[i]->GetEntries() << endl;
    }
  }
  int FindBin(double value, int dim) {
    if (dim < 0 || dim >= fNbin_Var.size()) {
      cerr << "Error: MHGroupTool::FindBin: dim is out of range" << endl;
      exit(1);
    }
    return fStrsVar4Hist[dim].FindBin(value);
  };
  int GetNBins(int dim) {
    if (dim < 0 || dim >= fNbin_Var.size()) {
      cerr << "Error: MHGroupTool::GetNBins: dim is out of range" << endl;
      exit(1);
    }
    if (dim != 0)
      return fNbin_Var[dim];
    else
      return fHistos[0]->GetNbinsX();
  };
  T *GetHist(int i) {
    if (i < 0 || i >= fHistos.size()) {
      cerr << "Error: MHGroupTool::GetHist: i is out of range" << endl;
      exit(1);
    }
    return fHistos[i];
  };
  T *GetHist(vector<int> vec_index) {
    if (vec_index.size() != fNbin_Var.size()) {
      cerr << "Error: MHGroupTool::GetHist: vec_index size is not equal to "
              "fNbin_Var size"
           << endl;
      exit(1);
    }
    int index = GetBinIndex(vec_index);
    return GetHist(index);
  };
};

using MHGroupTool1D = MHGroupTool<TH1D>;
using MHGroupTool2D = MHGroupTool<TH2D>;

#endif

#ifdef MRDF
#define RHistDefine1D(df, v, ...)                                              \
  gRResultHandles.push_back(df.Histo1D({__VA_ARGS__}, v));

#define RHistDefine1DWeighted(df, v, w, ...)                                   \
  gRResultHandles.push_back(df.Histo1D({__VA_ARGS__}, v, w));

#define RHistDefine2D(df, v1, v2, ...)                                         \
  gRResultHandles.push_back(df.Histo2D({__VA_ARGS__}, v1, v2));

#define RHistDefine2DWeighted(df, v1, v2, w, ...)                              \
  gRResultHandles.push_back(df.Histo2D({__VA_ARGS__}, v1, v2, w));

#define RHistDefine3D(df, v1, v2, v3, ...)                                     \
  gRResultHandles.push_back(df.Histo3D({__VA_ARGS__}, v1, v2, v3));

#define RHistDefine3DWeighted(df, v1, v2, v3, w, ...)                          \
  gRResultHandles.push_back(df.Histo3D({__VA_ARGS__}, v1, v2, v3, w));

#endif