#include "MHead.h"

#ifndef MHist_h
#define MHist_h

#define BinUsed_Mult MultFt0c
#define BinUsed_Pt pT_JpsiFlow
#define BinUsed_Eta Eta_JpsiFlow
#define BinDefMass 20, 1, 5
#define BinDefEta 18, -0.9, 0.9
#define BinDefVtxZ 20, -10, 10
#define BinDefMult 5, 0, 2000

double GetBinDefLeftEdge(int nbins, double low, double up, int bin) {
  return bin * (up - low) / (double)nbins + low;
}

double GetBinDefRightEdge(int nbins, double low, double up, int bin) {
  return (bin + 1) * (up - low) / (double)nbins + low;
}

double GetBinDefCenter(int nbins, double low, double up, int bin) {
  return (bin + 0.5) * (up - low) / (double)nbins + low;
}

typedef struct StrVar4Hist {
  TString name;
  TString title;
  TString unit;
  const int nbins;
  vector<double> bins;
} StrVar4Hist;

namespace MVariable {
#define bindef_pt "p_{T}", "GeV/c"
#define bindef_eta "#eta", ""
#define bindef_massDielec "Mass (e^{+}e^{-})", "GeV/c^{2}"
#define bindef_DeltaMass "Mass - Mass(dilepton)", "GeV/c^{2}"
#define bindef_TauZJpsi "TauZ(Jpsi)", "ns"
#define bindef_lowLimitAbsTauZJpsi "|TauZ(Jpsi)|>bin low edge", "ns"

#define bindef_MultFt0c "MultFt0c", ""

StrVar4Hist pT_Upsilon1 = {"pT_Upsilon1", bindef_pt, 3, {0., 3., 5., 10.}};
StrVar4Hist Mass_Upsilon1 = {"Mass_Upsilon1", bindef_massDielec, 500, {7, 12.}};

// for Pc
StrVar4Hist DeltaMass_Pc = {"DeltaMass_Pc", bindef_DeltaMass, 2250, {0.5, 5}};
StrVar4Hist pT_Pc = {"pT_Pc", bindef_pt, 5, {0., 5.}};
StrVar4Hist TauZJpsi = {"TauZJpsi", bindef_TauZJpsi, 2000, {-0.01, 0.01}};
StrVar4Hist lowLimitAbsTauZJpsi = {
    "AbsTauZJpsiLowLimit", bindef_lowLimitAbsTauZJpsi, 50, {0, 5e-3}};

// for Jpsi flow
StrVar4Hist Eta_JpsiFlow = {"Eta_JpsiFlow", bindef_eta, 18, {-0.9, 0.9}};
StrVar4Hist pT_JpsiFlow = {
    "pT_JpsiFlow", bindef_pt, 5, {0., 1., 2., 4., 8., 16.}};
StrVar4Hist MultFt0c = {"MultFt0c", bindef_MultFt0c, 5, {0., 2000.}};

#undef bindef_pt
#undef bindef_massDielec
}; // namespace MVariable

