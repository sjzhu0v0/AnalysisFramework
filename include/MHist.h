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

typedef struct StrVar4Hist {
  TString fName;
  TString fTitle;
  TString fUnit;
  const int fNbins;
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