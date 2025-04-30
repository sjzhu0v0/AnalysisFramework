#ifndef MHelper_h
#define MHelper_h

#include "MHist.h"

class AssocYeildHelper_v1 {
private:
  vector<int> IdRedundancy(vector<int> ids1, vector<int> ids2) {
    // get the redundancy index of ids2 different from ids1
    vector<int> ids;
    for (int i = 0; i < ids2.size(); i++) {
      bool found = false;
      for (int j = 0; j < ids1.size(); j++) {
        if (ids2[i] == ids1[j]) {
          found = true;
          break;
        }
      }
      if (!found) {
        ids.push_back(ids2[i]);
      }
    }
    return ids;
  }

public:
  MHnTool *fHnSame;
  MHnTool *fHnMix;
  MHnTool *fHnTrigger;

  AssocYeildHelper_v1(MHnTool *hnSame, MHnTool *hnMix, MHnTool *hnTrigger) {
    fHnSame = hnSame;
    fHnMix = hnMix;
    fHnTrigger = hnTrigger;
  }

  AssocYeildHelper_v1(THnD *hnSame, THnD *hnMix, THnD *hnTrigger) {
    fHnSame = new MHnTool(hnSame);
    fHnMix = new MHnTool(hnMix);
    fHnTrigger = new MHnTool(hnTrigger);
  }

  ~AssocYeildHelper_v1() {
    delete fHnMix;
    delete fHnSame;
    delete fHnTrigger;
  }

  void Rebin(int dimTarget, int n) {
    switch (dimTarget) {

    case kDeltaEta:
      fHnSame->Rebin(dimTarget, n);
      fHnMix->Rebin(dimTarget, n);
      break;
    case kDeltaPhi:
      fHnSame->Rebin(dimTarget, n);
      fHnMix->Rebin(dimTarget, n);
      break;
    case kVtxZ:
      fHnSame->Rebin(dimTarget, n);
      fHnMix->Rebin(dimTarget, n);
      fHnTrigger->Rebin(0, n);
      break;
    case kMass:
      fHnSame->Rebin(dimTarget, n);
      fHnMix->Rebin(dimTarget, n);
      fHnTrigger->Rebin(1, n);
      break;
    case kPt:
      fHnSame->Rebin(dimTarget, n);
      fHnMix->Rebin(dimTarget, n);
      fHnTrigger->Rebin(2, n);
      break;
    case kNumContrib:
      fHnSame->Rebin(dimTarget, n);
      fHnTrigger->Rebin(3, n);
      break;
    default:
      cerr << "Error: AssocYeildHelper_v1::Rebin: dimTarget is out of range"
           << endl;
      exit(1);
    }
  }

  int GetNBins(int dimTarget) {
    return fHnSame->hN->GetAxis(dimTarget)->GetNbins();
  }

