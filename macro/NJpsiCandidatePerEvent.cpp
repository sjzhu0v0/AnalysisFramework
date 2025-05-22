#define MRDF
#include "MALICE.h"
#include "MHead.h"
#include "MHist.h"
#include "MRootIO.h"
#include "TApplication.h"
#include "TLorentzVector.h"

void NJpsiCandidatePerEvent(
    TString path_input =
        "/home/szhu/data/PairFlow/22pass4_highIR/sample/O2dqflowvecd.root",
    TString path_output = "/home/szhu/work/alice/analysis/QA/output/event_jpsi/"
                          "NJpsiCandidatePerEvent.root") {
  TFile *file_event = TFile::Open(path_input);
  TFile *fOutput = new TFile(path_output, "RECREATE");

  TTree *tree_event = (TTree *)file_event->Get("O2dqflowvecd");

  vector<RResultHandle> gRResultHandlesFast;
  ROOT::RDataFrame rdf(*tree_event);

  auto isJPsiCandidate = [](float mass, int sign) {
    return (mass > 2.5f && mass < 3.2f && sign == 0);
  };

  /* #region rdf_all definition */
  auto rdf_all =
      rdf.Define("NJpsiCandidata",
                 [isJPsiCandidate](const ROOT::RVec<float> &mass,
                                   const ROOT::RVec<float> &sign) {
                   int nJpsiCandidate = 0;
                   for (size_t i = 0; i < mass.size(); ++i) {
                     if (isJPsiCandidate(mass[i], sign[i]))
                       nJpsiCandidate++;
                   }
                   return nJpsiCandidate;
                 },
                 {"fMass", "fSign"})
          .Define("mass_pair",
                  [isJPsiCandidate](const ROOT::RVec<float> &mass,
                                    const ROOT::RVec<float> &sign) {
                    ROOT::VecOps::RVec<std::pair<double, double>> pairs;
                    for (size_t i = 0; i < mass.size(); ++i) {
                      for (size_t j = i + 1; j < mass.size(); ++j) {
                        if (sign[i] == 0 && sign[j] == 0) {
                          pairs.push_back(std::make_pair(mass[i], mass[j]));
                        }
                      }
                    }
                    return pairs;
                  },
                  {"fMass", "fSign"})
          .Define(
              "mass_pair1",
              [](const ROOT::VecOps::RVec<std::pair<double, double>> pairs) {
                ROOT::VecOps::RVec<double> mass1;
                for (const auto &pair : pairs) {
                  mass1.push_back(pair.first);
                }
                return mass1;
              },
              {"mass_pair"})
          .Define(
              "mass_pair2",
              [](const ROOT::VecOps::RVec<std::pair<double, double>> pairs) {
                ROOT::VecOps::RVec<double> mass2;
                for (const auto &pair : pairs) {
                  mass2.push_back(pair.second);
                }
                return mass2;
              },
              {"mass_pair"})
          .Define("k_star",
                  [](const ROOT::RVec<float> &mass,
                     const ROOT::RVec<float> &phi, const ROOT::RVec<float> &eta,
                     const ROOT::RVec<float> &pt,
                     const ROOT::RVec<float> &sign) {
                    ROOT::RVec<float> k_star;
                    for (size_t i = 0; i < mass.size(); ++i) {
                      if (mass[i] > 2.5 && mass[i] < 3.2 && sign[i] == 0)
                        k_star.push_back(-1.);
                      for (size_t j = 0; j < i; ++j) {
                        TLorentzVector p1, p2;
                        if (sign[i] == 0 && sign[j] == 0 && mass[i] > 2.5 &&
                            mass[i] < 3.2 && mass[j] > 2.5 && mass[j] < 3.2) {
                          p1.SetPtEtaPhiM(pt[i], eta[i], phi[i], 3.0969);
                          p2.SetPtEtaPhiM(pt[j], eta[j], phi[j], 3.0969);
                          TLorentzVector p_sum = p1 + p2;
                          TLorentzVector p_delta = p1 - p2;
                          p_delta.Boost(-1. * p_sum.BoostVector());
                          // cout << p_delta.Vect().Mag() << endl;
                          k_star.push_back(p_delta.Vect().Mag());
                        }
                      }
                    }
                    return k_star;
                  },
                  {"fMass", "fPhi", "fEta", "fPT", "fSign"});
  // ROOT::RDF::Experimental::AddProgressBar(rdf_all);
  /* #endregion */

  gRResultHandlesFast.push_back(rdf_all.Histo1D(
      {"NJpsiCandidata", "N_{J/#psi candidates} (2.8<mass<3.2 GeV/c)", 6, -0.5,
       5.5},
      "NJpsiCandidata"));
  gRResultHandlesFast.push_back(rdf_all.Histo1D(
      {"k_star", "k* (GeV); k* (GeV)", 11000, -2, 10}, "k_star"));
  gRResultHandlesFast.push_back(
      rdf_all.Histo2D({"pair_mass", "Mass Pair; Mass (GeV); Mass (GeV)", 10, 1.,
                       5., 10, 1., 5.},
                      "mass_pair1", "mass_pair2"));

  RunGraphs(gRResultHandlesFast);

  fOutput->cd();
  RResultWrite(gRResultHandlesFast);
  fOutput->Close();
}

int main(int argc, char **argv) {
  TString path_input = argv[1];
  TString path_output = argv[2];
  NJpsiCandidatePerEvent(path_input, path_output);
}