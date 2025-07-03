#ifndef __MFit_h__
#define __MFit_h__

#include "MHead.h"
#include "MRootIO.h"
#include "RooAddPdf.h"
#include "RooCrystalBall.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooGenericPdf.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"
#include "tuple"

namespace MFit {
RooGenericPdf *GetGenericPdf(TF1 *f1, TString name_genericPdf = "") {
  if (name_genericPdf == "") {
    name_genericPdf = f1->GetName();
  }

  TString str_formula = f1->GetExpFormula();
  TList vars;
  for (int i = 0; i < f1->GetNpar(); ++i) {
    RooRealVar var(f1->GetParName(i), f1->GetParName(i), f1->GetParameter(i));
    double parmin, parmax;
    f1->GetParLimits(i, parmin, parmax);
    if (parmin != parmax)
      var.setRange(parmin, parmax);
    vars.Add(&var);
  }
  return (new RooGenericPdf(name_genericPdf, str_formula, RooArgList(vars)));
}
RooGenericPdf *GetGenericPdf(TString path_tf1, TString name_genericPdf = "") {
  TF1 *f1 = MRootIO::GetObjectDiectly<TF1>(path_tf1);
  return GetGenericPdf(f1, name_genericPdf);
}
RooGenericPdf *GetGenericPdf(TFile *f, TString name_tf1,
                             TString name_genericPdf = "") {
  TF1 *f1 = MRootIO::GetObjectDiectly<TF1>(f, name_tf1);
  return GetGenericPdf(f1, name_genericPdf);
}
} // namespace MFit

class MSignalFit {
public:
  RooWorkspace *ws = nullptr;
  RooAddPdf *model = nullptr;
  RooGenericPdf *pdf_signal;
  RooGenericPdf *pdf_bkg;
  RooRealVar *nsig;
  RooRealVar *nbkg;

  MSignalFit(TString name, TF1 *signal, TF1 *bkg) {
    ws = new RooWorkspace(name);
    pdf_signal = MFit::GetGenericPdf(signal, "pdf_signal");
    pdf_bkg = MFit::GetGenericPdf(bkg, "pdf_bkg");
    nsig = new RooRealVar("nsig", "Number of signal events", 3.7184e+04);
    nbkg = new RooRealVar("nbkg", "Number of background events", 1.4669e+04);
    ws->import(*pdf_signal);
    ws->import(*pdf_bkg);
    ws->import(*nsig);
    ws->import(*nbkg);

    model =
        new RooAddPdf("model", "Total PDF", RooArgList(*pdf_signal, *pdf_bkg),
                      RooArgList(*nsig, *nbkg));
    ws->import(*model);
  }

  virtual void operator<<(RooDataHist data) {
    if (!ws) {
      cout << "Workspace is not initialized!" << endl;
      return;
    }
    ws->import(data);
    RooFitResult *result =
        model->chi2FitTo(data, RooFit::SumW2Error(true), RooFit::Save());
    result->Print();
  }
};

#endif // __MFit_h__