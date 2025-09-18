#ifndef __MFit_h__
#define __MFit_h__

#include "MHead.h"
#include "MMath.h"
#include "MRootGraphic.h"
#include "MRootIO.h"
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooCBShape.h" // Crystal Ball function
#include "RooCategory.h"
#include "RooChebychev.h"
#include "RooCrystalBall.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooExponential.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooGaussian.h"
#include "RooGenericPdf.h"
#include "RooHistPdf.h"
#include "RooPlot.h"
#include "RooPolynomial.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooWorkspace.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TPad.h"
#include "TRandom3.h"
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
  return obj;
}

RooChebychev *GetChebyshevPdf(int order, RooRealVar &x, TString name) {
  RooArgList coefList;
  for (int i = 0; i <= order; ++i) {
    coefList.add(*new RooRealVar(Form("c%d", i), Form("c%d", i), 0.1, -1, 1));
  }
  return new RooChebychev(name, "Chebyshev PDF", x, coefList);
}

} // namespace MFit
struct StrSignalFit {
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
};

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
  TF1 *fSignalFunction = nullptr;
  // RooSimultaneous *fSimPdf = nullptr;

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

  virtual void clean() {
    // fWs->clearStudies();
    delete fNsig;
    delete fNbkg;
    delete fX;
    // delete fPdf_signal;
    delete fPdf_bkg;
    delete fPdf_signal;
    delete fDataHist;
    delete fWs;
    delete fResult;
    // fResult->Delete();
  }

  virtual void InputData(TH1D *data) {
    if (!fWs) {
      cerr << "MSignalFit::operator<<: Workspace is not initialized!" << endl;
      exit(1);
    }
    // RooDataHist datahist("Data", "J/#{psi} ee decay", *fX, data);
    fDataHist = new RooDataHist("Data", "J/#psi ee decay", *fX, data);

    fWs->import(*fDataHist);
  }

  virtual void chi2Fit() {
    if (!fWs) {
      cerr << "MSignalFit::chi2FitTo: Workspace is not initialized!" << endl;
      exit(1);
    }
    fNsig->setConstant(false);
    fNbkg->setConstant(false);
    fResult = fModel->chi2FitTo(*fDataHist, RooFit::SumW2Error(true),
                                RooFit::Save(), RooFit::PrintLevel(-1));
  }

  virtual void Fit() {
    if (!fWs) {
      cerr << "MSignalFit::FitTo: Workspace is not initialized!" << endl;
      exit(1);
    }
    fNsig->setConstant(false);
    fNbkg->setConstant(false);
    fResult = fModel->fitTo(*fDataHist, RooFit::SumW2Error(true),
                            RooFit::Save(), RooFit::PrintLevel(-1));
  }

  virtual void RemoveLimit() {
    RooArgSet *params = fModel->getParameters(*fX);
    for (RooAbsArg *arg : *params) {
      RooRealVar *var = dynamic_cast<RooRealVar *>(arg);
      if (var) {
        var->removeMin();
        var->removeMax();
      }
    }
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

  virtual void CopySignal(MSignalFit otherFit) {
    if (!fWs) {
      cerr << "MSignalFit::CopySignal: Workspace is not initialized!" << endl;
      exit(1);
    }
    RooArgSet *params = otherFit.fPdf_signal->getParameters(*otherFit.fX);
    RooFitResult *fResult = otherFit.fResult;

    for (RooAbsArg *arg : *params) {
      TString name_arg = arg->GetName();
      // get the corresponding variable in this fit
      // RooRealVar *var = dynamic_cast<RooRealVar *>(fWs->arg(name_arg));
      RooRealVar *var = fModel->getParameters(*fX)->find(name_arg)
                            ? dynamic_cast<RooRealVar *>(
                                  fModel->getParameters(*fX)->find(name_arg))
                            : nullptr;
      auto other_var =
          dynamic_cast<RooRealVar *>(fResult->floatParsFinal().find(name_arg));
      if (var) {
        if (other_var) {
          var->setVal(other_var->getVal());
          var->setError(other_var->getError());
          if (other_var->hasMin()) {
            var->setMin(other_var->getMin());
          } else {
            var->removeMin();
          }
          if (other_var->hasMax()) {
            var->setMax(other_var->getMax());
          } else {
            var->removeMax();
          }
        }
      } else {
        cerr << "MSignalFit::CopySignal: Variable " << name_arg
             << " not found in the current fit!" << endl;
        exit(1);
      }
    }
  }

  virtual void CopyBkg(MSignalFit otherFit) {
    if (!fWs) {
      cerr << "MSignalFit::CopyBkg: Workspace is not initialized!" << endl;
      exit(1);
    }
    RooArgSet *params = otherFit.fPdf_bkg->getParameters(*otherFit.fX);
    RooFitResult *fResult = otherFit.fResult;

    for (RooAbsArg *arg : *params) {
      TString name_arg = arg->GetName();
      // get the corresponding variable in this fit
      RooRealVar *var = fModel->getParameters(*fX)->find(name_arg)
                            ? dynamic_cast<RooRealVar *>(
                                  fModel->getParameters(*fX)->find(name_arg))
                            : nullptr;
      auto other_var =
          dynamic_cast<RooRealVar *>(fResult->floatParsFinal().find(name_arg));
      if (var) {
        // RooRealVar *other_var = dynamic_cast<RooRealVar *>(arg);
        if (other_var) {
          var->setVal(other_var->getVal());
          var->setError(other_var->getError());
          if (other_var->hasMin()) {
            var->setMin(other_var->getMin());
          } else {
            var->removeMin();
          }
          if (other_var->hasMax()) {
            var->setMax(other_var->getMax());
          } else {
            var->removeMax();
          }
        }
      } else {
        cerr << "MSignalFit::CopyBkg: Variable " << name_arg
             << " not found in the current fit!" << endl;
        exit(1);
      }
    }
  }

  virtual void FixSignal(bool doFixBkg = true) {
    if (!fWs) {
      cerr << "MSignalFit::FixSignal: Workspace is not initialized!" << endl;
      exit(1);
    }
    RooArgSet *params = fPdf_signal->getParameters(*fX);
    for (RooAbsArg *arg : *params) {
      RooRealVar *var = dynamic_cast<RooRealVar *>(arg);
      if (var) {
        var->setConstant(doFixBkg);
      } else {
        cerr << "MSignalFit::FixSignal: Argument is not a RooRealVar!" << endl;
        exit(1);
      }
    }
  }

  virtual void FixBkg(bool doFixBkg = true) {
    if (!fWs) {
      cerr << "MSignalFit::FixBkg: Workspace is not initialized!" << endl;
      exit(1);
    }
    RooArgSet *params = fPdf_bkg->getParameters(*fX);
    for (RooAbsArg *arg : *params) {
      RooRealVar *var = dynamic_cast<RooRealVar *>(arg);
      if (var) {
        var->setConstant(doFixBkg);
      } else {
        cerr << "MSignalFit::FixBkg: Argument is not a RooRealVar!" << endl;
        exit(1);
      }
    }
  }

  enum TypeParam { kValue = 0, kError = 1, kLimitLow = 2, kLimitHigh = 3 };
  virtual void SetParam(TString name, double value, TypeParam type = kValue) {
    if (!fWs) {
      cerr << "MSignalFit::SetParam: Workspace is not initialized!" << endl;
      exit(1);
    }
    RooRealVar *var = dynamic_cast<RooRealVar *>(fWs->arg(name));
    if (var) {
      switch (type) {
      case kValue:
        var->setVal(value);
        break;
      case kError:
        var->setError(value);
        break;
      case kLimitLow:
        var->setMin(value);
        break;
      case kLimitHigh:
        var->setMax(value);
        break;
      default:
        cerr << "MSignalFit::SetParam: Invalid type!" << endl;
        exit(1);
      }
    } else {
      cerr << "MSignalFit::SetParam: Variable " << name
           << " not found in the workspace!" << endl;
      exit(1);
    }
  }

  virtual TGraph *GetSignalToBackgroundCurve(int nPoints = 100) {
    TGraph *graph = new TGraph(nPoints);
    double xMin = fX->getMin();
    double xMax = fX->getMax();
    double step = (xMax - xMin) / (nPoints - 1);

    RooArgSet normSet(*fX);

    for (int i = 0; i < nPoints; ++i) {
      double x = xMin + i * step;
      fX->setVal(x);

      double s = fPdf_signal->getVal(normSet);
      double b = fPdf_bkg->getVal(normSet);

      double S = fNsig->getVal() * s;
      double B = fNbkg->getVal() * b;

      double ratio = (B > 0) ? S / B : 0;
      graph->SetPoint(i, x, ratio);
    }

    graph->SetTitle("Signal to Background Ratio;Mass;S/B");
    graph->SetLineColor(kRed);
    return graph;
  }

  virtual TGraph *GetSignalFractionCurve(int nPoints = 100) {
    TGraph *graph = new TGraph(nPoints);
    double xMin = fX->getMin();
    double xMax = fX->getMax();
    double step = (xMax - xMin) / (nPoints - 1);

    RooArgSet normSet(*fX);

    for (int i = 0; i < nPoints; ++i) {
      double x = xMin + i * step;
      fX->setVal(x);

      double s = fPdf_signal->getVal(normSet);
      double b = fPdf_bkg->getVal(normSet);

      double S = fNsig->getVal() * s;
      double B = fNbkg->getVal() * b;

      double frac = (S + B > 0) ? S / (S + B) : 0;
      graph->SetPoint(i, x, frac);
    }

    graph->SetTitle("Signal Fraction S / (S + B);Mass;S/(S+B)");
    graph->SetLineColor(kBlue);
    return graph;
  }

  virtual TGraph *GetBkgFractionCurve(int nPoints = 100) {
    TGraph *graph = new TGraph(nPoints);
    double xMin = fX->getMin();
    double xMax = fX->getMax();
    double step = (xMax - xMin) / (nPoints - 1);

    RooArgSet normSet(*fX);

    for (int i = 0; i < nPoints; ++i) {
      double x = xMin + i * step;
      fX->setVal(x);

      double s = fPdf_signal->getVal(normSet);
      double b = fPdf_bkg->getVal(normSet);

      double S = fNsig->getVal() * s;
      double B = fNbkg->getVal() * b;

      double frac = (S + B > 0) ? B / (S + B) : 0;
      graph->SetPoint(i, x, frac);
    }

    graph->SetTitle("Background Fraction B / (S + B);Mass;B/(S+B)");
    graph->SetLineColor(kGreen);
    return graph;
  }

  virtual ~MSignalFit() { clean(); }

  virtual unique_ptr<TF1> GetSignalFunc(unique_ptr<TF1> f_signal = nullptr) {
    if (!fWs) {
      cerr << "MSignalFit::GetParamsSignal: Workspace is not initialized!"
           << endl;
      exit(1);
    }
    if (!f_signal) {
      f_signal = make_unique<TF1>(
          "f_signal",
          "ROOT::Math::crystalball_function(x,[Alpha],[N],[Sigma],[Mean])", 0,
          fX->getMax());
    }
    RooArgSet *params = fPdf_signal->getParameters(*fX);
    int nParams = params->getSize();
    double *values = new double[nParams];
    int index = 0;
    for (RooAbsArg *arg : *params) {
      RooRealVar *var = dynamic_cast<RooRealVar *>(arg);
      if (var) {
        values[index++] = var->getVal();
      }
    }
    f_signal->SetParameters(values);
    delete[] values;
    return f_signal;
  }
};

