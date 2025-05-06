#include "MHead.h"

#ifndef MRootGraphic_h
#define MRootGraphic_h

TCanvas *gCanvas = nullptr;

namespace MColorSpace {
const int RGB_4_0[4][3] = {
    {223, 122, 94}, {60, 64, 91}, {130, 178, 154}, {242, 204, 142}}; // 2
const int RGB_5_0[5][3] = {{38, 70, 83},
                           {42, 157, 142},
                           {233, 196, 107},
                           {243, 162, 97},
                           {230, 111, 81}}; // 3
const int RGB_5_1[5][3] = {
    {1, 7, 19}, {1, 36, 76}, {1, 53, 101}, {255, 195, 0}, {251, 132, 2}}; // 14
const int RGB_6_0[6][3] = {{0, 0, 0},     {58, 9, 100},   {122, 27, 109},
                           {189, 55, 82}, {237, 104, 37}, {251, 180, 26}}; // 18
const int RGB_7_0[7][3] = {{144, 201, 231}, {33, 158, 188}, {19, 103, 131},
                           {2, 48, 74},     {254, 183, 5},  {255, 158, 2},
                           {250, 134, 0}}; // 5
const int RGB_7_1[7][3] = {{38, 70, 83},    {40, 114, 113},  {42, 157, 140},
                           {138, 176, 125}, {233, 196, 107}, {243, 162, 97},
                           {230, 111, 81}}; // 16
const int Color_Severity[5] = {kCyan + 2, kGreen + 2, kYellow + 1, kOrange + 7,
                               kRed + 1};
} // namespace MColorSpace

int GetColor(const int *rgb) {
  return TColor::GetColor(*rgb, *(rgb + 1), *(rgb + 2));
}

namespace MRootGraphic {
TCanvas *ComaparisonHist1D(vector<TH1 *> vec_h1, TString option_draw = "",
                           TString name_canvas = "", int ww = 800,
                           int hh = 600) {
  for (int i = 0; i < vec_h1.size(); i++) {
    if (vec_h1[i] == nullptr) {
      cerr << "Error: vec_h1[" << i << "] is nullptr" << endl;
      exit(1);
    }
    if (vec_h1[i]->GetDimension() != 1) {
      cerr << "Error: vec_h1[" << i << "] is not 1D histogram" << endl;
      exit(1);
    }
  }

  // if name_canvas is empty, generate a unique name
  if (name_canvas == "")
    name_canvas = Form("CanvasQA_%d", GenerateUID());
  TCanvas *c1 = new TCanvas(name_canvas, name_canvas, ww, hh);
  c1->cd();
  for (int i = 0; i < vec_h1.size(); i++) {
    if (i == 0)
      vec_h1[i]->Draw(option_draw);
    else
      vec_h1[i]->Draw(option_draw + " same");
  }
  return c1;
}

TCanvas *ComaparisonHist1D(vector<TH1D *> vec_h1, TString option_draw = "",
                           TString name_canvas = "", int ww = 800,
                           int hh = 600) {
  for (int i = 0; i < vec_h1.size(); i++) {
    if (vec_h1[i] == nullptr) {
      cerr << "Error: vec_h1[" << i << "] is nullptr" << endl;
      exit(1);
    }
    if (vec_h1[i]->GetDimension() != 1) {
      cerr << "Error: vec_h1[" << i << "] is not 1D histogram" << endl;
      exit(1);
    }
  }

  // if name_canvas is empty, generate a unique name
  if (name_canvas == "")
    name_canvas = Form("CanvasQA_%d", GenerateUID());
  TCanvas *c1 = new TCanvas(name_canvas, name_canvas, ww, hh);
  c1->cd();
  for (int i = 0; i < vec_h1.size(); i++) {
    if (i == 0)
      vec_h1[i]->Draw(option_draw);
    else
      vec_h1[i]->Draw(option_draw + " same");
  }
  return c1;
}

void SetTh1LineMarker(vector<TH1 *> h1, const int (*group_color)[3]) {
  for (int i = 0; i < h1.size(); i++) {
    h1[i]->SetMarkerSize(0.5);
    h1[i]->SetMarkerStyle(4);
    h1[i]->SetLineColor(GetColor(*(group_color + i)));
    h1[i]->SetMarkerColor(GetColor(*(group_color + i)));
  }
}

void SetTh1LineMarker(vector<TH1D *> h1, const int (*group_color)[3]) {
  for (int i = 0; i < h1.size(); i++) {
    h1[i]->SetMarkerSize(0.5);
    h1[i]->SetMarkerStyle(4);
    h1[i]->SetLineColor(GetColor(*(group_color + i)));
    h1[i]->SetMarkerColor(GetColor(*(group_color + i)));
  }
}

void SetTextCentral(TLatex *lat, double x_center = 0.5) {
  double width = lat->GetXsize();
  double pos = x_center - width / 2;
  lat->SetX(pos);
}

} // namespace MRootGraphic

class MPublisherCanvas {
private:
  int fNWidth;
  int fNHeight;
  float fW_Width;
  float fW_Height;
  TString fName;
  TString fTitle;
  TString fPathPdf;

public:
  int fIndexPadCurrent = 0;
  int fNumberPage = 0;

  MPublisherCanvas(TString path_pdf, int n_width = 4, int n_height = 4,
                   float w_width = 80000, float w_height = 60000,
                   TString name = "c_publisher", TString title = "c_publisher")
      : fPathPdf(path_pdf), fNWidth(n_width), fNHeight(n_height),
        fW_Width(w_width), fW_Height(w_height), fName(name), fTitle(title) {
    if (gCanvas)
      delete gCanvas;

    gCanvas =
        new TCanvas(fName, fTitle, fW_Width * fNWidth, fW_Height * fNHeight);
    // set the number of pixels in the canvas
    gCanvas->SetCanvasSize(fW_Width * fNWidth, fW_Height * fNHeight);
  }

