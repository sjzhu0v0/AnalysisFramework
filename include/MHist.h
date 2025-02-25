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

#define HistDefine(histType, histName, histTitle, ...)                         \
  histType *histName = new histType(                                           \
      #histName, TString(#histName) + ";" + TString(histTitle), __VA_ARGS__);

#define HistDefine1D(histName, histTitle, ...)                                 \
  HistDefine(TH1D, histName, histTitle, __VA_ARGS__)

#define HistDefine2D(histName, histTitle, ...)                                 \
  HistDefine(TH2D, histName, histTitle, __VA_ARGS__)

// double axisPt1D[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.8, 1.2, 1.6, 2.4, 3.2, 4};
// #define axisPt1D 10, axisPt1D
// HistDefine1D(h1_DCAxyCut, "p_{T} [GeV/c]; DCAxy Cut [cm]", axisPt1D);
// #define axisTrackV1D 10000, -1.e-1, 1.e-1
// #define axisDcakV1D 10000, -1.e-1, 1.e-1
//   double axisPt1D[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.8, 1.2, 1.6, 2.4, 3.2, 4};
// #define axisPt1D 10, axisPt1D
// #define axisDeltaPtFrac 100, -0.2, 0.2
// #define HistDefineTrackV(dim) \
//   HistDefine1D(h1_trackV##dim, "V" #dim " [cm]", axisTrackV1D); \
//   HistDefine1D(h1_PrimaryTrackV##dim, "V" #dim " [cm]", axisTrackV1D)

//   HistDefineTrackV(x);
//   HistDefineTrackV(y);
//   HistDefineTrackV(z);

//   HistDefine1D(h1_trackRecDCAxy, "Rec DCAxy [cm]", axisDcakV1D);
//   HistDefine1D(h1_trackRecDCAz, "Rec DCAz [cm]", axisDcakV1D);
//   HistDefine1D(h1_trackMcDCAxy, "MC DCAxy [cm]", axisDcakV1D);
//   HistDefine1D(h1_trackMcDCAz, "MC DCAz [cm]", axisDcakV1D);
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