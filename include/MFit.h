#ifndef __MFit_h__
#define __MFit_h__

#include "MHead.h"
#include "MRootGraphic.h"
#include "MRootIO.h"
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooCrystalBall.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooGenericPdf.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"
#include "TF1.h"
#include "TLegend.h"
#include "TString.h"
#include "tuple"
#include <iostream>
#include <vector>

namespace MFit {

RooGenericPdf *GetGenericPdf(TF1 *f1, RooRealVar &x,
                             TString name_genericPdf = "") {
  if (name_genericPdf == "") {
    name_genericPdf = f1->GetName();
  }

  // Convert formula from [0], [1], etc. to parameter names
  TString str_formula = f1->GetExpFormula();
  for (int i = 0; i < f1->GetNpar(); ++i) {
    TString old_str = Form("[%d]", i);
    TString new_str = f1->GetParName(i);
    str_formula.ReplaceAll(old_str, new_str);
  }
  str_formula.ReplaceAll("[", "");
  str_formula.ReplaceAll("]", "");
  std::cout << "Converted formula: " << str_formula.Data() << std::endl;

  // Prepare variable list
  RooArgList vars;
  vars.add(x);

  // Keep track of created RooRealVars to manage memory
  std::vector<RooRealVar *> owned_vars;

  for (int i = 0; i < f1->GetNpar(); ++i) {
    double value = f1->GetParameter(i);
    double error = f1->GetParError(i);
    TString pname = f1->GetParName(i);
    double delta = (error > 0) ? 3 * error : 1.0;

    auto *var =
        new RooRealVar(pname, pname, value, value - delta, value + delta);
    owned_vars.push_back(var);
    vars.add(*var); // No addOwned
  }

  // Construct the RooGenericPdf
  auto *obj = new RooGenericPdf(name_genericPdf, f1->GetYaxis()->GetTitle(),
                                str_formula.Data(), vars);

  std::cout << "PDF '" << name_genericPdf << "' created with "
            << obj->getParameters(x)->getSize() << " parameters." << std::endl;

  return obj;
}

} // namespace MFit
typedef struct StrSignalFit {
  using ParamView = std::array<double, 2>;
  ParamView fNsig;
  ParamView fNbkg;
  double chi2ToNdf;

  void print() const {
    std::cout << "Signal Fit Results:" << std::endl;
    std::cout << "  Nsig: " << fNsig[0] << " ± " << fNsig[1] << std::endl;
    std::cout << "  Nbkg: " << fNbkg[0] << " ± " << fNbkg[1] << std::endl;
    std::cout << "  Chi2/NDF: " << chi2ToNdf << std::endl;
  }

} StrSignalFit;

class MSignalFit {

public:
  RooWorkspace *fWs = nullptr;
  RooAddPdf *fModel = nullptr;
  RooGenericPdf *fPdf_signal;
  RooGenericPdf *fPdf_bkg;
  RooRealVar *fNsig;
  RooRealVar *fNbkg;
  RooRealVar *fX;
  RooFitResult *fResult;
  RooDataHist *fDataHist = nullptr;

  MSignalFit(TString name, TF1 *signal, TF1 *bkg) {
    fWs = new RooWorkspace(name);
    fX = new RooRealVar("x", "M_{ee} [GeV/c^{2}]", 1.56, 5.0);
    fWs->import(*fX);
    fPdf_signal = MFit::GetGenericPdf(signal, *fX, "pdf_signal");
    fPdf_bkg = MFit::GetGenericPdf(bkg, *fX, "pdf_bkg");
    fNsig = new RooRealVar("nsig", "Number of signal events", 3.7184e+04);
    fNbkg = new RooRealVar("nbkg", "Number of background events", 1.4669e+04);
    fModel =
        new RooAddPdf("model", "Total PDF", RooArgList(*fPdf_signal, *fPdf_bkg),
                      RooArgList(*fNsig, *fNbkg));
    fWs->import(*fModel);
  }

  MSignalFit(TString name, TF1 *signal, TF1 *bkg, double minX, double maxX) {
    fWs = new RooWorkspace(name);
    fX = new RooRealVar("x", "M_{ee} [GeV/c^{2}]", minX, maxX);
    fWs->import(*fX);
    fPdf_signal = MFit::GetGenericPdf(signal, *fX, "pdf_signal");
    fPdf_bkg = MFit::GetGenericPdf(bkg, *fX, "pdf_bkg");
    fNsig = new RooRealVar("nsig", "Number of signal events", 3.7184e+04);
    fNbkg = new RooRealVar("nbkg", "Number of background events", 1.4669e+04);
    fModel =
        new RooAddPdf("model", "Total PDF", RooArgList(*fPdf_signal, *fPdf_bkg),
                      RooArgList(*fNsig, *fNbkg));
    fWs->import(*fModel);
  }

