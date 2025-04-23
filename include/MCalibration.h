#ifndef MCALIBRATION_H
#define MCALIBRATION_H

#include "TF1.h"
#include "TFile.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TString.h"

class Calib_NumContrib_fPosZ_Run {
  Calib_NumContrib_fPosZ_Run() {};
  ~Calib_NumContrib_fPosZ_Run() {};

public:
  static TH1D *fHistCali;
  static TF1 *fFuncCali;
  static TProfile *fProfile_run;
  static int fRunNumber;
  static double fMeanNumContrib;

  static void GetHistCali(TString path, TString path2, int runNumber = 0) {
    fRunNumber = runNumber;
    TString path_file = path(0, path.First(":"));
    TString path_hist = path(path.First(":") + 1, path.Length());

    TFile *file = TFile::Open(path_file);
    if (!file || file->IsZombie()) {
      std::cerr << "Error: Could not open file " << path_file << std::endl;
      return;
    }

    fHistCali = dynamic_cast<TH1D *>(file->Get(path_hist));

    fHistCali->SetDirectory(0);
    fFuncCali = (TF1 *)fHistCali->GetListOfFunctions()->FindObject("pol4");
    if (!fHistCali) {
      std::cerr << "Error: Could not find histogram " << path_hist << std::endl;
      file->Close();
      return;
    }

    TString path_file2 = path2(0, path2.First(":"));
    TString path_hist2 = path2(path2.First(":") + 1, path2.Length());
    TFile *file2 = TFile::Open(path_file2);
    if (!file2 || file2->IsZombie()) {
      std::cerr << "Error: Could not open file " << path_file2 << std::endl;
      return;
    }

    fProfile_run = dynamic_cast<TProfile *>(file2->Get(path_hist2));
    fProfile_run->SetDirectory(0);
    if (!fProfile_run) {
      std::cerr << "Error: Could not find histogram " << path_hist2
                << std::endl;
      file2->Close();
      return;
    }

    for (int i = 1; i <= fProfile_run->GetNbinsX(); i++) {
      TString label = fProfile_run->GetXaxis()->GetBinLabel(i);
      if (label == Form("%d", fRunNumber)) {
        fMeanNumContrib = fProfile_run->GetBinContent(i);
        break;
      }
    }

    file->Close();
    file2->Close();
  }

  static double GetCaliFactor(double posZ) {
    if (!fHistCali) {
      std::cerr << "Error: fHistCali is nullptr" << std::endl;
      return 1.0;
    }

    if (posZ < fHistCali->GetXaxis()->GetBinLowEdge(1) ||
        posZ > fHistCali->GetXaxis()->GetBinUpEdge(fHistCali->GetNbinsX())) {
      std::cerr << "Error: posZ is out of range for calibration" << std::endl;
      return 1.0;
    }

    return fFuncCali->Eval(posZ);
  }

  static double NumContribCalibrated(unsigned short numContrib, float posZ) {
    return numContrib * GetCaliFactor(posZ) / 50. / fMeanNumContrib;
  }

  static double NumContribCalibratedFloat(unsigned int, float numContrib,
                                          float posZ) {
    return numContrib * GetCaliFactor(posZ) * 50. / fMeanNumContrib;
  }
};

TF1 *Calib_NumContrib_fPosZ_Run::fFuncCali = nullptr;
TH1D *Calib_NumContrib_fPosZ_Run::fHistCali = nullptr;
TProfile *Calib_NumContrib_fPosZ_Run::fProfile_run = nullptr;
int Calib_NumContrib_fPosZ_Run::fRunNumber = 0;
double Calib_NumContrib_fPosZ_Run::fMeanNumContrib = 0.0;

#endif