class MAssoYeildFit : public MSignalFit {
public:
  TString fName_AssoYeild;
  RooGenericPdf *fPdf_bkgAssoYeild;
  RooAddPdf *fModel_AssoYeild;
  RooSimultaneous *fSimPdf_AssoYeild = nullptr;

  RooRealVar *fNSig_AssoYeild;
  RooRealVar *fNBkg_AssoYeild;
  RooDataHist *fDataHist_AssoYeild = nullptr;
  RooDataSet *fDataSet_Simultaneout = nullptr;
  RooFitResult *fResult_AssoYeild = nullptr;

  MAssoYeildFit(TString name, TF1 *bkg_assoYeild, MSignalFit signalFit)
      : MSignalFit(signalFit) {
    fWs->cd();
    fName_AssoYeild = name;
    fPdf_bkgAssoYeild = MFit::GetGenericPdf(
        bkg_assoYeild, *fX, Form("bkg_assoYeild_%s", name.Data()));
    fNSig_AssoYeild = new RooRealVar(
        Form("nsig_assoYeild_%s", name.Data()),
        "Number of signal events for associated yield", 3.7184e+04);
    fNBkg_AssoYeild = new RooRealVar(
        Form("nbkg_assoYeild_%s", name.Data()),
        "Number of background events for associated yield", 1.4669e+04);
    fModel_AssoYeild =
        new RooAddPdf(Form("model_assoYeild_%s", name.Data()),
                      "Total PDF for associated yield",
                      RooArgList(*fPdf_signal, *fPdf_bkgAssoYeild),
                      RooArgList(*fNSig_AssoYeild, *fNBkg_AssoYeild));
  }