  TH2D *AssociatedYeildVtxZSum(int iVtxZ, int iMass, int iPt, int iMult) {
    TH2D *h2D = fHnSame->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                 {iVtxZ, iMass, iPt, iMult});
    TH2D *h2DMix = fHnMix->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                   {iVtxZ, iMass, iPt});
    vector<int> vec_idTrigger = {iVtxZ, iMass, iPt, iMult};
    double number_triggered =
        fHnTrigger->hN->GetBinContent(vec_idTrigger.data());
    TH2D *h_assoYeild = (TH2D *)h2D->Clone(
        Form("h_assoYeild_%d_%d_%d_%d", iVtxZ, iMass, iPt, iMult));
    DensityHisto2DNoWeight(h2DMix);
    h_assoYeild->Divide(h2DMix);
    h2DMix->Delete();
    h2D->Delete();
    return h_assoYeild;
  }

  TH1D *AssociatedYeildVtxZSum(double deltaEta, int iVtxZ, int iMass, int iPt,
                               int iMult) {
    TH2D *h2D = fHnSame->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                 {iVtxZ, iMass, iPt, iMult});
    TH2D *h2DMix = fHnMix->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                   {iVtxZ, iMass, iPt});
    DensityHisto2DNoWeight(h2DMix);
    vector<int> vec_idTrigger = {iVtxZ, iMass, iPt, iMult};
    double number_triggered =
        fHnTrigger->hN->GetBinContent(vec_idTrigger.data());

    int index_bin_deltaEta = h2D->GetXaxis()->FindBin(-1. * deltaEta) - 1;
    TH1D *h1_same = h2D->ProjectionY(Form("h1_same_%d", GenerateUID()), 1,
                                     index_bin_deltaEta);
    TH1D *h1_same2 = h2D->ProjectionY(Form("h1_same_%d", GenerateUID()),
                                      h2D->GetNbinsY() - index_bin_deltaEta + 1,
                                      h2D->GetNbinsY());
    h1_same->Add(h1_same2);
    TH1D *h1_mix = h2DMix->ProjectionY(Form("h1_mix_%d", GenerateUID()), 1,
                                       index_bin_deltaEta);
    TH1D *h1_mix2 = h2DMix->ProjectionY(
        Form("h1_mix_%d", GenerateUID()),
        h2DMix->GetNbinsY() - index_bin_deltaEta + 1, h2DMix->GetNbinsY());
    h1_mix->Add(h1_mix2);
    TH1D *h1_results = (TH1D *)h1_same->Clone(
        Form("h1_asso_%d_%d_%d_%d", iVtxZ, iMass, iPt, iMult));
    HistDivide1D(h1_results, h1_same, h1_mix);
    h2D->Delete();
    h2DMix->Delete();
    h1_same->Delete();
    h1_same2->Delete();
    h1_mix->Delete();
    h1_mix2->Delete();
    return h1_results;
  }

  TH2D *AssociatedYeildVtxZ(int iVtxZ, int iMass, int iPt, int iMult) {
    TH2D *h2D = fHnSame->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                 {iVtxZ, iMass, iPt, iMult});
    TH2D *h2DMix = fHnMix->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                   {iVtxZ, iMass, iPt});
    vector<int> vec_idTrigger = {iVtxZ, iMass, iPt, iMult};
    double number_triggered =
        fHnTrigger->hN->GetBinContent(vec_idTrigger.data());
    TH2D *h_assoYeild = (TH2D *)h2D->Clone(
        Form("h_assoYeild_%d_%d_%d_%d", iVtxZ, iMass, iPt, iMult));
    DensityHisto2DNoWeight(h2DMix);
    h_assoYeild->Divide(h2DMix);
    h_assoYeild->Scale(1.0 / number_triggered);
    h2DMix->Delete();
    h2D->Delete();
    return h_assoYeild;
  }

  TH2D *AssociatedYeild(int iMass, int iPt, int iMult) {
    int nVtxZ = glib_vars[gtype_vars::kVtxZ].fNbins;
    TH2D *h2_first = AssociatedYeildVtxZSum(1, iMass, iPt, iMult);
    TH1D *h1_trigger = fHnTrigger->Project(0, {iMass, iPt, iMult});
    h1_trigger->SetName(Form("h1_trigger_%d", GenerateUID()));
    double sum_number_triggered = h1_trigger->GetBinContent(1);
    for (int i = 2; i <= nVtxZ; i++) {
      double number_triggered = h1_trigger->GetBinContent(i);
      sum_number_triggered += number_triggered;
    }
    for (int i = 2; i <= nVtxZ; i++) {
      TH2D *h2D = AssociatedYeildVtxZSum(i, iMass, iPt, iMult);
      h2_first->Add(h2D);
      h2D->Delete();
    }
    if (sum_number_triggered == 0) {
      return h2_first;
    }
    ScaleHisto2D(h2_first, 1.0 / sum_number_triggered);
    h1_trigger->Delete();
    return h2_first;
  }

  TH1D *AssociatedYeild(double deltaEta, int iMass, int iPt, int iMult) {
    int nVtxZ = glib_vars[gtype_vars::kVtxZ].fNbins;
    TH1D *h1_first = AssociatedYeildVtxZSum(deltaEta, 1, iMass, iPt, iMult);
    TH1D *h1_trigger = fHnTrigger->Project(0, {iMass, iPt, iMult});
    h1_trigger->SetName(Form("h1_trigger_%d", GenerateUID()));
    double sum_number_triggered = h1_trigger->GetBinContent(1);
    for (int i = 2; i <= nVtxZ; i++) {
      double number_triggered = h1_trigger->GetBinContent(i);
      sum_number_triggered += number_triggered;
    }
    for (int i = 2; i <= nVtxZ; i++) {
      TH1D *h1D = AssociatedYeildVtxZSum(deltaEta, i, iMass, iPt, iMult);
      h1_first->Add(h1D);
      h1D->Delete();
    }
    if (sum_number_triggered == 0) {
      return h1_first;
    }
    ScaleHisto1D(h1_first, 1.0 / sum_number_triggered);
    h1_trigger->Delete();
    return h1_first;
  }
};

