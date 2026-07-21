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

  virtual ~MSignalFit() { clean(); }

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

class MAssoYieldFit : public MSignalFit {
public:
  TString fName_AssoYield;
  RooGenericPdf *fPdf_bkgAssoYield;
  RooAddPdf *fModel_AssoYield;
  RooSimultaneous *fSimPdf_AssoYield = nullptr;

  RooRealVar *fNSig_AssoYield;
  RooRealVar *fNBkg_AssoYield;
  RooDataHist *fDataHist_AssoYield = nullptr;
  RooDataSet *fDataSet_Simultaneout = nullptr;
  RooFitResult *fResult_AssoYield = nullptr;

  MAssoYieldFit(TString name, TF1 *bkg_assoYield, MSignalFit signalFit)
      : MSignalFit(signalFit) {
    fWs->cd();
    fName_AssoYield = name;
    fPdf_bkgAssoYield = MFit::GetGenericPdf(
        bkg_assoYield, *fX, Form("bkg_assoYield_%s", name.Data()));
    fNSig_AssoYield = new RooRealVar(
        Form("nsig_assoYield_%s", name.Data()),
        "Number of signal events for associated yield", 3.7184e+04);
    fNBkg_AssoYield = new RooRealVar(
        Form("nbkg_assoYield_%s", name.Data()),
        "Number of background events for associated yield", 1.4669e+04);
    fModel_AssoYield =
        new RooAddPdf(Form("model_assoYield_%s", name.Data()),
                      "Total PDF for associated yield",
                      RooArgList(*fPdf_signal, *fPdf_bkgAssoYield),
                      RooArgList(*fNSig_AssoYield, *fNBkg_AssoYield));
  }

  void InputData_AssoYield(TH1D *data) {
    if (!fWs) {
      cerr
          << "MAssoYieldFit::InputData_AssoYield: Workspace is not initialized!"
          << endl;
      exit(1);
    }
    fWs->cd();
    RooCategory sample("sample", "sample");
    sample.defineType("mass");
    sample.defineType("assoYield");

    //  RooDataSet("dataTotal", "combined data", RooArgSet(mass), Index(sample),
    //  Import("massFit", *dataMass), Import("v2Fit", dataV2));
    fDataHist_AssoYield =
        new RooDataHist(Form("Data_%s", fName_AssoYield.Data()),
                        "Associated yield of J/psi candidate", *fX, data);
    fDataSet_Simultaneout =
        new RooDataSet(Form("dataTotal_%s", fName_AssoYield.Data()),
                       "combined data", RooArgSet(*fX), RooFit::Index(sample),
                       RooFit::Import("mass", *fDataHist),
                       RooFit::Import("assoYield", *fDataHist_AssoYield));

    fSimPdf_AssoYield = new RooSimultaneous(
        Form("simPdf_assoYield_%s", fName_AssoYield.Data()),
        "simultaneous PDF for associated yield",
        {{"mass", fModel}, {"assoYield", fModel_AssoYield}}, sample);

    fWs->import(*fDataSet_Simultaneout);
  }

  void fit_AssoYield() {
    if (!fWs) {
      cerr << "MAssoYieldFit::Chi2Fit_AssoYield: Workspace is not initialized!"
           << endl;
      exit(1);
    }
    fWs->cd();
    RooDataHist *binnedDataSet = fDataSet_Simultaneout->binnedClone(
        Form("binnedData_%s", fName_AssoYield.Data()), "binned data");
    fResult_AssoYield = fSimPdf_AssoYield->chi2FitTo(
        *binnedDataSet, /* RooFit::Extended(kTRUE), */ RooFit::Save(),
        RooFit::PrintLevel(-1), RooFit::SumW2Error(true));
  }