  void InputData_AssoYeild(TH1D *data) {
    if (!fWs) {
      cerr
          << "MAssoYeildFit::InputData_AssoYeild: Workspace is not initialized!"
          << endl;
      exit(1);
    }
    fWs->cd();
    RooCategory sample("sample", "sample");
    sample.defineType("mass");
    sample.defineType("assoYeild");

    //  RooDataSet("dataTotal", "combined data", RooArgSet(mass), Index(sample),
    //  Import("massFit", *dataMass), Import("v2Fit", dataV2));
    fDataHist_AssoYeild =
        new RooDataHist(Form("Data_%s", fName_AssoYeild.Data()),
                        "Associated yield of J/psi candidate", *fX, data);
    fDataSet_Simultaneout =
        new RooDataSet(Form("dataTotal_%s", fName_AssoYeild.Data()),
                       "combined data", RooArgSet(*fX), RooFit::Index(sample),
                       RooFit::Import("mass", *fDataHist),
                       RooFit::Import("assoYeild", *fDataHist_AssoYeild));

    fSimPdf_AssoYeild = new RooSimultaneous(
        Form("simPdf_assoYeild_%s", fName_AssoYeild.Data()),
        "simultaneous PDF for associated yield",
        {{"mass", fModel}, {"assoYeild", fModel_AssoYeild}}, sample);

    fWs->import(*fDataSet_Simultaneout);
  }

  void fit_AssoYeild() {
    if (!fWs) {
      cerr << "MAssoYeildFit::Chi2Fit_AssoYeild: Workspace is not initialized!"
           << endl;
      exit(1);
    }
    fWs->cd();
    RooDataHist *binnedDataSet = fDataSet_Simultaneout->binnedClone(
        Form("binnedData_%s", fName_AssoYeild.Data()), "binned data");
    fResult_AssoYeild = fSimPdf_AssoYeild->chi2FitTo(
        *binnedDataSet, /* RooFit::Extended(kTRUE), */ RooFit::Save(),
        RooFit::PrintLevel(-1), RooFit::SumW2Error(true));
  }

  void chi2Fit_AssoYeild() {
    if (!fWs) {
      cerr << "MAssoYeildFit::Chi2Fit_AssoYeild: Workspace is not initialized!"
           << endl;
      exit(1);
    }
    fWs->cd();
    // fX->setBins(fDataHist->arraySize());
    const RooAbsBinning &binning = fX->getBinning();

    RooDataHist *binnedDataSet = fDataSet_Simultaneout
                                     ->binnedClone(Form("binnedData_%s",
                                                        fName_AssoYeild.Data()),
                                                   "binned data" /* ,
     RooFit::Binning(binning) */);
    fResult_AssoYeild = fSimPdf_AssoYeild->chi2FitTo(
        *binnedDataSet, /* RooFit::Extended(kTRUE), */ RooFit::Save(),
        RooFit::PrintLevel(-1), RooFit::SumW2Error(true));
  }
};

class MSignalFitCheby {
public:
  RooWorkspace *fWs = nullptr;
  RooAddPdf *fModel = nullptr;
  RooGenericPdf *fPdf_signal;
  RooAbsPdf *fPdf_bkg;
  RooRealVar *fNsig;
  RooRealVar *fNbkg;
  RooRealVar *fX;
  RooFitResult *fResult;
  RooDataHist *fDataHist = nullptr;

