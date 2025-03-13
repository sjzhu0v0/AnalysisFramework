#include "MHead.h"
#include "MRootGraphic.h"
#include "TPDF.h"

void HistComparison(
    TString path_file_input =
        "/home/szhu/work/alice/analysis/MultStudy/output/MultStudy.root") {
  TFile *file_input = TFile::Open(path_file_input);

  TList *keys = file_input->GetListOfKeys();

  std::vector<std::string> histNames;
  std::vector<TH1 *> histList;
  vector<int> vec_group;
  TIter next(keys);
  TKey *key;
  int group = 0;

  while ((key = (TKey *)next())) {
    TObject *obj = key->ReadObj();
    if (obj->InheritsFrom("TH1")) {
      histNames.push_back(obj->GetName());
      histList.push_back((TH1 *)obj);
      TString name(obj->GetName());
      bool doBelong = false;
      for (int i = 0; i < histNames.size() - 1; i++) {
        TString name2(histNames[i].c_str());
        doBelong = name.Contains(name2) || name2.Contains(name);
        // if (doBelong)
        //   cout << name << " belongs to " << name2 << endl;
        doBelong &= static_cast<TH1 *>(obj)->GetDimension() ==
                    static_cast<TH1 *>(file_input->Get(histNames[i].c_str()))
                        ->GetDimension();
        // if (doBelong)
        //   cout << name << " belongs to " << name2 << endl;
        if (doBelong) {
          vec_group.push_back(vec_group[i]);
          break;
        }
      }
      if (!doBelong) {
        vec_group.push_back(group);
        group++;
      }
    }
  }

  // print the group
  for (int i = 0; i < histNames.size(); i++) {
    cout << histNames[i] << " belongs to group " << vec_group[i] << endl;
  }

  for (int i = 0; i < group; i++) {
    int n_hist = 0;
    for (int j = 0; j < histNames.size(); j++) {
      if (vec_group[j] == i) {
        n_hist++;
      }
    }
    int scale = ceil(sqrt(n_hist + 1));
    gCanvas =
        new TCanvas(Form("c%d", i), Form("c%d", i), 400 * scale, 400 * scale);
    gCanvas->Divide(scale, scale);
    n_hist = 0;
    TString title; // find the shortest name
    for (int j = 0; j < histNames.size(); j++) {
      if (vec_group[j] == i) {
        n_hist++;
        gCanvas->cd(n_hist);
        histList[j]->Draw();
        if (title.IsNull() || title.Length() > histNames[j].length()) {
          title = histNames[j].c_str();
        }
      }
    }
    gCanvas->cd(n_hist + 1);
    n_hist = 0;
    for (int j = 0; j < histNames.size(); j++) {
      if (vec_group[j] == i) {
        TH1 *hist = (TH1 *)histList[j]->Clone(
            Form("%s_%d", histNames[j].c_str(), GenerateUID()));
        if (hist->GetDimension() == 1) {
          hist->Draw(n_hist == 0 ? "" : "same");
        }
        n_hist++;
      }
    }

    gCanvas->Update();
    if (i == 0)
      gCanvas->Print("output.pdf(", Form("Title:%s", title.Data()));
    else
      gCanvas->Print("output.pdf", Form("Title:%s", title.Data()));
    delete gCanvas;
    gCanvas =
        new TCanvas(Form("c%d", i), Form("c%d", i), 400 * scale, 400 * scale);
    // set log scale
    gCanvas->Divide(scale, scale);
    n_hist = 0;
    for (int j = 0; j < histNames.size(); j++) {
      if (vec_group[j] == i) {
        n_hist++;
        gCanvas->cd(n_hist);
        if (histList[j]->GetDimension() == 1) {
          gPad->SetLogy();
        } else if (histList[j]->GetDimension() == 2) {
          gPad->SetLogz();
        }
        histList[j]->Draw();
        if (title.IsNull() || title.Length() > histNames[j].length()) {
          title = histNames[j].c_str();
        }
      }
    }
    gCanvas->cd(n_hist + 1);
    n_hist = 0;
    for (int j = 0; j < histNames.size(); j++) {
      if (vec_group[j] == i) {
        TH1 *hist = (TH1 *)histList[j]->Clone(
            Form("%s_%d", histNames[j].c_str(), GenerateUID()));
        if (hist->GetDimension() == 1) {
          hist->Scale(1. / hist->Integral());
          hist->Draw(n_hist == 0 ? "" : "same");
          gPad->SetLogy();
        } else if (hist->GetDimension() == 2) {
          gPad->SetLogz();
        }
      }
      n_hist++;
    }
    gCanvas->Update();
    if (i == group - 1)
      gCanvas->Print("output.pdf)", Form("Title:%s_log", title.Data()));
    else
      gCanvas->Print("output.pdf", Form("Title:%s_log", title.Data()));
    delete gCanvas;
  }
}