#define MRDF
#include "MALICE.h"
#include "MHead.h"
#include "MHist.h"
#include "MRootIO.h"

funcWithJson(void, MultStudy)(TString path_config = "../config.json") {
  SetUpJson(path_config.Data());
  Configurable<string> config_pathInputFile("path_input", "path/to/dir/");
  Configurable<string> config_pathOutputFile("path_output", "output.root");

  //   /lustre/alice/users/szhu/job/Mult_22pass4/output/523142/O2reducedevent.root
  // /lustre/alice/users/szhu/job/Mult_22pass4/output/523142/O2reextended.root
  TFile *file_event = TFile::Open(
      Form("%s/O2reducedevent.root", config_pathInputFile.data.c_str()));
  TFile *file_event_ext = TFile::Open(
      Form("%s/O2reextended.root", config_pathInputFile.data.c_str()));
  TFile *fOutput = new TFile(config_pathOutputFile.data.c_str(), "RECREATE");

  TTree *tree_event = (TTree *)file_event->Get("O2reducedevent");
  TTree *tree_event_ext = (TTree *)file_event_ext->Get("O2reextended");

  tree_event->AddFriend(tree_event_ext);
  vector<RResultHandle> gRResultHandlesFast;
  ROOT::RDataFrame rdf(*tree_event);
  auto rdf_all =
      rdf.Define("isntSameBunchPileup", IsntSameBunchPileup, {"fSelection"});
  auto rdf_noPileup =
      rdf_all.Filter("isntSameBunchPileup", "no same bunch pileup");
  auto rdf_Pileup = rdf_all.Filter("!isntSameBunchPileup", "same bunch pileup");

  gRResultHandlesFast.push_back(rdf_all.Histo1D("fPosX"));
  gRResultHandlesFast.push_back(rdf_all.Histo1D("fPosY"));
  gRResultHandlesFast.push_back(rdf_all.Histo1D("fPosZ"));
  gRResultHandlesFast.push_back(rdf_all.Histo1D("fNumContrib"));
  gRResultHandlesFast.push_back(rdf_all.Histo1D("fMultTPC"));
  gRResultHandlesFast.push_back(rdf_all.Histo1D("fMultFV0A"));
  gRResultHandlesFast.push_back(rdf_all.Histo1D("fMultFT0A"));
  gRResultHandlesFast.push_back(rdf_all.Histo1D("fMultFT0C"));
  gRResultHandlesFast.push_back(rdf_all.Histo1D("fMultFDDA"));
  gRResultHandlesFast.push_back(rdf_all.Histo1D("fMultNTracksPV"));

  gRResultHandlesFast.push_back(rdf_noPileup.Histo1D("fPosX"));
  gRResultHandlesFast.push_back(rdf_noPileup.Histo1D("fPosY"));
  gRResultHandlesFast.push_back(rdf_noPileup.Histo1D("fPosZ"));
  gRResultHandlesFast.push_back(rdf_noPileup.Histo1D("fNumContrib"));
  gRResultHandlesFast.push_back(rdf_noPileup.Histo1D("fMultTPC"));
  gRResultHandlesFast.push_back(rdf_noPileup.Histo1D("fMultFV0A"));
  gRResultHandlesFast.push_back(rdf_noPileup.Histo1D("fMultFT0A"));
  gRResultHandlesFast.push_back(rdf_noPileup.Histo1D("fMultFT0C"));
  gRResultHandlesFast.push_back(rdf_noPileup.Histo1D("fMultFDDA"));
  gRResultHandlesFast.push_back(rdf_noPileup.Histo1D("fMultNTracksPV"));

  gRResultHandlesFast.push_back(rdf_Pileup.Histo1D("fPosX"));
  gRResultHandlesFast.push_back(rdf_Pileup.Histo1D("fPosY"));
  gRResultHandlesFast.push_back(rdf_Pileup.Histo1D("fPosZ"));
  gRResultHandlesFast.push_back(rdf_Pileup.Histo1D("fNumContrib"));
  gRResultHandlesFast.push_back(rdf_Pileup.Histo1D("fMultTPC"));
  gRResultHandlesFast.push_back(rdf_Pileup.Histo1D("fMultFV0A"));
  gRResultHandlesFast.push_back(rdf_Pileup.Histo1D("fMultFT0A"));
  gRResultHandlesFast.push_back(rdf_Pileup.Histo1D("fMultFT0C"));
  gRResultHandlesFast.push_back(rdf_Pileup.Histo1D("fMultFDDA"));
  gRResultHandlesFast.push_back(rdf_Pileup.Histo1D("fMultNTracksPV"));

  StrVar4Hist var_fPosX("fPosX", "fPosX", "cm", 200, {-0.1, 0.1});
  StrVar4Hist var_fPosY("fPosY", "fPosY", "cm", 200, {-0.1, 0.1});
  StrVar4Hist var_fPosZ("fPosZ", "fPosZ", "cm", 200, {-10, 10});
  StrVar4Hist var_fNumContrib("fNumContrib", "fNumContrib", "a.u.", 300,
                              {0, 300});
  StrVar4Hist var_fMultTPC("fMultTPC", "fMultTPC", "a.u.", 600, {0, 600});
  StrVar4Hist var_fMultFV0A("fMultFV0A", "fMultFV0A", "a.u.", 100, {0, 50.e3});
  StrVar4Hist var_fMultFT0A("fMultFT0A", "fMultFT0A", "a.u.", 310,
                            {-1000., 30000.});
  StrVar4Hist var_fMultFT0C("fMultFT0C", "fMultFT0C", "a.u.", 130,
                            {-1000., 12000.});
  StrVar4Hist var_fMultFDDA("fMultFDDA", "fMultFDDA", "a.u.", 360,
                            {-1000, 35000});
  StrVar4Hist var_fMultNTracksPV("fMultNTracksPV", "fMultNTracksPV", "a.u.",
                                 150, {0, 150});
  vector<StrVar4Hist> vec_var_pos = {var_fPosX, var_fPosY, var_fPosZ};
  vector<StrVar4Hist> vec_var_mult = {
      var_fNumContrib, var_fMultTPC,  var_fMultFV0A,     var_fMultFT0A,
      var_fMultFT0C,   var_fMultFDDA, var_fMultNTracksPV};

// mult correlation
#define RHistDefine2DLoop(df, v1, v2, cond)                                    \
  TString title = Form("%s_%s%s;%s (%s);%s (%s)", v1.fName.Data(),             \
                       v2.fName.Data(), cond.Data(), v1.fTitle.Data(),         \
                       v1.fUnit.Data(), v2.fTitle.Data(), v2.fUnit.Data());    \
  TH2DModel h2_mult(                                                           \
      Form("%s_%s%s", v1.fName.Data(), v2.fName.Data(), cond.Data()), title,   \
      v1.fNbins, v1.fBins.data(), v2.fNbins, v2.fBins.data());                 \
  gRResultHandlesFast.push_back(df.Histo2D(h2_mult, v1.fName, v2.fName));

  for (int i_mult = 0; i_mult < vec_var_mult.size(); i_mult++) {
    for (int j_mult = i_mult + 1; j_mult < vec_var_mult.size(); j_mult++) {
      if (i_mult == j_mult)
        continue;
      RHistDefine2DLoop(rdf_all, vec_var_mult[i_mult], vec_var_mult[j_mult],
                        gEmptyString);
    }
  }
  TString condition_noSameBunchPileup = "NoPileup";
  TString condition_SameBunchPileup = "Pileup";
  for (int i_mult = 0; i_mult < vec_var_mult.size(); i_mult++) {
    for (int j_mult = i_mult + 1; j_mult < vec_var_mult.size(); j_mult++) {
      if (i_mult == j_mult)
        continue;
      RHistDefine2DLoop(rdf_noPileup, vec_var_mult[i_mult],
                        vec_var_mult[j_mult], condition_noSameBunchPileup);
    }
  }

  for (int i_mult = 0; i_mult < vec_var_mult.size(); i_mult++) {
    for (int j_mult = i_mult + 1; j_mult < vec_var_mult.size(); j_mult++) {
      if (i_mult == j_mult)
        continue;
      RHistDefine2DLoop(rdf_Pileup, vec_var_mult[i_mult], vec_var_mult[j_mult],
                        condition_SameBunchPileup);
    }
  }

  // posz and mult correlation
  for (int i_mult = 0; i_mult < vec_var_mult.size(); i_mult++) {
    RHistDefine2DLoop(rdf_all, var_fPosZ, vec_var_mult[i_mult], gEmptyString);
  }

  for (int i_mult = 0; i_mult < vec_var_mult.size(); i_mult++) {
    RHistDefine2DLoop(rdf_noPileup, var_fPosZ, vec_var_mult[i_mult],
                      condition_noSameBunchPileup);
  }

  for (int i_mult = 0; i_mult < vec_var_mult.size(); i_mult++) {
    RHistDefine2DLoop(rdf_Pileup, var_fPosZ, vec_var_mult[i_mult],
                      condition_SameBunchPileup);
  }

  RunGraphs(gRResultHandlesFast);

  fOutput->cd();
  vector<TString> vec_name;
  vector<int> time;
  for (auto handle : gRResultHandlesFast) {
    TString name;
    bool is_th1 = false;
    bool is_th2 = false;
    bool is_th3 = false;
    try {
      auto th1 = handle.GetPtr<TH1D>();
      is_th1 = true;
    } catch (const std::exception &e) {
      is_th1 = false;
    }
    try {
      auto th2 = handle.GetPtr<TH2D>();
      is_th2 = true;
    } catch (const std::exception &e) {
      is_th2 = false;
    }
    try {
      auto th3 = handle.GetPtr<TH3D>();
      is_th3 = true;
    } catch (const std::exception &e) {
      is_th3 = false;
    }
    if (is_th1) {
      name = handle.GetPtr<TH1D>()->GetName();
    } else if (is_th2) {
      name = handle.GetPtr<TH2D>()->GetName();
    } else if (is_th3) {
      name = handle.GetPtr<TH3D>()->GetName();
    }

    bool doExist = false;
    int index_exist = -1;
    for (int i = 0; i < vec_name.size(); i++) {
      if (name == vec_name[i]) {
        doExist = true;
        index_exist = i;
        break;
      }
    }
    if (doExist) {
      time[index_exist]++;
    } else {
      vec_name.push_back(name);
      time.push_back(0);
    }

    if (!doExist) {
      if (is_th1) {
        handle.GetPtr<TH1D>()->Write();
      } else if (is_th2) {
        handle.GetPtr<TH2D>()->Write();
      } else if (is_th3) {
        handle.GetPtr<TH3D>()->Write();
      }
    } else {
      if (is_th1) {
        handle.GetPtr<TH1D>()->SetName(
            Form("%s_%d", handle.GetPtr<TH1D>()->GetName(), time[index_exist]));
        cout << time[index_exist] << endl;
        handle.GetPtr<TH1D>()->Write();
      } else if (is_th2) {
        handle.GetPtr<TH2D>()->SetName(
            Form("%s_%d", handle.GetPtr<TH2D>()->GetName(), time[index_exist]));
        handle.GetPtr<TH2D>()->Write();
      } else if (is_th3) {
        handle.GetPtr<TH3D>()->SetName(
            Form("%s_%d", handle.GetPtr<TH3D>()->GetName(), time[index_exist]));
        handle.GetPtr<TH3D>()->Write();
      }
    }
  }
  fOutput->Close();
}

int main(int argc, char **argv) {
  if (argc == 1)
    MultStudy();
  else
    MultStudy(argv[1]);
  return 0;
}