  ~MPublisherCanvas() { gCanvas->Update(); }

  int Index2Publish() {
    if (fIndexPadCurrent == fNWidth * fNHeight) {
      fIndexPadCurrent = 0;
      fNumberPage++;
      gCanvas->cd(0);
      TLatex *tex = new TLatex(0., 0.01, Form("Page %d", fNumberPage));
      tex->SetNDC();
      tex->SetTextSize(0.03);
      tex->Draw();
      gCanvas->Update();
      fNumberPage == 1 ? gCanvas->Print(fPathPdf + "(")
                       : gCanvas->Print(fPathPdf);
      gCanvas->Clear();
      delete tex;
    }
    fIndexPadCurrent++;
    return fIndexPadCurrent;
  }

  TPad *NewPad() {
    Index2Publish();
    gCanvas->cd();
    float x1 = 1. / (float)(fNWidth) * ((fIndexPadCurrent - 1) % fNWidth);
    float x2 = x1 + 1. / (float)(fNWidth);
    float y1 =
        1. - (1. / (fNHeight / 0.95) * ((fIndexPadCurrent - 1) / fNWidth));
    float y2 = y1 - 1. / (fNHeight / 0.95);
    TPad *pad = new TPad(Form("pad_%d", fIndexPadCurrent), "", x1, y2, x2, y1);
    pad->SetFillStyle(4000);
    pad->SetFrameFillStyle(4000);
    pad->SetCanvasSize(fW_Width, fW_Height);
    pad->Draw();
    return pad;
  }

  void NextPage() { fIndexPadCurrent = fNWidth * fNHeight; }

  void finalize() {
    fNumberPage++;
    gCanvas->cd(0);
    TLatex *tex = new TLatex(0., 0., Form("Page %d", fNumberPage));
    tex->SetNDC();
    tex->SetTextSize(0.03);
    gCanvas->Update();
    gCanvas->Print(fPathPdf + ")");
  }

  void SetCanvasNwNh(int n_width, int n_height) {
    fNWidth = n_width;
    fNHeight = n_height;
    fIndexPadCurrent = fNWidth * fNHeight;
  };

  void AddText(const char *text, double x = 0.5, double y = 0.01,
               int color = kBlack, double size = 0.05) {
    TVirtualPad *padCurrent = gPad;
    gCanvas->cd(0);
    TLatex *latex = new TLatex(x, y, text);
    latex->SetNDC();
    latex->SetTextSize(size);
    latex->SetTextColor(color);
    latex->Draw();
    padCurrent->cd();
  }

  void ShowObject(TObject *obj) {
    if (obj->IsFolder()) {
      TList *list = static_cast<TList *>(obj);
      TIter next(list);
      TObject *obj;
      while ((obj = next())) {
        ShowObject(obj);
      }
      return;
    }

    this->NewPad()->cd();
    int demension = ((TH1 *)obj)->GetDimension();
    if (demension == 1) {
      ((TH1 *)obj)->Draw();
    } else if (demension == 2) {
      ((TH2 *)obj)->Draw("colz");
    } else if (demension == 3) {
      ((TH3 *)obj)->Draw("box");
    }
    TIter next(((TH1 *)obj)->GetListOfFunctions());
    TObject *obj_func;
    while ((obj_func = next())) {
      if (obj_func->InheritsFrom("TF1")) {
        TF1 *f1 = reinterpret_cast<TF1 *>(obj_func);
        f1->Draw("same");
      }
      if (obj_func->InheritsFrom("TH1")) {
        TH1 *hist = reinterpret_cast<TH1 *>(obj_func);
        hist->Draw("AH same");
      }
    }
  }
};

MPublisherCanvas *gPublisherCanvas;

namespace StyleFlow {
void DeltaPhi_DeltaEta(TPad *pad, TH2D *h2) {
  // set pad as transparent
  pad->SetFillStyle(4000);
  pad->cd();
  h2->Draw("SURF1");
  pad->SetTopMargin(0.15);
  pad->SetBottomMargin(0.05);
  pad->SetLeftMargin(0.15);
  pad->SetRightMargin(0.05);

  pad->SetTheta(70);
  pad->SetPhi(40);

  TString title = h2->GetTitle();
  h2->SetTitle("");
  TLatex *tex = new TLatex(0.5, 0.95, title);
  // center the title
  double linewidth = tex->GetXsize();
  double pos = 0.55 - linewidth / 2;
  tex->SetX(pos);
  tex->SetNDC();
  tex->SetTextSize(0.055);
  tex->Draw();

  h2->SetTitleSize(0.05);

  h2->GetXaxis()->SetLabelSize(0.04);
  h2->GetYaxis()->SetLabelSize(0.04);
  h2->GetZaxis()->SetLabelSize(0.04);

  h2->GetXaxis()->SetTitleOffset(1.5);

  h2->GetXaxis()->SetTitleSize(0.05);
  h2->GetYaxis()->SetTitleSize(0.05);
  h2->GetZaxis()->SetTitleSize(0.05);

  h2->GetXaxis()->CenterTitle(true);
  h2->GetYaxis()->CenterTitle(true);
  h2->GetXaxis()->SetNdivisions(505);
  h2->GetYaxis()->SetNdivisions(505);
  h2->GetZaxis()->SetNdivisions(505);
}
} // namespace StyleFlow

#endif