  MSignalFitCheby(TString name, TF1 *signal, int order) {
    fWs = new RooWorkspace(name);
    fX = new RooRealVar("x", "M_{ee} [GeV/c^{2}]", 1.56, 5.0);
    fWs->import(*fX);
    fPdf_signal = MFit::GetGenericPdf(signal, *fX, "pdf_signal");
    fPdf_bkg = MFit::GetChebyshevPdf(order, *fX, "pdf_bkg");
    fNsig = new RooRealVar("nsig", "Number of signal events", 3.7184e+04);
    fNbkg = new RooRealVar("nbkg", "Number of background events", 1.4669e+04);
    fModel =
        new RooAddPdf("model", "Total PDF", RooArgList(*fPdf_signal, *fPdf_bkg),
                      RooArgList(*fNsig, *fNbkg));
    fWs->import(*fModel);
  }

  MSignalFitCheby(TString name, TF1 *signal, int order, double minX,
                  double maxX) {
    fWs = new RooWorkspace(name);
    fX = new RooRealVar("x", "M_{ee} [GeV/c^{2}]", minX, maxX);
    fWs->import(*fX);
    fPdf_signal = MFit::GetGenericPdf(signal, *fX, "pdf_signal");
    fPdf_bkg = MFit::GetChebyshevPdf(order, *fX, "pdf_bkg");
    fNsig = new RooRealVar("nsig", "Number of signal events", 3.7184e+04);
    fNbkg = new RooRealVar("nbkg", "Number of background events", 1.4669e+04);
    fModel =
        new RooAddPdf("model", "Total PDF", RooArgList(*fPdf_signal, *fPdf_bkg),
                      RooArgList(*fNsig, *fNbkg));
    fWs->import(*fModel);
  }

  virtual void clean() {
    delete fNsig;
    delete fNbkg;
    delete fX;
    delete fPdf_bkg;
    delete fPdf_signal;
    delete fDataHist;
    delete fWs;
    delete fResult;
  }

  virtual void InputData(TH1D *data) {
    if (!fWs) {
      std::cerr << "MSignalFitCheby::operator<<: Workspace is not initialized!"
                << std::endl;
      exit(1);
    }
    fDataHist = new RooDataHist("Data", "J/#psi ee decay", *fX, data);
    fWs->import(*fDataHist);
  }

  virtual void chi2Fit() {
    if (!fWs) {
      std::cerr << "MSignalFitCheby::chi2FitTo: Workspace is not initialized!"
                << std::endl;
      exit(1);
    }
    fNsig->setConstant(false);
    fNbkg->setConstant(false);
    fResult = fModel->chi2FitTo(*fDataHist, RooFit::SumW2Error(true),
                                RooFit::Save(), RooFit::PrintLevel(-1));
  }

  virtual void Fit() {
    if (!fWs) {
      std::cerr << "MSignalFitCheby::FitTo: Workspace is not initialized!"
                << std::endl;
      exit(1);
    }
    fNsig->setConstant(false);
    fNbkg->setConstant(false);
    fResult = fModel->fitTo(*fDataHist, RooFit::SumW2Error(true),
                            RooFit::Save(), RooFit::PrintLevel(-1));
  }

  virtual void RemoveLimit() {
    RooArgSet *params = fModel->getParameters(*fX);
    for (RooAbsArg *arg : *params) {
      RooRealVar *var = dynamic_cast<RooRealVar *>(arg);
      if (var) {
        var->removeMin();
        var->removeMax();
      }
    }
  }

