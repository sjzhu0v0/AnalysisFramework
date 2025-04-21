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

    int index_bin_deltaEta = h2D->GetXaxis()->FindBin(deltaEta / 2.);
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
    HistDivide2D(h1_results, h1_same, h1_mix);
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

#endif