  virtual void operator<<(TH1D *data) {
    if (!fWs) {
      cerr << "MSignalFit::operator<<: Workspace is not initialized!" << endl;
      exit(1);
    }
    // RooDataHist datahist("Data", "J/#{psi} ee decay", *fX, data);
    fDataHist = new RooDataHist("Data", "J/#psi ee decay", *fX, data);
    fWs->import(*fDataHist);
  }

  virtual void chi2Fit(bool doFixTemplate = true) {
    if (!fWs) {
      cerr << "MSignalFit::chi2FitTo: Workspace is not initialized!" << endl;
      exit(1);
    }
    if (doFixTemplate) {
      RooArgSet *params = fModel->getParameters(*fX);
      for (auto &param : *params) {
        RooRealVar *var = dynamic_cast<RooRealVar *>(param);
        var->setConstant(true);
      }
      fNsig->setConstant(false);
      fNbkg->setConstant(false);
    } else {
      RooArgSet *params = fModel->getParameters(*fX);
      for (auto &param : *params) {
        RooRealVar *var = dynamic_cast<RooRealVar *>(param);
        var->setConstant(false);
      }
    }
    fResult = fModel->chi2FitTo(*fDataHist, RooFit::SumW2Error(true),
                                RooFit::Save(), RooFit::PrintLevel(-1));
  }

  StrSignalFit getFitResult() const {
    StrSignalFit str_signal_fit;
    if (!fResult) {
      cerr << "MSignalFit::getFitResult: Fit result is not available!" << endl;
      exit(1);
    }
    auto absReal_chi2 =
        fModel->createChi2(*fDataHist, RooFit::SumW2Error(true));
    double chi2 = absReal_chi2->getVal();
    cout << "Chi2: " << chi2 << endl;
    int nBins = fDataHist->numEntries();

    int ndf = nBins - fModel->getParameters(*fX)->getSize();
    str_signal_fit.chi2ToNdf = (ndf > 0) ? chi2 / ndf : 0.0;
    str_signal_fit.fNsig = {fNsig->getVal(), fNsig->getError()};
    str_signal_fit.fNbkg = {fNbkg->getVal(), fNbkg->getError()};
    return str_signal_fit;
  }

  virtual void operator>>(TPad *pad) {
    pad->cd();
    RooPlot *frame = fX->frame();
    MRootGraphic::StyleHistCommon(frame);
    frame->GetYaxis()->SetMaxDigits(2);
    frame->GetYaxis()->SetTitle("Entries / (0.04 GeV/c^{2})");
    frame->SetTitle("");

    fWs->data("Data")->plotOn(frame, RooFit::MarkerColor(kBlack),
                              RooFit::MarkerStyle(20),
                              RooFit::LineColor(kBlack));
    fModel->plotOn(frame, RooFit::Components(*fPdf_signal),
                   RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
    fModel->plotOn(frame, RooFit::Components(*fPdf_bkg),
                   RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));
    fModel->plotOn(frame);
    StrSignalFit fit_result = getFitResult();
    frame->Draw();
    TLegend *legend = new TLegend(0.175, 0.75, 0.375, 0.89);
    legend->SetFillColor(0);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.035);
    legend->SetLineColor(0);
    legend->AddEntry(frame->getObject(0), "Data", "lep");
    legend->AddEntry(frame->getObject(1), "Signal", "l");
    legend->AddEntry(frame->getObject(2), "Background", "l");
    legend->Draw("same");

    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.03);
    tex->DrawLatex(0.55, 0.86,
                   Form("N_{sig} = %.2f #pm %.2f", fit_result.fNsig[0],
                        fit_result.fNsig[1]));
    tex->DrawLatex(0.55, 0.86 - 0.045,
                   Form("N_{bkg} = %.2f #pm %.2f", fit_result.fNbkg[0],
                        fit_result.fNbkg[1]));
    tex->DrawLatex(0.55, 0.86 - 0.045 * 2,
                   Form("#chi^{2}/NDF = %.2f", fit_result.chi2ToNdf));
    tex->Draw("same");
  }
};

#endif // __MFit_h__