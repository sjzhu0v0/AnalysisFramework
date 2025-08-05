#include "MHead.h"
#include "MMath.h"
#include "MSystem.h"
#include "ROOT/RDF/HistoModels.hxx"
#include "THn.h"
#include "tuple"

#ifndef MHist_h
#define MHist_h

#define BinUsed_Mult MultFt0c
#define BinUsed_Pt pT_JpsiFlow
#define BinUsed_Eta Eta_JpsiFlow
#define BinDefMass 20, 1, 5
#define BinDefEta 18, -0.9, 0.9
#define BinDefVtxZ 20, -10, 10
#define BinDefMult 5, 0, 2000

using namespace ROOT::RDF;

vector<double> GetLogBin(int n_bins, double low_bin, double high_bin) {
  if (low_bin <= 0 || high_bin <= 0 || n_bins <= 0) {
    cout << "Error: GetLogBin: low_bin, high_bin and n_bins must be positive"
         << endl;
    exit(1);
  }
  vector<double> bins;
  double log_low = log10(low_bin);
  double log_high = log10(high_bin);
  double log_step = (log_high - log_low) / n_bins;
  for (int i = 0; i <= n_bins; i++) {
    double bin = pow(10, log_low + i * log_step);
    bins.push_back(bin);
  }
  if (bins.size() != n_bins + 1) {
    cout << "Error: GetLogBin: bins size is not correct" << endl;
    exit(1);
  }
  return bins;
}

struct StrVar4Hist {
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

  double GetBinUpperEdge(int bin) {
    if (bin < 0 || bin >= fNbins) {
      cout << "Error: GetBinUpperEdge: bin index out of range" << endl;
      exit(1);
    }
    return fBins[bin + 1];
  }

  double GetBinCenter(int bin) {
    if (bin < 0 || bin >= fNbins) {
      cout << "Error: GetBinCenter: bin index out of range" << endl;
      exit(1);
    }
    return (fBins[bin] + fBins[bin + 1]) / 2.0;
  }
};

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

TH1DModel GetTH1DModelWithTitle(StrVar4Hist str, TString title = "",
                                TString tag = "") {
  TString name = str.fName;
  if (tag != "") {
    name += "_" + tag;
  }
  if (title == "") {
    title = str.CompleteTitle(tag);
  }
  return TH1DModel(name, title, str.fNbins, str.fBins.data());
}

TH1DModel GetTH1DModelWithTitle2(StrVar4Hist str, TString tag = "",
                                 TString title = "") {
  TString name = str.fName + "_" + tag;
  TString title_hist = title;
  title_hist += ";" + str.fTitle;
  if (str.fUnit != "")
    title_hist += " (" + str.fUnit + ")";
  else
    title_hist += ";";
  return TH1DModel(name, title_hist, str.fNbins, str.fBins.data());
}

using TupleTHnDModel = tuple<THnDModel, vector<string>>;

