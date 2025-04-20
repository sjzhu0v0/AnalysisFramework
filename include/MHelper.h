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
    // vector<int> vec_idredunMix =
    //     IdRedundancy(fHnSame->fVec_posVar, fHnMix->fVec_posVar);
    // vector<int> vec_idredunTrigger =
    //     IdRedundancy(fHnSame->fVec_posVar, fHnTrigger->fVec_posVar);

    // for (int i : vec_idredunMix)
    //   fHnMix->RebinTotal(i);
    // for (int i : vec_idredunTrigger)
    //   fHnTrigger->RebinTotal(i);
  }

  ~AssocYeildHelper_v1() {
    delete fHnMix;
    delete fHnSame;
    delete fHnTrigger;
  }

  TH2D *AssociatedYeildVtxZSum(int iVtxZ, int iMass, int iPt, int iMult) {
    TH2D *h2D = fHnSame->Project(gtype_vars::kDeltaEta, gtype_vars::kDeltaPhi,
                                 {iVtxZ, iMass, iPt, iMult});
    TH2D *h2DMix = fHnMix->Project(gtype_vars::kDeltaEta, gtype_vars::kDeltaPhi,
                                   {iVtxZ, iMass, iPt});
    vector<int> vec_idTrigger = {iVtxZ, iMass, iPt, iMult};
    double number_triggered =
        fHnTrigger->hN->GetBinContent(vec_idTrigger.data());
    TH2D *h_assoYeild = (TH2D *)h2D->Clone(
        Form("h_assoYeild_%d_%d_%d_%d", iVtxZ, iMass, iPt, iMult));
    DensityHisto2DNoWeight(h2DMix);
    h_assoYeild->Divide(h2DMix);
    return h_assoYeild;
  }

  TH2D *AssociatedYeildVtxZ(int iVtxZ, int iMass, int iPt, int iMult) {
    TH2D *h2D = fHnSame->Project(gtype_vars::kDeltaEta, gtype_vars::kDeltaPhi,
                                 {iVtxZ, iMass, iPt, iMult});
    TH2D *h2DMix = fHnMix->Project(gtype_vars::kDeltaEta, gtype_vars::kDeltaPhi,
                                   {iVtxZ, iMass, iPt});
    vector<int> vec_idTrigger = {iVtxZ, iMass, iPt, iMult};
    double number_triggered =
        fHnTrigger->hN->GetBinContent(vec_idTrigger.data());
    TH2D *h_assoYeild = (TH2D *)h2D->Clone(
        Form("h_assoYeild_%d_%d_%d_%d", iVtxZ, iMass, iPt, iMult));
    DensityHisto2DNoWeight(h2DMix);
    h_assoYeild->Divide(h2DMix);
    h_assoYeild->Scale(1.0 / number_triggered);
    return h_assoYeild;
  }

  TH2D *AssociatedYeild(int iMass, int iPt, int iMult) {
    int nVtxZ = glib_vars[gtype_vars::kVtxZ].fNbins;
    TH2D *h2_first = AssociatedYeildVtxZ(0, iMass, iPt, iMult);
    double sum_number_triggered = 0;
    for (int i = 0; i < nVtxZ; i++) {
      vector<int> vec_idTrigger = {i, iMass, iPt, iMult};
      double number_triggered =
          fHnTrigger->hN->GetBinContent(vec_idTrigger.data());
      sum_number_triggered += number_triggered;
    }
    for (int i = 1; i < nVtxZ; i++) {
      TH2D *h2D = AssociatedYeildVtxZSum(i, iMass, iPt, iMult);
      h2_first->Add(h2D);
    }
    h2_first->Scale(1.0 / sum_number_triggered);
    return h2_first;
  }
};

#endif