  StrSignalFit getFitResult() const {
    StrSignalFit str_signal_fit;
    if (!fResult) {
      std::cerr << "MSignalFitCheby::getFitResult: Fit result is not available!"
                << std::endl;
      exit(1);
    }
    auto absReal_chi2 =
        fModel->createChi2(*fDataHist, RooFit::SumW2Error(true));
    double chi2 = absReal_chi2->getVal();
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
    frame->GetYaxis()->SetMaxDigits(2);
    frame->GetYaxis()->SetTitle("Entries / (0.04 GeV/c^{2})");
    frame->SetTitle("");

    fWs->data("Data")->plotOn(frame, RooFit::MarkerColor(kBlack),
                              RooFit::MarkerStyle(20),
                              RooFit::LineColor(kBlack));
    fModel->plotOn(frame, RooFit::Components("pdf_signal"),
                   RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
    fModel->plotOn(frame, RooFit::Components("pdf_bkg"),
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

  virtual void CopySignal(MSignalFitCheby otherFit) {
    if (!fWs) {
      std::cerr << "MSignalFitCheby::CopySignal: Workspace is not initialized!"
                << std::endl;
      exit(1);
    }
    RooArgSet *params = otherFit.fPdf_signal->getParameters(*otherFit.fX);
    RooFitResult *fResult = otherFit.fResult;

    for (RooAbsArg *arg : *params) {
      TString name_arg = arg->GetName();
      RooRealVar *var = fModel->getParameters(*fX)->find(name_arg)
                            ? dynamic_cast<RooRealVar *>(
                                  fModel->getParameters(*fX)->find(name_arg))
                            : nullptr;
      auto other_var =
          dynamic_cast<RooRealVar *>(fResult->floatParsFinal().find(name_arg));
      if (var) {
        if (other_var) {
          var->setVal(other_var->getVal());
          var->setError(other_var->getError());
          if (other_var->hasMin()) {
            var->setMin(other_var->getMin());
          } else {
            var->removeMin();
          }
          if (other_var->hasMax()) {
            var->setMax(other_var->getMax());
          } else {
            var->removeMax();
          }
        }
      } else {
        std::cerr << "MSignalFitCheby::CopySignal: Variable " << name_arg
                  << " not found in the current fit!" << std::endl;
        exit(1);
      }
    }
  }

  virtual void CopyBkg(MSignalFitCheby otherFit) {
    if (!fWs) {
      std::cerr << "MSignalFitCheby::CopyBkg: Workspace is not initialized!"
                << std::endl;
      exit(1);
    }
    RooArgSet *params = otherFit.fPdf_bkg->getParameters(*otherFit.fX);
    RooFitResult *fResult = otherFit.fResult;

    for (RooAbsArg *arg : *params) {
      TString name_arg = arg->GetName();
      RooRealVar *var = fModel->getParameters(*fX)->find(name_arg)
                            ? dynamic_cast<RooRealVar *>(
                                  fModel->getParameters(*fX)->find(name_arg))
                            : nullptr;
      auto other_var =
          dynamic_cast<RooRealVar *>(fResult->floatParsFinal().find(name_arg));
      if (var) {
        if (other_var) {
          var->setVal(other_var->getVal());
          var->setError(other_var->getError());
          if (other_var->hasMin()) {
            var->setMin(other_var->getMin());
          } else {
            var->removeMin();
          }
          if (other_var->hasMax()) {
            var->setMax(other_var->getMax());
          } else {
            var->removeMax();
          }
        }
      } else {
        std::cerr << "MSignalFitCheby::CopyBkg: Variable " << name_arg
                  << " not found in the current fit!" << std::endl;
        exit(1);
      }
    }
  }

  virtual void FixSignal(bool doFixBkg = true) {
    if (!fWs) {
      std::cerr << "MSignalFitCheby::FixSignal: Workspace is not initialized!"
                << std::endl;
      exit(1);
    }
    RooArgSet *params = fPdf_signal->getParameters(*fX);
    for (RooAbsArg *arg : *params) {
      RooRealVar *var = dynamic_cast<RooRealVar *>(arg);
      if (var) {
        var->setConstant(doFixBkg);
      } else {
        std::cerr << "MSignalFitCheby::FixSignal: Argument is not a RooRealVar!"
                  << std::endl;
        exit(1);
      }
    }
  }

  virtual void FixBkg(bool doFixBkg = true) {
    if (!fWs) {
      std::cerr << "MSignalFitCheby::FixBkg: Workspace is not initialized!"
                << std::endl;
      exit(1);
    }
    RooArgSet *params = fPdf_bkg->getParameters(*fX);
    for (RooAbsArg *arg : *params) {
      RooRealVar *var = dynamic_cast<RooRealVar *>(arg);
      if (var) {
        var->setConstant(doFixBkg);
      } else {
        std::cerr << "MSignalFitCheby::FixBkg: Argument is not a RooRealVar!"
                  << std::endl;
        exit(1);
      }
    }
  }

  enum TypeParam { kValue = 0, kError = 1, kLimitLow = 2, kLimitHigh = 3 };
  virtual void SetParam(TString name, double value, TypeParam type = kValue) {
    if (!fWs) {
      std::cerr << "MSignalFitCheby::SetParam: Workspace is not initialized!"
                << std::endl;
      exit(1);
    }
    RooRealVar *var = dynamic_cast<RooRealVar *>(fWs->arg(name));
    if (var) {
      switch (type) {
      case kValue:
        var->setVal(value);
        break;
      case kError:
        var->setError(value);
        break;
      case kLimitLow:
        var->setMin(value);
        break;
      case kLimitHigh:
        var->setMax(value);
        break;
      default:
        std::cerr << "MSignalFitCheby::SetParam: Invalid type!" << std::endl;
        exit(1);
      }
    } else {
      std::cerr << "MSignalFitCheby::SetParam: Variable " << name
                << " not found in the workspace!" << std::endl;
      exit(1);
    }
  }

  virtual TGraph *GetSignalToBackgroundCurve(int nPoints = 100) {
    TGraph *graph = new TGraph(nPoints);
    double xMin = fX->getMin();
    double xMax = fX->getMax();
    double step = (xMax - xMin) / (nPoints - 1);

    RooArgSet normSet(*fX);

    for (int i = 0; i < nPoints; ++i) {
      double x = xMin + i * step;
      fX->setVal(x);

      double s = fPdf_signal->getVal(normSet);
      double b = fPdf_bkg->getVal(normSet);

      double S = fNsig->getVal() * s;
      double B = fNbkg->getVal() * b;

      double ratio = (B > 0) ? S / B : 0;
      graph->SetPoint(i, x, ratio);
    }

    graph->SetTitle("Signal to Background Ratio;Mass;S/B");
    graph->SetLineColor(kRed);
    return graph;
  }

  virtual TGraph *GetSignalFractionCurve(int nPoints = 100) {
    TGraph *graph = new TGraph(nPoints);
    double xMin = fX->getMin();
    double xMax = fX->getMax();
    double step = (xMax - xMin) / (nPoints - 1);

    RooArgSet normSet(*fX);

    for (int i = 0; i < nPoints; ++i) {
      double x = xMin + i * step;
      fX->setVal(x);

      double s = fPdf_signal->getVal(normSet);
      double b = fPdf_bkg->getVal(normSet);

      double S = fNsig->getVal() * s;
      double B = fNbkg->getVal() * b;

      double frac = (S + B > 0) ? S / (S + B) : 0;
      graph->SetPoint(i, x, frac);
    }

    graph->SetTitle("Signal Fraction S / (S + B);Mass;S/(S+B)");
    graph->SetLineColor(kBlue);
    return graph;
  }

  virtual TGraph *GetBkgFractionCurve(int nPoints = 100) {
    TGraph *graph = new TGraph(nPoints);
    double xMin = fX->getMin();
    double xMax = fX->getMax();
    double step = (xMax - xMin) / (nPoints - 1);

    RooArgSet normSet(*fX);

    for (int i = 0; i < nPoints; ++i) {
      double x = xMin + i * step;
      fX->setVal(x);

      double s = fPdf_signal->getVal(normSet);
      double b = fPdf_bkg->getVal(normSet);

      double S = fNsig->getVal() * s;
      double B = fNbkg->getVal() * b;

      double frac = (S + B > 0) ? B / (S + B) : 0;
      graph->SetPoint(i, x, frac);
    }

    graph->SetTitle("Background Fraction B / (S + B);Mass;B/(S+B)");
    graph->SetLineColor(kGreen);
    return graph;
  }
};

MDouble GetSumWithError1D(TH1D *h, Double_t (*fcn)(Double_t)) {
  double value_temp = 0.0;
  double error_temp = 0.0;
  for (int i = 1; i <= h->GetNbinsX(); ++i) {
    value_temp += h->GetBinContent(i) * fcn(h->GetXaxis()->GetBinCenter(i));
  }
  value_temp /= (double)h->GetNbinsX();

  double error2 = 0.0;
  for (int i = 1; i <= h->GetNbinsX(); ++i) {
    double content_bin = h->GetBinContent(i);
    double error_bin = h->GetBinError(i);
    double value_bin = fcn(h->GetXaxis()->GetBinCenter(i));
    error2 += content_bin * content_bin * value_bin * value_bin * error_bin *
              error_bin;
  }
  error_temp = sqrt(error2);
  error_temp /= (double)h->GetNbinsX();

  return MDouble(value_temp, error_temp);
}

template <typename T> void PrintParams(T *pdf) {
  RooArgSet *params = pdf->getParameters(RooArgSet());
  for (RooAbsArg *arg : *params) {
    RooRealVar *var = dynamic_cast<RooRealVar *>(arg);
    if (var) {
      std::cout << var->GetName() << ": " << var->getVal() << " ± "
                << var->getError() << std::endl;
    }
  }
}

class MFitterPoly {
private:
  // predeclare MFitterVec
  struct MFitterVec {
    vector<double> fVec;
    // vector<double> fVec_substructed;
    MFitterVec() {}
    MFitterVec(int size) { fVec.resize(size, 0.0); }
    MFitterVec(vector<double> vec) { fVec = vec; }

    MFitterVec throwBasicVec(vector<MDiscreteFunc> vecFunc) {
      vector<double> coeffs;
      for (auto func : vecFunc) {
        double coeff = func * fVec;
        coeffs.push_back(coeff);
      }

      MDiscreteFunc func_reconstructed = MDiscreteFunc(fVec.size());
      for (size_t i = 0; i < vecFunc.size(); ++i) {
        func_reconstructed += vecFunc[i] * coeffs[i];
      }

      vector<double> vec_substructed(fVec.size(), 0.0);
      for (size_t i = 0; i < fVec.size(); ++i) {
        vec_substructed[i] = fVec[i] - func_reconstructed.Eval(i + 1);
      }
      return MFitterVec(vec_substructed);
    }

    void resize(int size) { fVec.resize(size, 0.0); }

    void clear() { fVec.clear(); }

    double &operator[](int index) { return fVec[index]; }

    operator const vector<double> &() const { return fVec; }

    MFitterVec operator+(const MFitterVec &other) {
      if (fVec.size() != other.fVec.size()) {
        std::cerr << "MFitterVec::operator+: Vector size does not match!"
                  << std::endl;
        exit(1);
      }
      MFitterVec result(fVec.size());
      for (size_t i = 0; i < fVec.size(); ++i) {
        result.fVec[i] = fVec[i] + other.fVec[i];
      }
      return result;
    }

    MFitterVec operator-(const MFitterVec &other) {
      if (fVec.size() != other.fVec.size()) {
        std::cerr << "MFitterVec::operator-: Vector size does not match!"
                  << std::endl;
        exit(1);
      }
      MFitterVec result(fVec.size());
      for (size_t i = 0; i < fVec.size(); ++i) {
        result.fVec[i] = fVec[i] - other.fVec[i];
      }
      return result;
    }

    MFitterVec operator*(double scalar) {
      MFitterVec result(fVec.size());
      for (size_t i = 0; i < fVec.size(); ++i) {
        result.fVec[i] = fVec[i] * scalar;
      }
      return result;
    }

    double operator*(const MFitterVec &other) {
      if (fVec.size() != other.fVec.size()) {
        std::cerr << "MFitterVec::operator*: Vector size does not match!"
                  << std::endl;
        exit(1);
      }
      double result = 0.0;
      for (size_t i = 0; i < fVec.size(); ++i) {
        result += fVec[i] * other.fVec[i];
      }
      return result;
    }

    MFitterVec &operator=(const MFitterVec &other) {
      if (fVec.size() != other.fVec.size()) {
        std::cerr << "MFitterVec::operator=: Vector size does not match!"
                  << std::endl;
        exit(1);
      }
      for (size_t i = 0; i < fVec.size(); ++i) {
        fVec[i] = other.fVec[i];
      }
      return *this;
    }

    MFitterVec &operator+=(const MFitterVec &other) {
      if (fVec.size() != other.fVec.size()) {
        std::cerr << "MFitterVec::operator+=: Vector size does not match!"
                  << std::endl;
        exit(1);
      }
      for (size_t i = 0; i < fVec.size(); ++i) {
        fVec[i] += other.fVec[i];
      }
      return *this;
    }

    MFitterVec &operator-=(const MFitterVec &other) {
      if (fVec.size() != other.fVec.size()) {
        std::cerr << "MFitterVec::operator-=: Vector size does not match!"
                  << std::endl;
        exit(1);
      }
      for (size_t i = 0; i < fVec.size(); ++i) {
        fVec[i] -= other.fVec[i];
      }
      return *this;
    }

    MFitterVec &operator*=(double scalar) {
      for (size_t i = 0; i < fVec.size(); ++i) {
        fVec[i] *= scalar;
      }
      return *this;
    }
  };

  double fx_min;
  double fx_max;
  double fx_min_plot;
  double fx_max_plot;
  double fw_bins;
  double fn_bins;
  vector<MDiscreteFunc> fBasisVecs;

  MFitterVec fYraws;
  MFitterVec fYsignal;
  int fnOrderPoly = 2;

  MFitterVec Project(const vector<double> &y_vals) {
    if (y_vals.size() != fn_bins) {
      std::cerr << "MFitterPoly::fit: y_vals size does not match fn_bins!"
                << std::endl;
      exit(1);
    }
    vector<double> coeffs;
    for (int n = 0; n <= fnOrderPoly; ++n) {
      double coeff = fBasisVecs[n] * y_vals;
      coeffs.push_back(coeff);
    }
    return MFitterVec(coeffs);
  }

public:
  TH1D *fHisto = nullptr;
  TH1D *fHisto_signal = nullptr;
  double fNSignal = 0.0;
  vector<double> fResults_fit;

  MFitterPoly(TH1D *h1, double x_min, double x_max) {
    fHisto = (TH1D *)h1->Clone(Form("histo_%d", GenerateUID()));
    fx_min = x_min;
    fx_max = x_max;
    fw_bins = h1->GetXaxis()->GetBinWidth(1);

    int bin_min = h1->GetXaxis()->FindBin(fx_min);
    fx_min = h1->GetXaxis()->GetBinLowEdge(bin_min);
    int bin_max = h1->GetXaxis()->FindBin(fx_max);
    fx_max = h1->GetXaxis()->GetBinUpEdge(bin_max);

    if (fx_min != x_min) {
      std::cout << "MFitterPoly: x_min is adjusted to " << fx_min << std::endl;
    }
    if (fx_max != x_max) {
      std::cout << "MFitterPoly: x_max is adjusted to " << fx_max << std::endl;
    }
    fx_min_plot = fx_min + fw_bins / 2.;
    fx_max_plot = fx_max - fw_bins / 2.;

    fn_bins = bin_max - bin_min + 1;
    fYraws.resize(fn_bins);
    for (int i = 0; i < fn_bins; ++i) {
      fYraws[i] = h1->GetBinContent(bin_min + i);
    }
  }

  ~MFitterPoly() {
    fBasisVecs.clear();
    fYraws.clear();
    if (fHisto)
      delete fHisto;
  }

  void initializeBasis(int order = 4) {
    fnOrderPoly = order;
    fBasisVecs = InitOrthogonalDiscreteFuncs(fn_bins, order);
  }

  void setHisto(TH1D *h1) {
    // bin alignment check
    double bin_width = h1->GetXaxis()->GetBinWidth(1);
    if (bin_width != fw_bins) {
      std::cerr << "MFitterPoly::setHisto: bin width does not match!"
                << std::endl;
      std::cerr << "  fw_bins = " << fw_bins << ", bin_width = " << bin_width
                << std::endl;
      exit(1);
    }
    int bin_min = h1->GetXaxis()->FindBin(fx_min);
    double new_x_min = h1->GetXaxis()->GetBinLowEdge(bin_min);
    if (new_x_min != fx_min) {
      std::cerr << "MFitterPoly::setHisto: x_min does not match!" << std::endl;
      exit(1);
    }

    // fHisto->Delete();
    fHisto = h1;

    vector<double> new_y_vals;
    new_y_vals.resize(fn_bins);
    for (int i = 0; i < fn_bins; ++i) {
      new_y_vals[i] = fHisto->GetBinContent(bin_min + i);
    }
    fYraws = MFitterVec(new_y_vals);
  }

  void inputSignal(TH1D *h_signal, int n_sampling = 10000000) {
    fHisto_signal =
        new TH1D(Form("template_signal_%d", GenerateUID()), "Signal Template",
                 fn_bins, fx_min, fx_min + fn_bins * fw_bins);
    for (int i = 0; i < n_sampling; ++i) {
      double x = h_signal->GetRandom();
      fHisto_signal->Fill(x);
    }
    fHisto_signal->Scale(1. / (double)n_sampling);

    fYsignal.resize(fn_bins);
    for (int i = 0; i < fn_bins; ++i) {
      fYsignal[i] = fHisto_signal->GetBinContent(i + 1);
    }
  };

  void inputSignal(TF1 *f_signal, int n_sampling = 10000000) {
    if (fHisto_signal)
      delete fHisto_signal;

    fHisto_signal =
        new TH1D(Form("template_signal_%d", GenerateUID()), "Signal Template",
                 fn_bins, fx_min, fx_min + fn_bins * fw_bins);
    for (int i = 0; i < n_sampling; ++i) {
      double x = f_signal->GetRandom();
      fHisto_signal->Fill(x);
    }
    fHisto_signal->Scale(1. / (double)n_sampling);

    fYsignal.resize(fn_bins);
    for (int i = 0; i < fn_bins; ++i) {
      fYsignal[i] = fHisto_signal->GetBinContent(i + 1);
    }
  }

  void fit() {
    vector<double> coeffs = Project(fYraws);

    MDiscreteFunc fitted_func(fn_bins);
    for (int n = 0; n <= fnOrderPoly; ++n) {
      fitted_func += fBasisVecs[n] * coeffs[n];
    }
    fResults_fit = fitted_func.GetPars(fx_min, fw_bins);
  }

  void fitWithSignal() {
    MFitterVec raw_substracted = fYraws.throwBasicVec(fBasisVecs);
    MFitterVec signal_substracted = fYsignal.throwBasicVec(fBasisVecs);

    double norm_raw = sqrt(raw_substracted * raw_substracted);
    double norm_signal = sqrt(signal_substracted * signal_substracted);

    // cout << "MFitterPoly::fitWithSignal: norm_raw = " << norm_raw
    //      << ", norm_signal = " << norm_signal << endl;

    double cosTheta =
        (raw_substracted * signal_substracted) / (norm_raw * norm_signal);

    // cout << "MFitterPoly::fitWithSignal: cosTheta = " << cosTheta << endl;

    if (cosTheta < 0) {
      cerr << "MFitterPoly::fitWithSignal: cosTheta < 0, set nsignal = 0"
           << endl;
      fNSignal = 0.0;
    } else {
      fNSignal = cosTheta * norm_raw / norm_signal;
    }

    // cout << "MFitterPoly::fitWithSignal: fNSignal = " << fNSignal << endl;

    MFitterVec raw_minus_signal = fYraws - fYsignal * fNSignal;

    vector<double> coeffs = Project(raw_minus_signal);
    MDiscreteFunc fitted_func(fn_bins);
    for (int n = 0; n <= fnOrderPoly; ++n) {
      fitted_func += fBasisVecs[n] * coeffs[n];
    }
    fResults_fit = fitted_func.GetPars(fx_min, fw_bins);
  }

  int GetNbins() const { return fn_bins; }
  double GetXmin() const { return fx_min; }
  double GetXmax() const { return fx_max; }
  double GetBinWidth() const { return fw_bins; }

  void SetRangePlot(double x_min, double x_max) {
    fx_min_plot = x_min;
    fx_max_plot = x_max;
  }

  void Draw() {
    gStyle->SetEndErrorSize(2.);
    auto raw = (TH1D *)fHisto->Clone(Form("histo_raw_%d", GenerateUID()));
    raw->GetXaxis()->SetRangeUser(fx_min, fx_max);
    TF1 *bg = new TF1(Form("bg_fit_%d", GenerateUID()),
                      Form("pol%d", fnOrderPoly), fx_min, fx_max);
    bg->SetParameters(fResults_fit.data());
    auto signal =
        (TH1D *)fHisto_signal->Clone(Form("histo_signal_%d", GenerateUID()));
    signal->Scale(fNSignal);
    auto fit_total =
        (TH1D *)signal->Clone(Form("histo_fit_total_%d", GenerateUID()));
    for (int i = 1; i <= fit_total->GetNbinsX(); ++i) {
      double x = fit_total->GetXaxis()->GetBinCenter(i);
      double y_bg = bg->Eval(x);
      double y_signal = signal->GetBinContent(i);
      fit_total->SetBinContent(i, y_bg + y_signal);
    }
    TGraph *gr_signal = new TGraph(signal);
    TGraph *gr_fitTotal = new TGraph(fit_total);

    MRootGraphic::StyleHistCommon(raw);
    // ============================================
    raw->SetTitle("");
    raw->GetYaxis()->SetTitle(Form("Entries / (%.2f GeV^{2}/c^{4})", fw_bins));
    double max_raw = raw->GetMaximum();
    raw->GetYaxis()->SetRangeUser(0, max_raw * 1.2);
    raw->GetXaxis()->SetRangeUser(fx_min_plot, fx_max_plot);
    raw->SetMarkerColor(kBlack);
    raw->SetMarkerColor(kBlack);
    raw->SetMarkerStyle(20);

    // ============================================
    gr_fitTotal->SetLineColor(kBlue);
    gr_fitTotal->SetLineWidth(2);

    // ============================================
    bg->SetLineColor(kGreen + 1);
    bg->SetLineStyle(kDashed);
    bg->SetLineWidth(3);

    // =====================================
    gr_signal->SetLineColor(kRed);
    gr_signal->SetMarkerStyle(0);
    gr_signal->SetLineStyle(kDashed);
    gr_signal->SetLineWidth(3);

    raw->Draw("E1");
    bg->Draw("same");
    gr_fitTotal->Draw("same C");
    gr_signal->Draw("same C");

    TLegend *legend = new TLegend(0.175, 0.75, 0.375, 0.89);
    legend->SetFillColor(0);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.035);
    legend->SetLineColor(0);
    legend->AddEntry(raw, "Data", "lep");
    legend->AddEntry(gr_signal, "Signal", "l");
    legend->AddEntry(bg, "Background", "l");
    legend->Draw("same");

    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.04);
    // tex->DrawLatex(0.55, 0.86, Form("N_{sig} = %.2f", fNSignal));
    // Draw fNsignal in scientific notation with 2 decimal places
    tex->DrawLatex(0.65, 0.8, Form("N_{sig} = %.2e", fNSignal));

    // tex->DrawLatex(0.55, 0.86 - 0.045,
    //                Form("N_{bkg} = %.2f", fit_result.fNbkg[0],
    //                     fit_result.fNbkg[1]));
    // tex->DrawLatex(0.55, 0.86 - 0.045 * 2,
    //                Form("#chi^{2}/NDF = %.2f", fit_result.chi2ToNdf));
    tex->Draw("same");
  }
};

#endif // __MFit_h__