namespace MHist {
TH2D *CreateHist2D(StrVar4Hist var1, StrVar4Hist var2, TString name_hist = "",
                   TString title_hist = "") {
  if (name_hist == "")
    name_hist = var1.name + "_" + var2.name;
  if (title_hist == "") {
    title_hist = var1.name + ":" + var2.name + ";" + var1.title + " [" +
                 var1.unit + "];" + var2.title + " [" + var2.unit + "]";
  } else {
    title_hist = title_hist + ";" + var1.title + " [" + var1.unit + "];" +
                 var2.title + " [" + var2.unit + "]";
  }

  // bins check
  if (var1.bins.size() != var1.nbins + 1) {
    if (var1.bins.size() != 2) {
      cerr << "Error: bins size is not equal to nbins + 1" << endl;
      exit(1);
    }
    // clear bins
    double max_bins = var1.bins.back();
    double min_bins = var1.bins.front();
    var1.bins.clear();
    for (int i = 0; i < var1.nbins + 1; i++)
      var1.bins.push_back(i * (max_bins - min_bins) / (double)var1.nbins +
                          min_bins);
  }

  if (var2.bins.size() != var2.nbins + 1) {
    if (var2.bins.size() != 2) {
      cerr << "Error: bins size is not equal to nbins + 1" << endl;
      exit(1);
    }
    // clear bins
    double max_bins = var2.bins.back();
    double min_bins = var2.bins.front();
    var2.bins.clear();
    for (int i = 0; i < var2.nbins + 1; i++)
      var2.bins.push_back(i * (max_bins - min_bins) / (double)var2.nbins +
                          min_bins);
  }

  TH2D *hist = new TH2D(name_hist, title_hist, var1.nbins, var1.bins.data(),
                        var2.nbins, var2.bins.data());
  return hist;
}

TH3D *CreateHist3D(StrVar4Hist var1, StrVar4Hist var2, StrVar4Hist var3,
                   TString name_hist = "", TString title_hist = "") {
  if (name_hist == "")
    name_hist = var1.name + "_" + var2.name + "_" + var3.name;
  if (title_hist == "") {
    title_hist = var1.name + ":" + var2.name + ":" + var3.name + ";" +
                 var1.title + " [" + var1.unit + "];" + var2.title + " [" +
                 var2.unit + "];" + var3.title + " [" + var3.unit + "]";
  } else {
    title_hist = title_hist + ";" + var1.title + " [" + var1.unit + "];" +
                 var2.title + " [" + var2.unit + "];" + var3.title + " [" +
                 var3.unit + "]";
  }

  // bins check
  if (var1.bins.size() != var1.nbins + 1) {
    if (var1.bins.size() != 2) {
      cerr << "Error: bins size is not equal to nbins + 1" << endl;
      exit(1);
    }
    // clear bins
    double max_bins = var1.bins.back();
    double min_bins = var1.bins.front();
    var1.bins.clear();
    for (int i = 0; i < var1.nbins + 1; i++)
      var1.bins.push_back(i * (max_bins - min_bins) / (double)var1.nbins +
                          min_bins);
  }

  if (var2.bins.size() != var2.nbins + 1) {
    if (var2.bins.size() != 2) {
      cerr << "Error: bins size is not equal to nbins + 1" << endl;
      exit(1);
    }
    // clear bins
    double max_bins = var2.bins.back();
    double min_bins = var2.bins.front();
    var2.bins.clear();
    for (int i = 0; i < var2.nbins + 1; i++)
      var2.bins.push_back(i * (max_bins - min_bins) / (double)var2.nbins +
                          min_bins);
  }

  if (var3.bins.size() != var3.nbins + 1) {
    if (var3.bins.size() != 2) {
      cerr << "Error: bins size is not equal to nbins + 1" << endl;
      exit(1);
    }
    // clear bins
    double max_bins = var3.bins.back();
    double min_bins = var3.bins.front();
    var3.bins.clear();
    for (int i = 0; i < var3.nbins + 1; i++)
      var3.bins.push_back(i * (max_bins - min_bins) / (double)var3.nbins +
                          min_bins);
  }

  TH3D *hist =
      new TH3D(name_hist, title_hist, var1.nbins, var1.bins.data(), var2.nbins,
               var2.bins.data(), var3.nbins, var3.bins.data());
  return hist;
}

TH1D *CreateHist1D(StrVar4Hist var, TString name_hist = "",
                   TString title_hist = "") {
  if (name_hist == "")
    name_hist = var.name;
  if (title_hist == "") {
    title_hist = var.name + ";" + var.title + " [" + var.unit + "]";
  } else {
    title_hist = title_hist + ";" + var.title + " [" + var.unit + "]";
  }

  // bins check
  if (var.bins.size() != var.nbins + 1) {
    if (var.bins.size() != 2) {
      cerr << "Error: bins size is not equal to nbins + 1" << endl;
      exit(1);
    }
    // clear bins
    double max_bins = var.bins.back();
    double min_bins = var.bins.front();
    var.bins.clear();
    for (int i = 0; i < var.nbins + 1; i++)
      var.bins.push_back(i * (max_bins - min_bins) / var.nbins + min_bins);
  }

  TH1D *hist = new TH1D(name_hist, title_hist, var.nbins, var.bins.data());
  return hist;
}
} // namespace MHist

vector<array<int, 2>> GetIndexDeltaEta(double delta_eta) {
  vector<array<int, 2>> objReturn;
  for (int i = 0; i < 18; i++) {
    for (int j = 0; j < 18; j++) {
      double min_eta1 = GetBinDefLeftEdge(BinDefEta, i);
      double max_eta1 = GetBinDefRightEdge(BinDefEta, i);
      double min_eta2 = GetBinDefLeftEdge(BinDefEta, j);
      double max_eta2 = GetBinDefRightEdge(BinDefEta, j);

      double delta1 = abs(min_eta1 - min_eta2);
      double delta2 = abs(min_eta1 - max_eta2);
      double delta3 = abs(max_eta1 - min_eta2);
      double delta4 = abs(max_eta1 - max_eta2);
      double min_delta = min(delta1, min(delta2, min(delta3, delta4)));

      if (delta_eta <= min_delta)
        objReturn.push_back({i, j});
    }
  }
  return objReturn;
}

int GetIndexMult(double mult) {
  int bin = -1;
  for (int i = 0; i < MVariable::BinUsed_Mult.nbins; i++) {
    if (mult >= GetBinDefLeftEdge(BinDefMult, i) &&
        mult < GetBinDefRightEdge(BinDefMult, i)) {
      bin = i;
      break;
    }
  }
  return bin;
}

int GetIndexPt(double pt) {
  int bin = -1;
  for (int i = 0; i < MVariable::BinUsed_Pt.nbins; i++) {
    if (pt >= MVariable::BinUsed_Pt.bins[i] &&
        pt < MVariable::BinUsed_Pt.bins[i + 1]) {
      bin = i;
      break;
    }
  }
  return bin;
}

#endif