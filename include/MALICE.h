#ifndef MALICE_H
#define MALICE_H
#include "MRootIO.h"
#include "TMath.h"
#include "vector"

using std::vector;

namespace MALICE {
enum EventSelectionFlags {
  kIsBBV0A = 0, // cell-averaged time in V0A in beam-beam window
  kIsBBV0C, // cell-averaged time in V0C in beam-beam window (for Run 2 only)
  kIsBBFDA, // cell-averaged time in FDA (or AD in Run2) in beam-beam window
  kIsBBFDC, // cell-averaged time in FDC (or AD in Run2) in beam-beam window
  kIsBBT0A, // cell-averaged time in T0A in beam-beam window
  kIsBBT0C, // cell-averaged time in T0C in beam-beam window
  kNoBGV0A, // cell-averaged time in V0A in beam-gas window
  kNoBGV0C, // cell-averaged time in V0C in beam-gas window (for Run 2 only)
  kNoBGFDA, // cell-averaged time in FDA (AD in Run2) in beam-gas window
  kNoBGFDC, // cell-averaged time in FDC (AD in Run2) in beam-gas window
  kNoBGT0A, // cell-averaged time in T0A in beam-gas window
  kNoBGT0C, // cell-averaged time in T0C in beam-gas window
  kIsBBZNA, // time in common ZNA channel in beam-beam window
  kIsBBZNC, // time in common ZNC channel in beam-beam window
  kIsBBZAC, // time in ZNA and ZNC in beam-beam window - circular cut in ZNA-ZNC
            // plane
  kNoBGZNA, // time in common ZNA channel is outside of beam-gas window
  kNoBGZNC, // time in common ZNC channel is outside of beam-gas window
  kNoV0MOnVsOfPileup, // no out-of-bunch pileup according to online-vs-offline
                      // VOM correlation
  kNoSPDOnVsOfPileup, // no out-of-bunch pileup according to online-vs-offline
                      // SPD correlation
  kNoV0Casymmetry, // no beam-gas according to correlation of V0C multiplicities
                   // in V0C3 and V0C012
  kIsGoodTimeRange,  // good time range
  kNoIncompleteDAQ,  // complete event according to DAQ flags
  kNoTPCLaserWarmUp, // no TPC laser warm-up event (used in Run 1)
  kNoTPCHVdip,       // no TPC HV dip
  kNoPileupFromSPD,  // no pileup according to SPD vertexer
  kNoV0PFPileup,     // no out-of-bunch pileup according to V0 past-future info
  kNoSPDClsVsTklBG,  // no beam-gas according to cluster-vs-tracklet correlation
  kNoV0C012vsTklBG,  // no beam-gas according to V0C012-vs-tracklet correlation
  kNoInconsistentVtx,  // no inconsistency in SPD and Track vertices
  kNoPileupInMultBins, // no pileup according to multiplicity-differential
                       // pileup checks
  kNoPileupMV,         // no pileup according to multi-vertexer
  kNoPileupTPC,        // no pileup in TPC
  kIsTriggerTVX, // FT0 vertex (acceptable FT0C-FT0A time difference) at trigger
                 // level
  kIsINT1,       // SPDGFO >= 1 || V0A || V0C
  kNoITSROFrameBorder, // bunch crossing is far from ITS RO Frame border
  kNoTimeFrameBorder,  // bunch crossing is far from Time Frame borders
  kNoSameBunchPileup,  // reject collisions in case of pileup with another
                       // collision in the same foundBC
  kIsGoodZvtxFT0vsPV,  // small difference between z-vertex from PV and from FT0
  kIsVertexITSTPC,     // at least one ITS-TPC track (reject vertices built from
                       // ITS-only tracks)
  kIsVertexTOFmatched, // at least one of vertex contributors is matched to TOF
  kIsVertexTRDmatched, // at least one of vertex contributors is matched to TRD
  kNoCollInTimeRangeNarrow,   // no other collisions in specified time range
                              // (narrower than Strict)
  kNoCollInTimeRangeStrict,   // no other collisions in specified time range
  kNoCollInTimeRangeStandard, // no other collisions in specified time range
                              // with per-collision multiplicity above threshold
  kNoCollInRofStrict,         // no other collisions in this Readout Frame
  kNoCollInRofStandard,       // no other collisions in this Readout Frame with
                              // per-collision multiplicity above threshold
  kNoHighMultCollInPrevRof,   // veto an event if FT0C amplitude in previous ITS
                              // ROF is above threshold
  kIsGoodITSLayer3, // number of inactive chips on ITS layer 3 is below maximum
                    // allowed value
  kIsGoodITSLayer0123, // numbers of inactive chips on ITS layers 0-3 are below
                       // maximum allowed values
  kIsGoodITSLayersAll, // numbers of inactive chips on all ITS layers are below
                       // maximum allowed values
  kNsel                // counter
};

bool doEventHaveTrigger(const ULong64_t &eventSelectionMask, const int &flag) {
  return eventSelectionMask & (1 << flag);
}

vector<double> triggermap(ULong64_t fSelection) {
  // EventSelectionFlags::kNsel
  vector<double> trigger_map;
  for (int i = 0; i < EventSelectionFlags::kNsel; i++) {
    if ((fSelection >> i) & 1)
      trigger_map.push_back(i);
  }
  // if (trigger_map.size() == 0)
  //   trigger_map.push_back(-1);
  return trigger_map;
}

bool IsntSPDPileup(ULong64_t fSelection) {
  return (fSelection >> EventSelectionFlags::kNoPileupFromSPD) & 1;
}

bool IsntTPCPileup(ULong64_t fSelection) {
  return (fSelection >> EventSelectionFlags::kNoPileupTPC) & 1;
}

// kNoSameBunchPileup
// bool IsntSameBunchPileup(ULong64_t fSelection) {
//   return (fSelection >> EventSelectionFlags::kNoSameBunchPileup) & 1;
// }
bool IsntSameBunchPileup(unsigned int, ULong64_t fSelection) {
  return (fSelection >> EventSelectionFlags::kNoSameBunchPileup) & 1;
}

TH1D *h1_runInfo = nullptr;
double
EventNumberMinbias(int run,
                   TString EventNumberMinbias =
                       "/home/szhu/work/alice/analysis/InfoRun/"
                       "runInfo22_DQ.root:bc-selection-task/hCounterTVX") {
  if (h1_runInfo == nullptr) {
    h1_runInfo = MRootIO::GetTH1D(EventNumberMinbias);
  }
  for (int i = 1; i <= h1_runInfo->GetNbinsX(); i++) {
    TString name_run = h1_runInfo->GetXaxis()->GetBinLabel(i);
    if (name_run.Atoi() == run) {
      return h1_runInfo->GetBinContent(i);
    }
  }
  return -1;
}
}; // namespace MALICE

#endif // MALICE_H