#include "MHead.h"
#include "MSystem.h"

#ifndef MHist_h
#define MHist_h

#define BinUsed_Mult MultFt0c
#define BinUsed_Pt pT_JpsiFlow
#define BinUsed_Eta Eta_JpsiFlow
#define BinDefMass 20, 1, 5
#define BinDefEta 18, -0.9, 0.9
#define BinDefVtxZ 20, -10, 10
#define BinDefMult 5, 0, 2000

typedef struct StrVar4Hist;
vector<StrVar4Hist> gHistVar;
typedef struct StrVar4Hist {
  TString fName;
  TString fTitle;
  TString fUnit;
  const int fNbins;
  vector<double> fBins;
  StrVar4Hist(TString name, TString title, TString unit, int nbins,
              vector<double> bins)
      : fName(name), fTitle(title), fUnit(unit), fNbins(nbins), fBins(bins) {
    if (fBins.size() != fNbins + 1 && fBins.size() != 2) {
      cout << "Error: bins size is not correct" << endl;
      exit(1);
    }
    if (fBins.size() == 2) {
      fBins.clear();
      for (int i = 0; i <= fNbins; i++) {
        fBins.push_back(fBins[0] + i * (fBins[1] - fBins[0]) / fNbins);
      }
    }
    if (fUnit == "") {
      fUnit = "a.u.";
    }
  }
  TString CompleteTitle() { return fTitle + ";" + fName + " (" + fUnit + ")"; }
} StrVar4Hist;

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