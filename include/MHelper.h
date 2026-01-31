#ifndef MHelper_h
#define MHelper_h

#include "MHist.h"

class AssocYieldHelper_v1 {
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

  AssocYieldHelper_v1(MHnTool *hnSame, MHnTool *hnMix, MHnTool *hnTrigger) {
    fHnSame = hnSame;
    fHnMix = hnMix;
    fHnTrigger = hnTrigger;
  }

  AssocYieldHelper_v1(THnD *hnSame, THnD *hnMix, THnD *hnTrigger) {
    fHnSame = new MHnTool(hnSame);
    fHnMix = new MHnTool(hnMix);
    fHnTrigger = new MHnTool(hnTrigger);
  }

  ~AssocYieldHelper_v1() {
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
      cerr << "Error: AssocYieldHelper_v1::Rebin: dimTarget is out of range"
           << endl;
      exit(1);
    }
  }

  int GetNBins(int dimTarget) {
    return fHnSame->hN->GetAxis(dimTarget)->GetNbins();
  }

  TH2D *AssociatedYieldVtxZSum(int iVtxZ, int iMass, int iPt, int iMult) {
    TH2D *h2D = fHnSame->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                 {iVtxZ, iMass, iPt, iMult});
    TH2D *h2DMix = fHnMix->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                   {iVtxZ, iMass, iPt});
    // int iMass_new = fHnTrigger->hN->GetAxis(1)->FindBin(
    //     fHnMix->hN->GetAxis(3)->GetBinCenter(iMass));
    vector<int> vec_idTrigger_new = {iVtxZ, iMass, iPt, iMult};
    double number_triggered =
        fHnTrigger->hN->GetBinContent(vec_idTrigger_new.data());
    TH2D *h_assoYield = (TH2D *)h2D->Clone(
        Form("h_assoYield_%d_%d_%d_%d", iVtxZ, iMass, iPt, iMult));
    if (h2DMix->Integral() > 0) {
      AccCorrHisto2DNoWeight(h2DMix);
      h_assoYield->Divide(h2DMix);
    }
    h2DMix->Delete();
    h2D->Delete();
    return h_assoYield;
  }

  TH1D *AssociatedYieldVtxZSum(double deltaEta, int iVtxZ, int iMass, int iPt,
                               int iMult) {
    TH2D *h2D = fHnSame->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                 {iVtxZ, iMass, iPt, iMult});
    TH2D *h2DMix = fHnMix->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                   {iVtxZ, iMass, iPt});
    DensityHisto2DNoWeight(h2DMix);
    // int iMass_new = fHnTrigger->hN->GetAxis(1)->FindBin(
    //     fHnMix->hN->GetAxis(3)->GetBinCenter(iMass));
    vector<int> vec_idTrigger_new = {iVtxZ, iMass, iPt, iMult};
    double number_triggered =
        fHnTrigger->hN->GetBinContent(vec_idTrigger_new.data());

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

  TH2D *AssociatedYieldVtxZ(int iVtxZ, int iMass, int iPt, int iMult) {
    TH2D *h2D = fHnSame->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                 {iVtxZ, iMass, iPt, iMult});
    TH2D *h2DMix = fHnMix->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                   {iVtxZ, iMass, iPt});
    // int iMass_new = fHnTrigger->hN->GetAxis(1)->FindBin(
    //     fHnMix->hN->GetAxis(3)->GetBinCenter(iMass));
    vector<int> vec_idTrigger_new = {iVtxZ, iMass, iPt, iMult};
    double number_triggered =
        fHnTrigger->hN->GetBinContent(vec_idTrigger_new.data());
    TH2D *h_assoYield = (TH2D *)h2D->Clone(
        Form("h_assoYield_%d_%d_%d_%d", iVtxZ, iMass, iPt, iMult));
    if (h2DMix->Integral() > 0) {
      AccCorrHisto2DNoWeight(h2DMix);
      h_assoYield->Divide(h2DMix);
    }
    h_assoYield->Scale(1.0 / number_triggered);
    h2DMix->Delete();
    h2D->Delete();
    return h_assoYield;
  }

  TH2D *AssociatedYield(int iMass, int iPt, int iMult) {
    int nVtxZ = glib_vars[gtype_vars::kVtxZ].fNbins;
    TH2D *h2_first = AssociatedYieldVtxZSum(1, iMass, iPt, iMult);
    TH1D *h1_trigger = fHnTrigger->Project(0, {iMass, iPt, iMult});
    h1_trigger->SetName(Form("h1_trigger_%d", GenerateUID()));
    double sum_number_triggered = h1_trigger->GetBinContent(1);
    for (int i = 2; i <= nVtxZ; i++) {
      double number_triggered = h1_trigger->GetBinContent(
          h1_trigger->FindBin(h1_trigger->GetXaxis()->GetBinCenter(i)));
      sum_number_triggered += number_triggered;
    }
    for (int i = 2; i <= nVtxZ; i++) {
      TH2D *h2D = AssociatedYieldVtxZSum(i, iMass, iPt, iMult);
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

  TH1D *AssociatedYield(double deltaEta, int iMass, int iPt, int iMult) {
    int nVtxZ = glib_vars[gtype_vars::kVtxZ].fNbins;
    TH1D *h1_first = AssociatedYieldVtxZSum(deltaEta, 1, iMass, iPt, iMult);
    TH1D *h1_trigger = fHnTrigger->Project(0, {iMass, iPt, iMult});
    h1_trigger->SetName(Form("h1_trigger_%d", GenerateUID()));
    double sum_number_triggered = h1_trigger->GetBinContent(1);
    for (int i = 2; i <= nVtxZ; i++) {
      double number_triggered = h1_trigger->GetBinContent(
          h1_trigger->FindBin(h1_trigger->GetXaxis()->GetBinCenter(i)));
      sum_number_triggered += number_triggered;
    }
    for (int i = 2; i <= nVtxZ; i++) {
      TH1D *h1D = AssociatedYieldVtxZSum(deltaEta, i, iMass, iPt, iMult);
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

class AssocYieldHelper_v2 {
private:
public:
  MHnTool *fHnSame;
  MHnTool *fHnMix;
  MHnTool *fHnTrigger;
  bool doMixMultInt = false;

  AssocYieldHelper_v2(MHnTool *hnSame, MHnTool *hnMix, MHnTool *hnTrigger) {
    fHnSame = hnSame;
    fHnMix = hnMix;
    fHnTrigger = hnTrigger;
  }

  void SetMixMultInt(bool doMixMultInt_ = true) {
    doMixMultInt = doMixMultInt_;
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
      fHnMix->Rebin(dimTarget, n);
      fHnTrigger->Rebin(3, n);
      break;
    default:
      cerr << "Error: AssocYieldHelper_v1::Rebin: dimTarget is out of range"
           << endl;
      exit(1);
    }
  }

  void SetRangeUser(int dim, double min, double max) {
    if (dim < 0 || dim >= fHnSame->hN->GetNdimensions()) {
      cerr << "Error: AssocYieldHelper_v2::SetRangeUser: dim is out of range"
           << endl;
      exit(1);
    }
    switch (dim) {
    case kDeltaEta:
      fHnSame->SetRangeUser(dim, min, max);
      fHnMix->SetRangeUser(dim, min, max);
      break;
    case kDeltaPhi:
      fHnSame->SetRangeUser(dim, min, max);
      fHnMix->SetRangeUser(dim, min, max);
      break;
    case kVtxZ:
      fHnSame->SetRangeUser(dim, min, max);
      fHnMix->SetRangeUser(dim, min, max);
      fHnTrigger->SetRangeUser(0, min, max);
      break;
    case kMass:
      fHnSame->SetRangeUser(dim, min, max);
      fHnMix->SetRangeUser(dim, min, max);
      fHnTrigger->SetRangeUser(1, min, max);
      break;
    case kPt:
      fHnSame->SetRangeUser(dim, min, max);
      fHnMix->SetRangeUser(dim, min, max);
      fHnTrigger->SetRangeUser(2, min, max);
      break;
    case kNumContrib:
      fHnSame->SetRangeUser(dim, min, max);
      fHnMix->SetRangeUser(dim, min, max);
      fHnTrigger->SetRangeUser(3, min, max);
      break;
    default:
      cerr << "Error: AssocYieldHelper_v2::SetRangeUser: dim is out of range"
           << endl;
      exit(1);
    }
  }

  TH2D *AssociatedYieldVtxZ(int iVtxZ, int iMass, int iPt, int iMult,
                            bool doNTrigScale = true) {
    TH2D *h2D = fHnSame->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                 {iVtxZ, iMass, iPt, iMult});
    TH2D *h2DMix =
        fHnMix->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                        {iVtxZ, iMass, iPt, doMixMultInt ? 0 : iMult});
    // int iMass_new;

    // if (iMass != 0)
    //   iMass_new = fHnTrigger->hN->GetAxis(1)->FindBin(
    //       fHnMix->hN->GetAxis(3)->GetBinCenter(iMass));
    // else
    //   iMass_new = 0; // default to 1 if iMass is 0

    vector<int> vec_idTrigger_new = {iVtxZ, iMass, iPt, iMult};
    double number_triggered = fHnTrigger->GetBinContent(vec_idTrigger_new);
    TH2D *h_assoYield = (TH2D *)h2D->Clone(
        Form("h_assoYield_%d_%d_%d_%d", iVtxZ, iMass, iPt, iMult));
    if (h2DMix->Integral() > 0) {
      AccCorrHisto2DNoWeight(h2DMix);
      h_assoYield->Divide(h2DMix);
    }
    // h_assoYield->Scale(1.0 / number_triggered);
    if (doNTrigScale) {
      if (number_triggered != 0) {
        h_assoYield->Scale(1.0 / number_triggered);
      } else {
        cerr << "AssocYieldHelper_v2:AssociatedYieldVtxZ: Error: "
                "number_triggered is zero, not scaling the histogram"
             << endl;
        exit(1);
      }
    }

    h2DMix->Delete();
    h2D->Delete();
    return h_assoYield;
  }

  TH2D *AssociatedYieldVtxZPtSum(int iVtxZ, int iMass, vector<int> vec_iPt,
                                 int iMult, bool doNTrigScale = true) {
    TH2D *h2D = fHnSame->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                 {iVtxZ, iMass, vec_iPt[0], iMult});
    TH2D *h2DMix =
        fHnMix->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                        {iVtxZ, iMass, vec_iPt[0], doMixMultInt ? 0 : iMult});

    vector<int> vec_idTrigger_new = {iVtxZ, iMass, vec_iPt[0], iMult};
    double number_triggered = fHnTrigger->GetBinContent(vec_idTrigger_new);

    for (int i = 1; i < vec_iPt.size(); i++) {
      TH2D *h2D_temp =
          fHnSame->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                           {iVtxZ, iMass, vec_iPt[i], iMult});
      TH2D *h2DMix_temp =
          fHnMix->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                          {iVtxZ, iMass, vec_iPt[i], doMixMultInt ? 0 : iMult});
      h2D->Add(h2D_temp);
      h2DMix->Add(h2DMix_temp);
      h2D_temp->Delete();
      h2DMix_temp->Delete();
      vector<int> vec_idTrigger_temp = {iVtxZ, iMass, vec_iPt[i], iMult};
      double number_triggered_temp =
          fHnTrigger->GetBinContent(vec_idTrigger_temp);
      number_triggered += number_triggered_temp;
    }

    TH2D *h_assoYield = (TH2D *)h2D->Clone(
        Form("h_assoYield_%d_%d_%d_%d", iVtxZ, iMass, vec_iPt[0], iMult));
    if (h2DMix->Integral() > 0) {
      AccCorrHisto2DNoWeight(h2DMix);
      h_assoYield->Divide(h2DMix);
    }

    // h_assoYield->Scale(1.0 / number_triggered);
    if (doNTrigScale) {
      if (number_triggered != 0) {
        h_assoYield->Scale(1.0 / number_triggered);
      } else {
        cerr << "AssocYieldHelper_v2:AssociatedYieldVtxZ: Error: "
                "number_triggered is zero, not scaling the histogram"
             << endl;
        exit(1);
      }
    }

    h2DMix->Delete();
    h2D->Delete();
    return h_assoYield;
  }

  TH2D *AssociatedYieldVtxZPtSum(int iVtxZ, int iMass, vector<int> vec_iPt,
                                 vector<int> vec_iMult,
                                 bool doNTrigScale = true) {
    TH2D *h2D = fHnSame->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                                 {iVtxZ, iMass, vec_iPt[0], vec_iMult[0]});
    TH2D *h2DMix = fHnMix->Project(
        gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
        {iVtxZ, iMass, vec_iPt[0], doMixMultInt ? 0 : vec_iMult[0]});

    vector<int> vec_idTrigger_new = {iVtxZ, iMass, vec_iPt[0], vec_iMult[0]};
    double number_triggered = fHnTrigger->GetBinContent(vec_idTrigger_new);
    for (int j = 0; j < vec_iMult.size(); j++)
      for (int i = 0; i < vec_iPt.size(); i++) {
        if (i == 0 && j == 0)
          continue;
        TH2D *h2D_temp =
            fHnSame->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                             {iVtxZ, iMass, vec_iPt[i], vec_iMult[j]});
        h2D->Add(h2D_temp);
        h2D_temp->Delete();
        vector<int> vec_idTrigger_temp = {iVtxZ, iMass, vec_iPt[i],
                                          vec_iMult[j]};
        double number_triggered_temp =
            fHnTrigger->GetBinContent(vec_idTrigger_temp);
        number_triggered += number_triggered_temp;
      }
    if (!doMixMultInt)
      for (int j = 0; j < vec_iMult.size(); j++)
        for (int i = 0; i < vec_iPt.size(); i++) {
          if (i == 0 && j == 0)
            continue;
          TH2D *h2DMix_temp = fHnMix->Project(
              gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
              {iVtxZ, iMass, vec_iPt[i], doMixMultInt ? 0 : vec_iMult[j]});
          h2DMix->Add(h2DMix_temp);
          h2DMix_temp->Delete();
        }
    else {
      for (int i = 0; i < vec_iPt.size(); i++) {
        if (i == 0)
          continue;
        TH2D *h2DMix_temp =
            fHnMix->Project(gtype_vars::kDeltaPhi, gtype_vars::kDeltaEta,
                            {iVtxZ, iMass, vec_iPt[i], 0});
        h2DMix->Add(h2DMix_temp);
        h2DMix_temp->Delete();
      }
    }

    TH2D *h_assoYield = (TH2D *)h2D->Clone(Form(
        "h_assoYield_%d_%d_%d_%d", iVtxZ, iMass, vec_iPt[0], vec_iMult[0]));
    if (h2DMix->Integral() > 0) {
      AccCorrHisto2DNoWeight(h2DMix);
      h_assoYield->Divide(h2DMix);
    }
    // h_assoYield->Scale(1.0 / number_triggered);
    if (doNTrigScale) {
      if (number_triggered != 0) {
        h_assoYield->Scale(1.0 / number_triggered);
      } else {
        cerr << "AssocYieldHelper_v2:AssociatedYieldVtxZ: Error: "
                "number_triggered is zero, not scaling the histogram"
             << endl;
        exit(1);
      }
    }

    h2DMix->Delete();
    h2D->Delete();
    return h_assoYield;
  }

  TH2D *AssociatedYieldPtSum(int iMass, vector<int> vec_iPt,
                             vector<int> vec_iMult, bool doNTrigScale = true) {
    int nVtxZ = fHnSame->GetNbins(2);
    TH2D *h2_first =
        AssociatedYieldVtxZPtSum(1, iMass, vec_iPt, vec_iMult, false);
    // int iMass_new;
    // if (iMass != 0)
    //   iMass_new = fHnTrigger->hN->GetAxis(1)->FindBin(
    //       fHnMix->hN->GetAxis(3)->GetBinCenter(iMass));
    // else
    //   iMass_new = 0; // default to 1 if iMass is 0

    TH1D *h1_trigger =
        fHnTrigger->Project(0, {iMass, vec_iPt[0], vec_iMult[0]});
    for (int j = 0; j < vec_iMult.size(); j++)
      for (int i = 0; i < vec_iPt.size(); i++) {
        if (i == 0 && j == 0)
          continue;
        TH1D *h1D_temp =
            fHnTrigger->Project(0, {iMass, vec_iPt[i], vec_iMult[j]});
        h1_trigger->Add(h1D_temp);
        h1D_temp->Delete();
      }
    h1_trigger->SetName(Form("h1_trigger_%d", GenerateUID()));
    double sum_number_triggered = 0;
    for (int i = 1; i <= nVtxZ; i++) {
      double number_triggered = h1_trigger->GetBinContent(i);
      sum_number_triggered += number_triggered;
    }

    for (int i = 2; i <= nVtxZ; i++) {
      TH2D *h2D = AssociatedYieldVtxZPtSum(i, iMass, vec_iPt, vec_iMult, false);
      h2_first->Add(h2D);
      h2D->Delete();
    }
    if (sum_number_triggered == 0) {
      return h2_first;
    }
    // ScaleHisto2D(h2_first, 1.0 / sum_number_triggered);
    if (doNTrigScale) {
      if (sum_number_triggered != 0) {
        h2_first->Scale(1.0 / sum_number_triggered);
      } else {
        cerr << "AssocYieldHelper_v2:AssociatedYield: Error: "
                "sum_number_triggered is zero, not scaling the histogram"
             << endl;
        exit(1);
      }
    }
    h1_trigger->Delete();
    return h2_first;
  }

  TH2D *AssociatedYieldPtSum(int iMass, vector<int> vec_iPt, int iMult,
                             bool doNTrigScale = true) {
    int nVtxZ = fHnSame->GetNbins(2);
    TH2D *h2_first = AssociatedYieldVtxZPtSum(1, iMass, vec_iPt, iMult, false);
    // int iMass_new;
    // if (iMass != 0)
    //   iMass_new = fHnTrigger->hN->GetAxis(1)->FindBin(
    //       fHnMix->hN->GetAxis(3)->GetBinCenter(iMass));
    // else
    //   iMass_new = 0; // default to 1 if iMass is 0

    TH1D *h1_trigger = fHnTrigger->Project(0, {iMass, vec_iPt[0], iMult});
    for (int i = 1; i < vec_iPt.size(); i++) {
      TH1D *h1D_temp = fHnTrigger->Project(0, {iMass, vec_iPt[i], iMult});
      h1_trigger->Add(h1D_temp);
      h1D_temp->Delete();
    }
    h1_trigger->SetName(Form("h1_trigger_%d", GenerateUID()));
    double sum_number_triggered = 0;
    for (int i = 1; i <= nVtxZ; i++) {
      double number_triggered = h1_trigger->GetBinContent(i);
      sum_number_triggered += number_triggered;
    }
    for (int i = 2; i <= nVtxZ; i++) {
      TH2D *h2D = AssociatedYieldVtxZPtSum(i, iMass, vec_iPt, iMult, false);
      h2_first->Add(h2D);
      h2D->Delete();
    }
    if (sum_number_triggered == 0) {
      return h2_first;
    }
    // ScaleHisto2D(h2_first, 1.0 / sum_number_triggered);
    if (doNTrigScale) {
      if (sum_number_triggered != 0) {
        h2_first->Scale(1.0 / sum_number_triggered);
      } else {
        cerr << "AssocYieldHelper_v2:AssociatedYield: Error: "
                "sum_number_triggered is zero, not scaling the histogram"
             << endl;
        exit(1);
      }
    }
    h1_trigger->Delete();
    return h2_first;
  }

  TH2D *AssociatedYield(int iMass, int iPt, int iMult,
                        bool doNTrigScale = true) {
    int nVtxZ = fHnSame->GetNbins(2);
    TH2D *h2_first = AssociatedYieldVtxZ(1, iMass, iPt, iMult, false);
    // int iMass_new;
    // if (iMass != 0)
    //   iMass_new = fHnTrigger->hN->GetAxis(1)->FindBin(
    //       fHnMix->hN->GetAxis(3)->GetBinCenter(iMass));
    // else
    //   iMass_new = 0; // default to 1 if iMass is 0

    TH1D *h1_trigger = fHnTrigger->Project(0, {iMass, iPt, iMult});
    h1_trigger->SetName(Form("h1_trigger_%d", GenerateUID()));
    double sum_number_triggered = 0;
    for (int i = 1; i <= nVtxZ; i++) {
      double number_triggered = h1_trigger->GetBinContent(i);
      sum_number_triggered += number_triggered;
    }
    for (int i = 2; i <= nVtxZ; i++) {
      TH2D *h2D = AssociatedYieldVtxZ(i, iMass, iPt, iMult, false);
      h2_first->Add(h2D);
      h2D->Delete();
    }
    if (sum_number_triggered == 0) {
      return h2_first;
    }
    // ScaleHisto2D(h2_first, 1.0 / sum_number_triggered);
    if (doNTrigScale) {
      if (sum_number_triggered != 0) {
        h2_first->Scale(1.0 / sum_number_triggered);
      } else {
        cerr << "AssocYieldHelper_v2:AssociatedYield: Error: "
                "sum_number_triggered is zero, not scaling the histogram"
             << endl;
        exit(1);
      }
    }
    h1_trigger->Delete();
    return h2_first;
  }
};