class AssocYeildSub_v1 : public MHGroupTool1D {
private:
  int fBinLowMult = 0;
  int fBinHighMult = 0;

public:
  AssocYeildSub_v1(MHGroupTool1D *hgroupTool1d)
      : MHGroupTool1D(hgroupTool1d) {};
  ~AssocYeildSub_v1() {};

  void Rebin(int n) {
    for (int i = 0; i < fHistos.size(); i++) {
      fHistos[i]->Rebin(n);
    }
  }

  void SetMultBin(int low, int high) {
    if (low < 0 || high < 0) {
      cerr << "Error: AssocYeildSub_v1::SetMultBin: low or high is negative"
           << endl;
      exit(1);
    } else if (low > high) {
      cerr << "Error: AssocYeildSub_v1::SetMultBin: low is greater than high"
           << endl;
      exit(1);
    } else if (low > fNbin_Var[2] || high > fNbin_Var[2]) {
      cerr << "Error: AssocYeildSub_v1::SetMultBin: low or high is out of range"
           << endl;
      exit(1);
    }
    fBinLowMult = low;
    fBinHighMult = high;
  }

  double GetBinContent(int phi, int mass, int pt, int mult) {
    TH1D *h1 = fHistos[GetBinIndex({mass, pt, mult})];
    if (h1 == nullptr) {
      cerr << "Error: AssocYeildSub_v1::GetBinContent: h1 is null" << endl;
      exit(1);
    }
    return h1->GetBinContent(phi);
  }

  double GetBinError(int phi, int mass, int pt, int mult) {
    TH1D *h1 = fHistos[GetBinIndex({mass, pt, mult})];
    if (h1 == nullptr) {
      cerr << "Error: AssocYeildSub_v1::GetBinError: h1 is null" << endl;
      exit(1);
    }
    return h1->GetBinError(phi);
  }

  double GetBinContentMultSub(int phi, int mass, int pt) {
    if (fBinLowMult == fBinHighMult) {
      cerr << "Error: AssocYeildSub_v1::GetBinContentMultSub: "
              "fBinLowMult or fBinHighMult is not set"
           << endl;
      exit(1);
    }
    return GetBinContent(phi, mass, pt, fBinHighMult) -
           GetBinContent(phi, mass, pt, fBinLowMult);
  }

  double GetBinErrorMultSub(int phi, int mass, int pt) {
    if (fBinLowMult == fBinHighMult) {
      cerr << "Error: AssocYeildSub_v1::GetBinErrorMultSub: "
              "fBinLowMult or fBinHighMult is not set"
           << endl;
      exit(1);
    }
    return TMath::Sqrt(GetBinError(phi, mass, pt, fBinHighMult) *
                           GetBinError(phi, mass, pt, fBinHighMult) +
                       GetBinError(phi, mass, pt, fBinLowMult) *
                           GetBinError(phi, mass, pt, fBinLowMult));
  }

  TH1D *GetAssoYeild_Mass(int phi, int pt) {
    StrVar4Hist strMass = fStrsVar4Hist[0];
    TH1D *h1_assoYeild_mass =
        new TH1D(Form("h1_%d", GenerateUID()), strMass.CompleteTitle(),
                 strMass.fNbins, strMass.fBins.data());

    for (int imass = 1; imass <= strMass.fNbins; imass++) {
      h1_assoYeild_mass->SetBinContent(imass,
                                       GetBinContentMultSub(phi, imass, pt));
      h1_assoYeild_mass->SetBinError(imass, GetBinErrorMultSub(phi, imass, pt));
    }
    return h1_assoYeild_mass;
  }
};
#endif