TupleTHnDModel GetTHnDModelWithTitle(vector<StrVar4Hist> vec_var,
                                     TString title = "", TString tag = "") {
  TString name_hist = vec_var[0].fName;
  for (int i = 1; i < vec_var.size(); i++)
    name_hist += "_" + vec_var[i].fName;
  if (tag != "")
    name_hist += "_" + tag;

  TString title_hist = title;
  title_hist += ";";
  for (auto var : vec_var) {
    TString title_var = var.fTitle;
    if (var.fUnit != "")
      title_var += " (" + var.fUnit + ")";
    title_hist += title_var + ";";
  }

  vector<int> nbins_hist;
  for (auto var : vec_var)
    nbins_hist.push_back(var.fNbins);

  vector<vector<double>> bins_hist;
  for (auto var : vec_var)
    bins_hist.push_back(var.fBins);

  THnDModel model(name_hist.Data(), title_hist.Data(), vec_var.size(),
                  nbins_hist, bins_hist);

  vector<string> column_var;
  for (auto var : vec_var)
    column_var.push_back(var.fName.Data());

  return {model, column_var};
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
  double nBins = h2->GetNbinsX() * h2->GetNbinsY();
  for (int iBinX = 1; iBinX <= h2->GetNbinsX(); iBinX++) {
    for (int iBinY = 1; iBinY <= h2->GetNbinsY(); iBinY++) {
      double binContent = h2->GetBinContent(iBinX, iBinY);
      double binError = h2->GetBinError(iBinX, iBinY);
      double binWidthX = h2->GetXaxis()->GetBinWidth(iBinX);
      double binWidthY = h2->GetYaxis()->GetBinWidth(iBinY);
      h2->SetBinContent(iBinX, iBinY, binContent / integral * nBins);
      double error = sqrt(binContent * (integral - binContent) / integral) /
                     integral * nBins;
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
  using MHToolAxisCut = tuple<int, double, double>;
  vector<MHToolAxisCut> fAxisCuts;

  MHnTool(THnD *h) { SetHn(h); }
  ~MHnTool() {
    if (hN)
      hN->Delete();
  }

  MHnTool(const MHnTool *other) {
    hN = other->hN;
    fNDimensions = other->fNDimensions;
    fAxisCuts = other->fAxisCuts;
  }

  auto &currentObject() { return *this; }

  void ChangeName(TString) {}

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

  void SetRangeUser(int dim, double min, double max) {
    if (dim < 0 || dim >= fNDimensions) {
      cerr << "Error: MHnTool::SetRangeUser: dim is out of range" << endl;
      exit(1);
    }
    fAxisCuts.push_back(make_tuple(dim, min, max));
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
      double min_range, max_range;
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
        min_range = hN->GetAxis(i)->GetBinLowEdge(index_bins);
        max_range = min_range + hN->GetAxis(i)->GetBinWidth(index_bins);
      } else {
        min_range = hN->GetAxis(i)->GetXmin();
        max_range = hN->GetAxis(i)->GetXmax();
      }
      for (const auto &cut : fAxisCuts) {
        if (get<0>(cut) == i) {
          min_range = max(min_range, get<1>(cut));
          max_range = min(max_range, get<2>(cut));
        }
      }
      hN->GetAxis(i)->SetRangeUser(min_range, max_range);
    }

    TH1D *h1D = hN->Projection(dimTarget);
    h1D->SetName(Form("%s_%s_%d_%d_%d", hN->GetName(), hN->GetTitle(),
                      dimTarget, binsTargets[0], GenerateUID()));
    return h1D;
  }

  TH2D *Project(int dimTarget1, int dimTarget2, vector<int> binsTargets) {
    if (binsTargets.size() + 2 != fNDimensions) {
      cerr << "Error: MHnTool::Project: binsTargets.size() + 2 != dimTarget"
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
      double min_range, max_range;
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
        min_range = hN->GetAxis(i)->GetBinLowEdge(index_bins);
        max_range = min_range + hN->GetAxis(i)->GetBinWidth(index_bins);
      } else {
        min_range = hN->GetAxis(i)->GetXmin();
        max_range = hN->GetAxis(i)->GetXmax();
      }
      for (const auto &cut : fAxisCuts) {
        if (get<0>(cut) == i) {
          min_range = max(min_range, get<1>(cut));
          max_range = min(max_range, get<2>(cut));
        }
      }
      hN->GetAxis(i)->SetRangeUser(min_range, max_range);
    }

    TH2D *h2D = hN->Projection(dimTarget1, dimTarget2);
    h2D->SetName(Form("%s_%s_%d_%d_%d", hN->GetName(), hN->GetTitle(),
                      dimTarget1, dimTarget2, GenerateUID()));

    return h2D;
  }

  double GetBinContent(vector<int> vec_targetedBins) {
    if (vec_targetedBins.size() != fNDimensions) {
      cerr << "Error: MHnTool::GetBinContent: vec_targetedBins.size() != "
              "fNDimensions"
           << endl;
      exit(1);
    }
    for (int i = 0; i < fNDimensions; i++) {
      if (vec_targetedBins[i] < 0 ||
          vec_targetedBins[i] > hN->GetAxis(i)->GetNbins()) {
        cerr << "Error: MHnTool::GetBinContent: vec_targetedBins[" << i
             << "] is out of range" << endl;
        exit(1);
      }
    }

    vector<int> vec_binNotZero;
    vector<int> vec_binNotZeroTargeted;
    for (int i = 0; i < fNDimensions; i++) {
      if (vec_targetedBins[i] != 0) {
        vec_binNotZero.push_back(i);
        vec_binNotZeroTargeted.push_back(vec_targetedBins[i]);
      }
    }
    if (vec_binNotZero.size() == fNDimensions)
      return hN->GetBinContent(vec_targetedBins.data());
    for (int i_dim = 0; i_dim < fNDimensions; i_dim++) {
      double min_range = hN->GetAxis(i_dim)->GetXmin();
      double max_range = hN->GetAxis(i_dim)->GetXmax();
      for (const auto &cut : fAxisCuts) {
        if (get<0>(cut) == i_dim) {
          min_range = max(min_range, get<1>(cut));
          max_range = min(max_range, get<2>(cut));
        }
      }
      hN->GetAxis(i_dim)->SetRangeUser(min_range, max_range);
    }
    if (vec_binNotZero.size() == 0) {
      return hN->Integral(true);
    }

    THnD *h_temp =
        (THnD *)hN->ProjectionND(vec_binNotZero.size(), vec_binNotZero.data());
    double binContent = h_temp->GetBinContent(vec_binNotZeroTargeted.data());
    h_temp->Delete();
    return binContent;
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

  int GetNbins(int i) { return hN->GetAxis(i)->GetNbins(); }
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

    for (const auto &str : fStrsVar4Hist) {
      fNbin_Var.emplace_back(str.fNbins);
    }

    int nbins_total = 1;
    for (const auto &str : fStrsVar4Hist) {
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
    for (auto &h : hgroupTool->fHistos) {
      fHistos.push_back((T *)h->Clone());
    }
  };
  ~MHGroupTool() {
    for (auto &h : fHistos) {
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
    return fNbin_Var[dim];
  };
  T *GetHist(int i) {
    if (i < 0 || i >= fHistos.size()) {
      cerr << "Error: MHGroupTool::GetHist: i is out of range" << endl;
      cerr << "i = " << i << ", fHistos.size() = " << fHistos.size() << endl;
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

template <typename T, typename T2> class MIndex {
public:
  T fVar;
  int fBinIndex = 0;
  int fIndex = 0;

  MIndex(T var, int binIndex = 1)
      : fVar(var), fBinIndex(binIndex), fIndex(binIndex) {
    if (binIndex < 0 || binIndex >= var.fNbins) {
      cerr << "Error: MVar::MVar: binIndex is out of range" << endl;
      exit(1);
    }
  }

  void restore() { fIndex = fBinIndex; }

  operator T2 &() const { return static_cast<T2 &>(*this); }

  int operator++() {
    fIndex++;
    if (fIndex > fVar.fNbins) {
      cerr << "Error: MVar::operator++: index is out of range" << endl;
      exit(1);
    }
    return fIndex;
  }

  class Iterator {
  private:
    MIndex *fMVar;
    int fCurrentIndex;

  public:
    Iterator(MIndex *mvar, int index) : fMVar(mvar), fCurrentIndex(index) {}
    Iterator &operator++() {
      fCurrentIndex++;
      fMVar->fIndex++;
      return *this;
    }
    bool operator!=(const Iterator &other) const {
      return fCurrentIndex != other.fCurrentIndex;
      ;
    }
    int operator*() const { return fCurrentIndex; }
  };
  Iterator begin() {
    this->restore();
    return Iterator(this, fBinIndex);
  }
  Iterator end() { return Iterator(this, fVar.fNbins + this->fBinIndex); }
};

#define DefineMIndexAny(tag, type_var)                                         \
  class MIndex##tag : public MIndex<type_var, MIndex##tag> {                   \
  public:                                                                      \
    MIndex##tag(type_var strVar, int binIndex = 1)                             \
        : MIndex<type_var, MIndex##tag>(strVar, binIndex) {}                   \
                                                                               \
    operator int() const { return fIndex; }                                    \
  };

template <typename T> class MIndexAny : public MIndex<T, MIndexAny<T>> {
public:
  MIndexAny(T strVar, int binIndex = 1)
      : MIndex<T, MIndexAny<T>>(strVar, binIndex) {}

  operator int() const { return this->fIndex; }
};

struct str_cond {
  vector<array<string, 2>> fConds;
  int fNbins;

  str_cond(vector<array<string, 2>> cond_init) : fConds(cond_init) {
    fNbins = fConds.size();
  }

  operator vector<array<string, 2>> &() { return fConds; }
};

// DefineMIndexAny(Cond, str_cond);

class MIndexHist : public MIndex<StrVar4Hist, MIndexHist> {
public:
  MIndexHist(StrVar4Hist strVar, int binIndex = 1, int rebin = 1)
      : MIndex<StrVar4Hist, MIndexHist>(strVar, binIndex) {
    if (rebin > 1) {
      fVar.rebin(rebin);
    }
  }

  operator int() const { return fIndex; }

  double GetBinUpperEdge() { return fVar.GetBinUpperEdge(fIndex - 1); }

  double GetBinCenter() {
    return fVar.GetBinCenter(fIndex - 1);
  }
};

template <typename T, typename T2> class MHist {
public:
  std::shared_ptr<T> fHisto;

  MHist() {}

  auto &currentObject() { return static_cast<T2 &>(*this); }

  void Write(TDirectory *dir = gDirectory) {
    if (!fHisto) {
      cerr << "Error: MHist1D::Write: fHisto is null" << endl;
      exit(1);
    }
    if (dir) {
      dir->cd();
    }
    fHisto->Write();
  }

  void SetName(TString name) {
    if (!fHisto) {
      cerr << "Error: MHist1D::SetName: fHisto is null" << endl;
      exit(1);
    }
    fHisto->SetName(name);
  }

  TString GetName() const {
    if (!fHisto) {
      cerr << "Error: MHist1D::GetName: fHisto is null" << endl;
      exit(1);
    }
    return fHisto->GetName();
  }

  void ChangeName(TString name) {
    if (!fHisto) {
      cerr << "Error: MHist1D::ChangeName: fHisto is null" << endl;
      exit(1);
    }
    fHisto->SetName(Form("%s_%s", fHisto->GetName(), name.Data()));
  }

  operator T *() {
    if (!fHisto) {
      cerr << "Error: MHist1D::operator TH1D*: fHisto is null" << endl;
      exit(1);
    }
    return fHisto.get();
  }

  void Delete() {
    if (!fHisto) {
      cerr << "Error: MHist1D::Delete: fHisto is null" << endl;
      exit(1);
    }
    fHisto->Delete();
    fHisto = nullptr;
  }
};

class MHist1D : public MHist<TH1D, MHist1D> {
public:
  MIndexHist &fIndexHist;

  MHist1D(TH1DModel model1D, MIndexHist &indexHist)
      : MHist<TH1D, MHist1D>(), fIndexHist(indexHist) {
    fHisto = model1D.GetHistogram();
  }

  MHist1D(MHist1D const *hist)
      : MHist<TH1D, MHist1D>(),
        fIndexHist(hist->fIndexHist) { // Copy constructor
    TString name = hist->fHisto->GetName();
    TString title = hist->fHisto->GetTitle();
    TString xaxis_title = hist->fHisto->GetXaxis()->GetTitle();
    TString yaxis_title = hist->fHisto->GetYaxis()->GetTitle();
    TH1DModel model(name, title, hist->fHisto->GetNbinsX(),
                    hist->fHisto->GetXaxis()->GetXbins()->GetArray());
    fHisto = model.GetHistogram();
    fHisto->GetXaxis()->SetTitle(xaxis_title);
    fHisto->GetYaxis()->SetTitle(yaxis_title);
  }

  MHist1D(MIndexHist &indexHist, TString tag = "", TString title = "",
          TDirectory *dir = gDirectory)
      : MHist<TH1D, MHist1D>(), fIndexHist(indexHist) {
    TString name = indexHist.fVar.fName;
    if (tag != "") {
      name += "_" + tag;
    }
    TString title_hist = title;
    title_hist += ";" + indexHist.fVar.fTitle;
    if (indexHist.fVar.fUnit != "")
      title_hist += " (" + indexHist.fVar.fUnit + ")";
    else
      title_hist += ";";

    TH1DModel model(name, title_hist, indexHist.fVar.fNbins,
                    indexHist.fVar.fBins.data());
    fHisto = model.GetHistogram();
    fHisto->SetDirectory(dir);
  }

  MHist1D(MIndexHist &indexHist, TH1D *h1)
      : MHist<TH1D, MHist1D>(), fIndexHist(indexHist) {
    if (!h1) {
      cerr << "Error: MHist1D::MHist1D: h1 is null" << endl;
      exit(1);
    }
    // check bininfo
    if (h1->GetNbinsX() < 1 || h1->GetNbinsX() != fIndexHist.fVar.fNbins) {
      cerr << "Error: MHist1D::MHist1D: h1 has wrong number of bins" << endl;
      exit(1);
    }
    TString name(h1->GetName());
    h1->SetName(Form("%d", GenerateUID()));

    fHisto = std::shared_ptr<TH1D>(new TH1D(*h1), [](TH1D *h) {
      h->SetDirectory(nullptr); // Prevent TH1D from deleting the directory
      delete h;
    });
    fHisto->SetName(name);
    h1->Delete();
  }

  void SetBinInfo(double content, double error = 0) {
    if (fIndexHist.fIndex < 1 || fIndexHist.fIndex > fIndexHist.fVar.fNbins) {
      cerr << "Error: MHist1D::SetBinInfo: index is out of range" << endl;
      exit(1);
    }
    fHisto->SetBinContent(fIndexHist.fIndex, content);
    fHisto->SetBinError(fIndexHist.fIndex, error);
  }

  void SetBinInfo(const MDouble &content) {
    if (fIndexHist.fIndex < 1 || fIndexHist.fIndex > fIndexHist.fVar.fNbins) {
      cerr << "Error: MHist1D::SetBinInfo: index is out of range" << endl;
      exit(1);
    }
    fHisto->SetBinContent(fIndexHist.fIndex, content.fValue);
    fHisto->SetBinError(fIndexHist.fIndex, content.fError);
  }

  MDouble GetBinInfo() const {
    if (fIndexHist.fIndex < 1 || fIndexHist.fIndex > fIndexHist.fVar.fNbins) {
      cerr << "Error: MHist1D::GetBinInfo: index is out of range" << endl;
      exit(1);
    }
    double content = fHisto->GetBinContent(fIndexHist.fIndex);
    double error = fHisto->GetBinError(fIndexHist.fIndex);
    return MDouble(content, error);
  }
};

class MHist2D : public MHist<TH2D, MHist2D> {
public:
  MIndexHist &fIndexHistX;
  MIndexHist &fIndexHistY;

  MHist2D(TH2DModel model2D, MIndexHist &indexHistX, MIndexHist &indexHistY)
      : MHist<TH2D, MHist2D>(), fIndexHistX(indexHistX),
        fIndexHistY(indexHistY) {
    fHisto = model2D.GetHistogram();
  }

  MHist2D(MIndexHist &indexHistX, MIndexHist &indexHistY, TString tag = "",
          TString title = "", TDirectory *dir = gDirectory)
      : MHist<TH2D, MHist2D>(), fIndexHistX(indexHistX),
        fIndexHistY(indexHistY) {
    TString name = indexHistX.fVar.fName + "_" + indexHistY.fVar.fName;
    if (tag != "") {
      name += "_" + tag;
    }
    TString title_hist = title;
    title_hist += ";" + indexHistX.fVar.fTitle;
    if (indexHistX.fVar.fUnit != "")
      title_hist += " (" + indexHistX.fVar.fUnit + ");";
    else
      title_hist += ";";

    title_hist += indexHistY.fVar.fTitle;
    if (indexHistY.fVar.fUnit != "")
      title_hist += " (" + indexHistY.fVar.fUnit + ")";

    TH2DModel model(name, title_hist, indexHistX.fVar.fNbins,
                    indexHistX.fVar.fBins.data(), indexHistY.fVar.fNbins,
                    indexHistY.fVar.fBins.data());
    fHisto = model.GetHistogram();
    fHisto->SetDirectory(dir);
  }

  MHist2D(MHist2D const *hist)
      : MHist<TH2D, MHist2D>(), fIndexHistX(hist->fIndexHistX),
        fIndexHistY(hist->fIndexHistY) {
    TH2D *h2 = new TH2D(*hist->fHisto);
    fHisto = std::shared_ptr<TH2D>(h2, [](TH2D *) {});
  }

  void SetBinInfo(double content, double error = 0) {
    if (fIndexHistX.fIndex < 1 ||
        fIndexHistX.fIndex > fIndexHistX.fVar.fNbins) {
      cerr << "Error: MHist2D::SetBinInfo: x index is out of range" << endl;
      exit(1);
    }
    if (fIndexHistY.fIndex < 1 ||
        fIndexHistY.fIndex > fIndexHistY.fVar.fNbins) {
      cerr << "Error: MHist2D::SetBinInfo: y index is out of range" << endl;
      exit(1);
    }
    fHisto->SetBinContent(fIndexHistX.fIndex, fIndexHistY.fIndex, content);
    fHisto->SetBinError(fIndexHistX.fIndex, fIndexHistY.fIndex, error);
  }

  void SetBinInfo(const MDouble &content) {
    if (fIndexHistX.fIndex < 1 ||
        fIndexHistX.fIndex > fIndexHistX.fVar.fNbins) {
      cerr << "Error: MHist2D::SetBinInfo: x index is out of range" << endl;
      exit(1);
    }
    if (fIndexHistY.fIndex < 1 ||
        fIndexHistY.fIndex > fIndexHistY.fVar.fNbins) {
      cerr << "Error: MHist2D::SetBinInfo: y index is out of range" << endl;
      exit(1);
    }
    fHisto->SetBinContent(fIndexHistX.fIndex, fIndexHistY.fIndex,
                          content.fValue);
    fHisto->SetBinError(fIndexHistX.fIndex, fIndexHistY.fIndex, content.fError);
  }
};

class MHist3D : public MHist<TH3D, MHist3D> {
public:
  MIndexHist &fIndexHistX;
  MIndexHist &fIndexHistY;
  MIndexHist &fIndexHistZ;

  MHist3D(TH3DModel model3D, MIndexHist &indexHistX, MIndexHist &indexHistY,
          MIndexHist &indexHistZ)
      : MHist<TH3D, MHist3D>(), fIndexHistX(indexHistX),
        fIndexHistY(indexHistY), fIndexHistZ(indexHistZ) {
    fHisto = model3D.GetHistogram();
  }

  MHist3D(MIndexHist &indexHistX, MIndexHist &indexHistY,
          MIndexHist &indexHistZ, TString tag = "", TString title = "",
          TDirectory *dir = gDirectory)
      : MHist<TH3D, MHist3D>(), fIndexHistX(indexHistX),
        fIndexHistY(indexHistY), fIndexHistZ(indexHistZ) {
    TString name = indexHistX.fVar.fName + "_" + indexHistY.fVar.fName + "_" +
                   indexHistZ.fVar.fName;
    if (tag != "") {
      name += "_" + tag;
    }
    TString title_hist = title;
    title_hist += ";" + indexHistX.fVar.fTitle;
    if (indexHistX.fVar.fUnit != "")
      title_hist += " (" + indexHistX.fVar.fUnit + ");";
    else
      title_hist += ";";

    title_hist += indexHistY.fVar.fTitle;
    if (indexHistY.fVar.fUnit != "")
      title_hist += " (" + indexHistY.fVar.fUnit + ");";
    else
      title_hist += ";";

    title_hist += indexHistZ.fVar.fTitle;
    if (indexHistZ.fVar.fUnit != "")
      title_hist += " (" + indexHistZ.fVar.fUnit + ")";

    TH3DModel model(name, title_hist, indexHistX.fVar.fNbins,
                    indexHistX.fVar.fBins.data(), indexHistY.fVar.fNbins,
                    indexHistY.fVar.fBins.data(), indexHistZ.fVar.fNbins,
                    indexHistZ.fVar.fBins.data());
    fHisto = model.GetHistogram();
    fHisto->SetDirectory(dir);
  }

  MHist3D(MHist3D const *hist)
      : MHist<TH3D, MHist3D>(), fIndexHistX(hist->fIndexHistX),
        fIndexHistY(hist->fIndexHistY), fIndexHistZ(hist->fIndexHistZ) {
    TH3D *h3 = new TH3D(*hist->fHisto);
    fHisto = std::shared_ptr<TH3D>(h3, [](TH3D *) {});
  }

  void SetBinInfo(double content, double error = 0) {
    if (fIndexHistX.fIndex < 1 ||
        fIndexHistX.fIndex > fIndexHistX.fVar.fNbins) {
      cerr << "Error: MHist3D::SetBinInfo: x index is out of range" << endl;
      exit(1);
    }
    if (fIndexHistY.fIndex < 1 ||
        fIndexHistY.fIndex > fIndexHistY.fVar.fNbins) {
      cerr << "Error: MHist3D::SetBinInfo: y index is out of range" << endl;
      exit(1);
    }
    if (fIndexHistZ.fIndex < 1 ||
        fIndexHistZ.fIndex > fIndexHistZ.fVar.fNbins) {
      cerr << "Error: MHist3D::SetBinInfo: z index is out of range" << endl;
      exit(1);
    }
    fHisto->SetBinContent(fIndexHistX.fIndex, fIndexHistY.fIndex,
                          fIndexHistZ.fIndex, content);
    fHisto->SetBinError(fIndexHistX.fIndex, fIndexHistY.fIndex,
                        fIndexHistZ.fIndex, error);
  }

  void SetBinInfo(const MDouble &content) {
    if (fIndexHistX.fIndex < 1 ||
        fIndexHistX.fIndex > fIndexHistX.fVar.fNbins) {
      cerr << "Error: MHist3D::SetBinInfo: x index is out of range" << endl;
      exit(1);
    }
    if (fIndexHistY.fIndex < 1 ||
        fIndexHistY.fIndex > fIndexHistY.fVar.fNbins) {
      cerr << "Error: MHist3D::SetBinInfo: y index is out of range" << endl;
      exit(1);
    }
    if (fIndexHistZ.fIndex < 1 ||
        fIndexHistZ.fIndex > fIndexHistZ.fVar.fNbins) {
      cerr << "Error: MHist3D::SetBinInfo: z index is out of range" << endl;
      exit(1);
    }
    fHisto->SetBinContent(fIndexHistX.fIndex, fIndexHistY.fIndex,
                          fIndexHistZ.fIndex, content.fValue);
    fHisto->SetBinError(fIndexHistX.fIndex, fIndexHistY.fIndex,
                        fIndexHistZ.fIndex, content.fError);
  }

  void Write(TDirectory *dir = gDirectory) {
    if (!fHisto) {
      cerr << "Error: MHist3D::Write: fHisto is null" << endl;
      exit(1);
    }
    if (dir) {
      dir->cd();
    }
    fHisto->Write();
  }

  operator TH3D *() {
    if (!fHisto) {
      cerr << "Error: MHist3D::operator TH3D*: fHisto is null" << endl;
      exit(1);
    }
    return fHisto.get();
  }

  void SetName(TString name) {
    if (!fHisto) {
      cerr << "Error: MHist3D::SetName: fHisto is null" << endl;
      exit(1);
    }
    fHisto->SetName(name);
  }

  void ChangeName(TString name) {
    if (!fHisto) {
      cerr << "Error: MHist3D::ChangeName: fHisto is null" << endl;
      exit(1);
    }
    fHisto->SetName(Form("%s_%s", fHisto->GetName(), name.Data()));
  }

  TString GetName() const {
    if (!fHisto) {
      cerr << "Error: MHist3D::GetName: fHisto is null" << endl;
      exit(1);
    }
    return fHisto->GetName();
  }

  void Delete() {
    if (!fHisto) {
      cerr << "Error: MHist3D::Delete: fHisto is null" << endl;
      exit(1);
    }
    fHisto.reset();
    fHisto->Delete();
  }
};

template <typename T, typename T2 = MIndexHist> class MVec {
public:
  T2 &fIndexHist;
  vector<T> fVec;

  MVec(T2 &indexHist) : fIndexHist(indexHist) {}

  MVec(T2 &indexHist, T t, TString naming = "") : fIndexHist(indexHist) {
    Preparing(t, naming);
  }

  MVec(MVec<T> *vec) : fIndexHist(vec->fIndexHist) {
    for (const auto &v : vec->fVec) {
      fVec.push_back(T(&v)); // Copy constructor
    }
  }

  MVec(MVec<T> const *vec) : fIndexHist(vec->fIndexHist) {
    for (const auto &v : vec->fVec) {
      fVec.push_back(T(&v)); // Copy constructor
    }
  }

  T &operator[](int index) {
    if (index < 0 || index >= fIndexHist.fVar.fNbins) {
      cerr << "Error: MVec::operator[]: index is out of range" << endl;
      exit(1);
    }
    return fVec[index];
  }

  T &current() {
    if (fIndexHist.fIndex < 1 || fIndexHist.fIndex > fIndexHist.fVar.fNbins) {
      cerr << "Error: MVec::current: index is out of range" << endl;
      exit(1);
    }
    return fVec[fIndexHist.fIndex - 1];
  }

  auto &currentObject() {
    if (fIndexHist.fIndex < 1 || fIndexHist.fIndex > fIndexHist.fVar.fNbins) {
      cerr << "Error: MVec::current: index is out of range" << endl;
      exit(1);
    }

    if constexpr (std::is_pointer<T>::value) {
      return current()->currentObject();
    } else
      return current().currentObject();
  }

  int size() const { return fVec.size(); }

  void Preparing(
      T &value, std::function<TString(const T2 &, int)> func_str =
                    [](const T2 &index, int i) {
                      return index.fVar.fName + "_" + TString(i);
                    }) {
    for (int i = 0; i < fIndexHist.fVar.fNbins; i++) {
      T t(&value);
      if constexpr (std::is_pointer<T>::value) {
        t->ChangeName(func_str(fIndexHist, i));
      } else
        t.ChangeName(func_str(fIndexHist, i));
      fVec.push_back(t);
    }
  }

  void ChangeName(TString nameing = "") {
    if (nameing == "") {
      nameing = fIndexHist.fVar.fName + "_%d";
    }

    for (int i = 0; i < fIndexHist.fVar.fNbins; i++) {
      if constexpr (std::is_pointer<T>::value) {
        fVec[i]->ChangeName(Form(nameing.Data(), i));
      } else
        fVec[i].ChangeName(Form(nameing.Data(), i));
    }
  }

  void Write(TDirectory *dir = gDirectory) {
    if (dir) {
      dir->cd();
    }
    for (auto v : fVec) {
      if constexpr (std::is_pointer<T>::value) {
        v->Write(dir);
      } else
        v.Write(dir);
    }
  }

  void Delete() {
    for (auto &v : fVec) {
      if constexpr (std::is_pointer<T>::value) {
        v->Delete();
      } else
        v.Delete();
    }
    fVec.clear();
  }
};

#endif