class AssocYieldSub_v1 : public MHGroupTool1D {
private:
  int fBinLowMult = 0;
  int fBinHighMult = 0;

public:
  AssocYieldSub_v1(MHGroupTool1D *hgroupTool1d) : MHGroupTool1D(hgroupTool1d){};
  ~AssocYieldSub_v1(){};

  void Rebin(int n) {
    for (int i = 0; i < fHistos.size(); i++) {
      fHistos[i]->Rebin(n);
    }
  }

  void SetMultBin(int low, int high) {
    if (low < 0 || high < 0) {
      cerr << "Error: AssocYieldSub_v1::SetMultBin: low or high is negative"
           << endl;
      exit(1);
    } else if (low > high) {
      cerr << "Error: AssocYieldSub_v1::SetMultBin: low is greater than high"
           << endl;
      exit(1);
    } else if (low > fNbin_Var[2] || high > fNbin_Var[2]) {
      cerr << "Error: AssocYieldSub_v1::SetMultBin: low or high is out of range"
           << endl;
      exit(1);
    }
    fBinLowMult = low;
    fBinHighMult = high;
  }

  double GetBinContent(int phi, int mass, int pt, int mult) {
    TH1D *h1 = fHistos[GetBinIndex({mass, pt, mult})];
    if (h1 == nullptr) {
      cerr << "Error: AssocYieldSub_v1::GetBinContent: h1 is null" << endl;
      exit(1);
    }
    return h1->GetBinContent(phi);
  }

