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
  static TF1 *fFuncCali;

public:
  static int fRunNumber;
  static double fMeanNumContrib;

  static void GetHistCali(TString path, int runNumber = 0) {
    fRunNumber = runNumber;
    TString path_file = path(0, path.First(":"));
    TString path_hist = path(path.First(":") + 1, path.Length());

    TFile *file = TFile::Open(path_file);
    if (!file || file->IsZombie()) {
      std::cerr << "Error: Could not open file " << path_file << std::endl;
      return;
    }
    try {
      fFuncCali = dynamic_cast<TF1 *>(
          file->Get(Form("%s%d", path_hist.Data(), fRunNumber)));
    } catch (const std::exception &e) {
      cout << "Error: Calib_NumContrib_fPosZ_Run::GetHistCali: Could not get "
              "TF1 from file: "
           << e.what() << endl;
    }

    file->Close();
  }

  static double GetCaliFactor(double posZ) { return fFuncCali->Eval(posZ); }

  static double NumContribCalibrated(unsigned short numContrib, float posZ) {
    return numContrib * GetCaliFactor(posZ);
  }

  static double NumContribCalibratedFloat(unsigned int,
                                          unsigned short numContrib,
                                          float posZ) {
    return numContrib * GetCaliFactor(posZ);
  }
};
TF1 *Calib_NumContrib_fPosZ_Run::fFuncCali = nullptr;
int Calib_NumContrib_fPosZ_Run::fRunNumber = 0;
double Calib_NumContrib_fPosZ_Run::fMeanNumContrib = 0.0;

class Cut_MultTPC_NumContrib {
  Cut_MultTPC_NumContrib() {};
  ~Cut_MultTPC_NumContrib() {};
  static TF1 *fFuncCut;

public:
  static void init(TString path_func) {
    if (fFuncCut) {
      delete fFuncCut;
    }
    TString path_file = path_func(0, path_func.First(":"));
    TString path_func = path_func(path_func.First(":") + 1, path_func.Length());
    TFile *file = TFile::Open(path_file);
    if (!file || file->IsZombie()) {
      std::cerr << "Error: Could not open file " << path_file << std::endl;
      return;
    }
    try {
      fFuncCut = dynamic_cast<TF1 *>(file->Get(path_func));
    } catch (const std::exception &e) {
      cout << "Error: Cut_MultTPC_NumContrib::init: Could not get TF1 from "
              "file: "
           << e.what() << endl;
    }
    if (!fFuncCut) {
      std::cerr << "Error: Could not get TF1 " << path_func << " from file "
                << path_file << std::endl;
      return;
    }
    file->Close();
  }

  static bool IsInCut(unsigned short numContrib, int multTPC) {
    if (!fFuncCut) {
      std::cerr << "Error: Cut_MultTPC_NumContrib::IsInCut: fFuncCut is not "
                   "initialized"
                << std::endl;
      return false;
    }
    double cutValue = fFuncCut->Eval(numContrib);
    return multTPC < cutValue;
  }

  static bool isInCutSlot(unsigned int, unsigned short numContrib,
                          float multTPC) {
    return IsInCut(numContrib, multTPC);
  }
};

TF1 *Cut_MultTPC_NumContrib::fFuncCut = nullptr;
#endif