  void chi2Fit_AssoYield() {
    if (!fWs) {
      cerr << "MAssoYieldFit::Chi2Fit_AssoYield: Workspace is not initialized!"
           << endl;
      exit(1);
    }
    fWs->cd();
    // fX->setBins(fDataHist->arraySize());
    const RooAbsBinning &binning = fX->getBinning();

    RooDataHist *binnedDataSet = fDataSet_Simultaneout
                                     ->binnedClone(Form("binnedData_%s",
                                                        fName_AssoYield.Data()),
                                                   "binned data" /* ,
     RooFit::Binning(binning) */);
    fResult_AssoYield = fSimPdf_AssoYield->chi2FitTo(
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

#include "MMath.h"

class MFitterPoly {
private:
  struct MFitterVec {
    std::vector<double> fVec;

    MFitterVec() = default;
    MFitterVec(int size) { fVec.resize(size, 0.0); }
    MFitterVec(const std::vector<double> &vec) : fVec(vec) {}

    MFitterVec throwBasicVec(std::vector<MDiscreteFunc> vecFunc) const {
      std::vector<double> coeffs;
      for (auto func : vecFunc) {
        double coeff = func * fVec;
        coeffs.push_back(coeff);
      }

      MDiscreteFunc func_reconstructed(fVec.size());
      for (size_t i = 0; i < vecFunc.size(); ++i) {
        func_reconstructed += vecFunc[i] * coeffs[i];
      }

      std::vector<double> vec_substructed(fVec.size(), 0.0);
      for (size_t i = 0; i < fVec.size(); ++i) {
        vec_substructed[i] = fVec[i] - func_reconstructed.Eval(i + 1);
      }
      return MFitterVec(vec_substructed);
    }

    void resize(int size) { fVec.resize(size, 0.0); }
    void clear() { fVec.clear(); }
    size_t size() const { return fVec.size(); }

    double &operator[](int index) { return fVec[index]; }
    const double &operator[](int index) const { return fVec[index]; }

    operator const std::vector<double> &() const { return fVec; }

    // Arithmetic operators
    MFitterVec operator+(const MFitterVec &other) const {
      if (fVec.size() != other.fVec.size()) {
        std::cerr << "MFitterVec::operator+: size mismatch!" << std::endl;
        std::exit(1);
      }
      MFitterVec result(fVec.size());
      for (size_t i = 0; i < fVec.size(); ++i)
        result.fVec[i] = fVec[i] + other.fVec[i];
      return result;
    }

    MFitterVec operator-(const MFitterVec &other) const {
      if (fVec.size() != other.fVec.size()) {
        std::cerr << "MFitterVec::operator-: size mismatch!" << std::endl;
        std::exit(1);
      }
      MFitterVec result(fVec.size());
      for (size_t i = 0; i < fVec.size(); ++i)
        result.fVec[i] = fVec[i] - other.fVec[i];
      return result;
    }

    MFitterVec operator*(double scalar) const {
      MFitterVec result(fVec.size());
      for (size_t i = 0; i < fVec.size(); ++i)
        result.fVec[i] = fVec[i] * scalar;
      return result;
    }

    double operator*(const MFitterVec &other) const {
      if (fVec.size() != other.fVec.size()) {
        std::cerr << "MFitterVec::operator*: size mismatch!" << std::endl;
        std::exit(1);
      }
      double result = 0.0;
      for (size_t i = 0; i < fVec.size(); ++i)
        result += fVec[i] * other.fVec[i];
      return result;
    }

    // Assignment operator — safe and idiomatic
    MFitterVec &operator=(const MFitterVec &other) {
      if (this != &other) {
        fVec = other.fVec; // std::vector handles resize + copy
      }
      return *this;
    }

    MFitterVec &operator+=(const MFitterVec &other) {
      if (fVec.size() != other.fVec.size()) {
        std::cerr << "MFitterVec::operator+=: size mismatch!" << std::endl;
        std::exit(1);
      }
      for (size_t i = 0; i < fVec.size(); ++i)
        fVec[i] += other.fVec[i];
      return *this;
    }

    MFitterVec &operator-=(const MFitterVec &other) {
      if (fVec.size() != other.fVec.size()) {
        std::cerr << "MFitterVec::operator-=: size mismatch!" << std::endl;
        std::exit(1);
      }
      for (size_t i = 0; i < fVec.size(); ++i)
        fVec[i] -= other.fVec[i];
      return *this;
    }

    MFitterVec &operator*=(double scalar) {
      for (auto &v : fVec)
        v *= scalar;
      return *this;
    }
  };

  double fx_min;
  double fx_max;
  double fx_min_plot;
  double fx_max_plot;
  double fw_bins;
  int fn_bins; // changed to int for consistency
  std::vector<MDiscreteFunc> fBasisVecs;

  MFitterVec fYraws;
  MFitterVec fYsignal;
  int fnOrderPoly = 2;

  bool fOwnsHisto = true;        // 👈 新增：管理 fHisto 所有权
  bool fOwnsHistoSignal = false; // 👈 新增：管理 fHisto_signal 所有权

  MFitterVec Project(const std::vector<double> &y_vals) {
    if ((int)y_vals.size() != fn_bins) {
      std::cerr << "MFitterPoly::Project: y_vals size != fn_bins!" << std::endl;
      std::exit(1);
    }

    // 🔒 安全检查：确保 fBasisVecs 足够大
    if ((int)fBasisVecs.size() <= fnOrderPoly) {
      std::cerr << "MFitterPoly::Project: fBasisVecs not initialized for order "
                << fnOrderPoly << " (size=" << fBasisVecs.size() << ")"
                << std::endl;
      std::exit(1);
    }

    std::vector<double> coeffs;
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
  std::vector<double> fResults_fit;

  MFitterPoly(TH1D *h1, double x_min, double x_max) {
    if (!h1) {
      std::cerr << "MFitterPoly: input histogram is null!" << std::endl;
      std::exit(1);
    }
    fHisto = (TH1D *)h1->Clone(Form("histo_%d", GenerateUID()));
    fOwnsHisto = true;

    fx_min = x_min;
    fx_max = x_max;
    fw_bins = h1->GetXaxis()->GetBinWidth(1);

    int bin_min = h1->GetXaxis()->FindBin(fx_min);
    fx_min = h1->GetXaxis()->GetBinLowEdge(bin_min);
    int bin_max = h1->GetXaxis()->FindBin(fx_max);
    fx_max = h1->GetXaxis()->GetBinUpEdge(bin_max);

    if (fx_min != x_min) {
      std::cout << "MFitterPoly: x_min adjusted to " << fx_min << std::endl;
    }
    if (fx_max != x_max) {
      std::cout << "MFitterPoly: x_max adjusted to " << fx_max << std::endl;
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
    if (fOwnsHisto && fHisto) {
      delete fHisto;
      fHisto = nullptr;
    }
    if (fOwnsHistoSignal && fHisto_signal) {
      delete fHisto_signal;
      fHisto_signal = nullptr;
    }
  }

  void initializeBasis(int order = 4) {
    fnOrderPoly = order;
    fBasisVecs = InitOrthogonalDiscreteFuncs(fn_bins, order);
    // Optional: verify size
    if ((int)fBasisVecs.size() != order + 1) {
      std::cerr << "Warning: InitOrthogonalDiscreteFuncs returned "
                << fBasisVecs.size() << " functions, expected " << order + 1
                << std::endl;
    }
  }

  void setHisto(TH1D *h1) {
    if (!h1) {
      std::cerr << "MFitterPoly::setHisto: null pointer!" << std::endl;
      std::exit(1);
    }

    double bin_width = h1->GetXaxis()->GetBinWidth(1);
    if (std::abs(bin_width - fw_bins) > 1e-6) {
      std::cerr << "MFitterPoly::setHisto: bin width mismatch!" << std::endl;
      std::cerr << "  expected = " << fw_bins << ", got = " << bin_width
                << std::endl;
      std::exit(1);
    }

    int bin_min = h1->GetXaxis()->FindBin(fx_min);
    double new_x_min = h1->GetXaxis()->GetBinLowEdge(bin_min);
    if (std::abs(new_x_min - fx_min) > 1e-6) {
      std::cerr << "MFitterPoly::setHisto: x_min mismatch!" << std::endl;
      std::exit(1);
    }

    // Clean up old histogram if we own it
    if (fOwnsHisto && fHisto) {
      delete fHisto;
    }

    // Clone the new one — we take ownership
    fHisto = (TH1D *)h1->Clone(Form("histo_set_%d", GenerateUID()));
    fOwnsHisto = true;

    // Update fYraws
    std::vector<double> new_y_vals(fn_bins);
    for (int i = 0; i < fn_bins; ++i) {
      new_y_vals[i] = fHisto->GetBinContent(bin_min + i);
    }
    fYraws = MFitterVec(new_y_vals);
  }

  void inputSignal(TH1D *h_signal, int n_sampling = 10000000) {
    if (fOwnsHistoSignal && fHisto_signal) {
      delete fHisto_signal;
    }

    fHisto_signal =
        new TH1D(Form("template_signal_%d", GenerateUID()), "Signal Template",
                 fn_bins, fx_min, fx_min + fn_bins * fw_bins);
    fOwnsHistoSignal = true;

    for (int i = 0; i < n_sampling; ++i) {
      double x = h_signal->GetRandom();
      fHisto_signal->Fill(x);
    }
    fHisto_signal->Scale(1.0 / n_sampling);

    fYsignal.resize(fn_bins);
    for (int i = 0; i < fn_bins; ++i) {
      fYsignal[i] = fHisto_signal->GetBinContent(i + 1);
    }
  }

  void inputSignal(TF1 *f_signal, int n_sampling = 10000000) {
    if (fOwnsHistoSignal && fHisto_signal) {
      delete fHisto_signal;
    }

    fHisto_signal =
        new TH1D(Form("template_signal_%d", GenerateUID()), "Signal Template",
                 fn_bins, fx_min, fx_min + fn_bins * fw_bins);
    fOwnsHistoSignal = true;

    for (int i = 0; i < n_sampling; ++i) {
      double x = f_signal->GetRandom();
      fHisto_signal->Fill(x);
    }
    fHisto_signal->Scale(1.0 / n_sampling);

    fYsignal.resize(fn_bins);
    for (int i = 0; i < fn_bins; ++i) {
      fYsignal[i] = fHisto_signal->GetBinContent(i + 1);
    }
  }

  void fit() {
    std::vector<double> coeffs_vec = Project(fYraws);
    std::vector<double> coeffs(coeffs_vec.begin(), coeffs_vec.end());

    MDiscreteFunc fitted_func(fn_bins);
    for (int n = 0; n <= fnOrderPoly; ++n) {
      fitted_func += fBasisVecs[n] * coeffs[n];
    }
    fResults_fit = fitted_func.GetPars(fx_min, fw_bins);
  }

  TF1 *GetBkgFunc() const {
    auto bg = new TF1(Form("bg_fit_%d", GenerateUID()),
                      Form("pol%d", fnOrderPoly), fx_min, fx_max);
    bg->SetParameters(fResults_fit.data());
    return bg;
  }

  void fitWithSignal() {
    MFitterVec raw_substracted = fYraws.throwBasicVec(fBasisVecs);
    MFitterVec signal_substracted = fYsignal.throwBasicVec(fBasisVecs);

    double norm_raw = std::sqrt(raw_substracted * raw_substracted);
    double norm_signal = std::sqrt(signal_substracted * signal_substracted);

    if (norm_raw == 0 || norm_signal == 0) {
      fNSignal = 0.0;
    } else {
      double cosTheta =
          (raw_substracted * signal_substracted) / (norm_raw * norm_signal);
      fNSignal = (cosTheta > 0) ? cosTheta * norm_raw / norm_signal : 0.0;
    }

    MFitterVec raw_minus_signal = fYraws - fYsignal * fNSignal;
    std::vector<double> coeffs_vec = Project(raw_minus_signal);
    std::vector<double> coeffs(coeffs_vec.begin(), coeffs_vec.end());

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

  double GetChi2OvNdf() const {
    auto bg = GetBkgFunc();
    double chi2 = 0.0;
    int ndf = fn_bins - (fnOrderPoly + 1) - 1; // -1 for fNSignal
    int bin_min = fHisto->GetXaxis()->FindBin(fx_min);

    for (int i = 0; i < fn_bins; ++i) {
      double y = fHisto->GetBinContent(bin_min + i);
      double err_y = fHisto->GetBinError(bin_min + i);
      double y_sig = fYsignal.fVec[i] * fNSignal;
      double x = fHisto->GetBinCenter(bin_min + i);
      double y_bg = bg->Eval(x);
      if (err_y > 0) {
        double resid = y - y_sig - y_bg;
        chi2 += resid * resid / (err_y * err_y);
      }
    }

    delete bg; // 👈 避免内存泄漏
    return (ndf > 0) ? chi2 / ndf : 0.0;
  }

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
    raw->SetTitle("");
    raw->GetYaxis()->SetTitle(Form("Entries / (%.2f GeV^{2}/c^{4})", fw_bins));
    double max_raw = raw->GetMaximum();
    raw->GetYaxis()->SetRangeUser(0, max_raw * 1.2);
    raw->GetXaxis()->SetRangeUser(fx_min_plot, fx_max_plot);
    raw->SetMarkerColor(kBlack);
    raw->SetMarkerStyle(20);

    gr_fitTotal->SetLineColor(kBlue);
    gr_fitTotal->SetLineWidth(2);

    bg->SetLineColor(kGreen + 1);
    bg->SetLineStyle(kDashed);
    bg->SetLineWidth(3);

    gr_signal->SetLineColor(kRed);
    gr_signal->SetMarkerStyle(0);
    gr_signal->SetLineStyle(kDashed);
    gr_signal->SetLineWidth(3);

    raw->DrawClone("E1");
    bg->DrawClone("same");
    gr_fitTotal->DrawClone("same C");
    gr_signal->DrawClone("same C");

    TLegend *legend = new TLegend(0.175, 0.75, 0.375, 0.89);
    legend->SetFillColor(0);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.035);
    legend->SetLineColor(0);
    legend->AddEntry(raw, "Data", "lep");
    legend->AddEntry(gr_signal, "Signal", "l");
    legend->AddEntry(bg, "Background", "l");
    legend->DrawClone("same");

    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.04);
    tex->DrawLatex(0.65, 0.8, Form("N_{sig} = %.2e", fNSignal));
    tex->DrawClone("same");

    // Clean up local objects if not added to canvas
    delete raw;
    delete signal;
    delete fit_total;
    delete gr_signal;
    delete gr_fitTotal;
    delete bg;
    delete legend;
    delete tex;
  }
};

// ==================================================================

class MTestFitter {
  MFitterPoly &fitter;
  TF1 *fFunc_sig;
  TH1D *fH1_mass;
  int fN_test = 30;
  std::vector<double> fVec_chi2ONdf;
  std::vector<TF1 *> fVec_func_sig;

public:
  MTestFitter(MFitterPoly &fitterObj, TH1D *hist, int n_test = 30)
      : fitter(fitterObj), fH1_mass(hist), fN_test(n_test) {

    if (!hist) {
      std::cerr << "MTestFitter: input histogram is null!" << std::endl;
      std::exit(1);
    }

    fFunc_sig = new TF1("fitted_sig",
                        "[z_height]*ROOT::Math::crystalball_function(x,[Alpha],"
                        "[N],[Sigma],[Mean])",
                        1.88, 4.32);
    fFunc_sig->SetParameters(0.234623, 3.06213, 5.12185, 0.0491487, 1.);
    fVec_chi2ONdf.reserve(fN_test);
    fitterObj.setHisto(fH1_mass);
  }

  ~MTestFitter() {
    delete fFunc_sig;
    for (auto *f : fVec_func_sig) {
      delete f;
    }
  }

  void performTestFitting(bool doDraw = false) {
    int index_max = 1000;
    for (int i = 0; i < fN_test; i++) {
      fitter.inputSignal(fFunc_sig, 1000000);
      fitter.fitWithSignal();

      if (doDraw) {
        gPublisherCanvas->NewPad()->cd();
        fitter.Draw();
      }

      TH1D *mass_copy =
          (TH1D *)fH1_mass->Clone(Form("mass_copy_%d", GenerateUID()));
      auto bkg = fitter.GetBkgFunc();

      for (int j = 1; j <= mass_copy->GetNbinsX(); ++j) {
        double x = mass_copy->GetXaxis()->GetBinCenter(j);
        double y_bkg = bkg->Eval(x);
        mass_copy->SetBinContent(j, mass_copy->GetBinContent(j) - y_bkg);
      }

      mass_copy->GetXaxis()->SetRangeUser(2.5, 4.0);

      TF1 *func2fit = nullptr;
      if (fVec_func_sig.empty()) {
        func2fit = (TF1 *)fFunc_sig->Clone(Form("init_sig_%d", GenerateUID()));
      } else {
        func2fit = (TF1 *)fVec_func_sig.back()->Clone(
            Form("sig_clone_%d", GenerateUID()));
      }

      func2fit->SetParameter("z_height", mass_copy->Integral("width"));
      if (doDraw)
        gPublisherCanvas->NewPad()->cd();
      // mass_copy->Fit(func2fit, "RQ", "", 2.1, 4.0); never plot it
      mass_copy->Fit(func2fit, "RQN", "", 2.1, 4.0);

      if (doDraw) {
        mass_copy->DrawClone("");
      }

      double chi2OvNdf = fitter.GetChi2OvNdf();
      fVec_func_sig.push_back((TF1 *)func2fit->Clone()); // take ownership
      fVec_chi2ONdf.push_back(chi2OvNdf);

      delete mass_copy;
      delete bkg;

      index_max--;
      if (index_max <= 0) {
        std::cerr << "MTestFitter: max iterations reached." << std::endl;
        break;
      }
    }
  }

  const std::vector<double> &getChi2OverNdf() const { return fVec_chi2ONdf; }
  TF1 *getSignalFunction() const { return fFunc_sig; }

  void setSignalParameters(double height, double alpha, double n, double sigma,
                           double mean) {
    fFunc_sig->SetParameters(height, alpha, n, sigma, mean);
  }

  void setTestCount(int n_test) {
    fN_test = n_test;
    fVec_chi2ONdf.reserve(n_test);
  }

  int getTestCount() const { return fN_test; }
  std::vector<TF1 *> getAllSignalFunctions() const { return fVec_func_sig; }
};

// Inverse-variance counterpart to MFitterPoly.  Kept separate so the
// unit-weight production fitter above remains unchanged.
class MFitterPolyInvSigma2 {
  TH1D* fHisto = nullptr;
  int fFirstBin = 0, fNBins = 0, fOrder = 2;
  double fXMin = 0., fBinWidth = 0.;
  bool fHasSufficientPoints = false;
  std::vector<double> fWeights, fSignal;
  std::vector<std::vector<double>> fBasis;

  double dot(const std::vector<double>& a, const std::vector<double>& b) const {
    double sum = 0.;
    for (int i = 0; i < fNBins; ++i) sum += fWeights[i] * a[i] * b[i];
    return sum;
  }
  std::vector<double> projectOut(const std::vector<double>& values) const {
    auto residual = values;
    for (const auto& basis : fBasis) {
      const double coefficient = dot(basis, values);
      for (int i = 0; i < fNBins; ++i) residual[i] -= coefficient * basis[i];
    }
    return residual;
  }
  void buildBasis() {
    fHasSufficientPoints = false;
    fWeights.resize(fNBins);
    int nWeighted = 0;
    for (int i = 0; i < fNBins; ++i) {
      const double error = fHisto->GetBinError(fFirstBin + i);
      fWeights[i] = error > 0. ? 1. / (error * error) : 0.;
      if (fWeights[i] > 0.) ++nWeighted;
    }
    if (nWeighted <= fOrder + 1) {
      std::cerr << "MFitterPolyInvSigma2: insufficient nonzero-error bins ("
                << nWeighted << " available; need more than " << fOrder + 1
                << "); setting signal yield to zero" << std::endl;
      fBasis.clear();
      fNSignal = 0.;
      return;
    }
    fBasis.clear();
    for (int power = 0; power <= fOrder; ++power) {
      std::vector<double> current(fNBins);
      for (int i = 0; i < fNBins; ++i) current[i] = std::pow(i + 1, power);
      for (const auto& previous : fBasis) {
        const double coefficient = dot(current, previous);
        for (int i = 0; i < fNBins; ++i) current[i] -= coefficient * previous[i];
      }
      const double norm = std::sqrt(dot(current, current));
      if (norm == 0.) {
        std::cerr << "MFitterPolyInvSigma2: singular basis; setting signal yield to zero"
                  << std::endl;
        fBasis.clear();
        fNSignal = 0.;
        return;
      }
      for (double& value : current) value /= norm;
      fBasis.push_back(current);
    }
    fHasSufficientPoints = true;
  }
 public:
  double fNSignal = 0.;
  MFitterPolyInvSigma2(TH1D* histogram, double xMin, double xMax) {
    if (!histogram) { std::cerr << "MFitterPolyInvSigma2: null histogram" << std::endl; std::exit(1); }
    fHisto = histogram;
    fBinWidth = histogram->GetXaxis()->GetBinWidth(1);
    fFirstBin = histogram->GetXaxis()->FindBin(xMin);
    fXMin = histogram->GetXaxis()->GetBinLowEdge(fFirstBin);
    fNBins = histogram->GetXaxis()->FindBin(xMax) - fFirstBin + 1;
  }
  void initializeBasis(int order = 4) { fOrder = order; buildBasis(); }
  void setHisto(TH1D* histogram) {
    if (!histogram || std::abs(histogram->GetXaxis()->GetBinWidth(1) - fBinWidth) > 1e-6) {
      std::cerr << "MFitterPolyInvSigma2: incompatible histogram" << std::endl; std::exit(1);
    }
    const int firstBin = histogram->GetXaxis()->FindBin(fXMin);
    if (std::abs(histogram->GetXaxis()->GetBinLowEdge(firstBin) - fXMin) > 1e-6) {
      std::cerr << "MFitterPolyInvSigma2: incompatible mass range" << std::endl; std::exit(1);
    }
    fHisto = histogram; fFirstBin = firstBin; buildBasis();
  }
  void inputSignal(TH1D* signal, int nSampling = 10000000) {
    if (!signal) { std::cerr << "MFitterPolyInvSigma2: null signal template" << std::endl; std::exit(1); }
    TH1D sampled(Form("template_signal_%d", GenerateUID()), "Signal Template", fNBins, fXMin,
                 fXMin + fNBins * fBinWidth);
    for (int i = 0; i < nSampling; ++i) sampled.Fill(signal->GetRandom());
    sampled.Scale(1. / nSampling);
    fSignal.resize(fNBins);
    for (int i = 0; i < fNBins; ++i) fSignal[i] = sampled.GetBinContent(i + 1);
  }
  void inputSignal(TF1* signal, int nSampling = 10000000) {
    if (!signal) { std::cerr << "MFitterPolyInvSigma2: null signal function" << std::endl; std::exit(1); }
    TH1D sampled(Form("template_signal_%d", GenerateUID()), "Signal Template", fNBins, fXMin,
                 fXMin + fNBins * fBinWidth);
    for (int i = 0; i < nSampling; ++i) sampled.Fill(signal->GetRandom());
    sampled.Scale(1. / nSampling);
    fSignal.resize(fNBins);
    for (int i = 0; i < fNBins; ++i) fSignal[i] = sampled.GetBinContent(i + 1);
  }
  void fitWithSignal() {
    if (!fHasSufficientPoints) {
      fNSignal = 0.;
      return;
    }
    std::vector<double> raw(fNBins);
    for (int i = 0; i < fNBins; ++i) raw[i] = fHisto->GetBinContent(fFirstBin + i);
    const auto rawPerp = projectOut(raw), signalPerp = projectOut(fSignal);
    const double denominator = dot(signalPerp, signalPerp);
    fNSignal = denominator > 0. ? dot(rawPerp, signalPerp) / denominator : 0.;
    if (fNSignal < 0.) fNSignal = 0.;
  }
};
#endif // __MFit_h__