  double GetBinError(int phi, int mass, int pt, int mult) {
    TH1D *h1 = fHistos[GetBinIndex({mass, pt, mult})];
    if (h1 == nullptr) {
      cerr << "Error: AssocYieldSub_v1::GetBinError: h1 is null" << endl;
      exit(1);
    }
    return h1->GetBinError(phi);
  }

  double GetBinContentMultSub(int phi, int mass, int pt) {
    if (fBinLowMult == fBinHighMult) {
      cerr << "Error: AssocYieldSub_v1::GetBinContentMultSub: "
              "fBinLowMult or fBinHighMult is not set"
           << endl;
      exit(1);
    }
    return GetBinContent(phi, mass, pt, fBinHighMult) -
           GetBinContent(phi, mass, pt, fBinLowMult);
  }

  double GetBinErrorMultSub(int phi, int mass, int pt) {
    if (fBinLowMult == fBinHighMult) {
      cerr << "Error: AssocYieldSub_v1::GetBinErrorMultSub: "
              "fBinLowMult or fBinHighMult is not set"
           << endl;
      exit(1);
    }
    return TMath::Sqrt(GetBinError(phi, mass, pt, fBinHighMult) *
                           GetBinError(phi, mass, pt, fBinHighMult) +
                       GetBinError(phi, mass, pt, fBinLowMult) *
                           GetBinError(phi, mass, pt, fBinLowMult));
  }

  TH1D *GetAssoYield_Mass(int phi, int pt) {
    StrVar4Hist strMass = fStrsVar4Hist[0];
    TH1D *h1_assoYield_mass =
        new TH1D(Form("h1_%d", GenerateUID()), strMass.CompleteTitle(),
                 strMass.fNbins, strMass.fBins.data());

    for (int imass = 1; imass <= strMass.fNbins; imass++) {
      h1_assoYield_mass->SetBinContent(imass,
                                       GetBinContentMultSub(phi, imass, pt));
      h1_assoYield_mass->SetBinError(imass, GetBinErrorMultSub(phi, imass, pt));
    }
    return h1_assoYield_mass;
  }
};

#endif
