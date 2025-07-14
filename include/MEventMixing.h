#ifndef MEventMixing_h
#define MEventMixing_h

#include "ROOT/RVec.hxx"
#include "iostream"
#include "tuple"
#include "vector"

using namespace std;

struct EventInfo {
  Int_t fMultTPC = 0;
  Int_t fMultTracklets = 0;
  Int_t fMultNTracksPV = 0;
  Float_t fMultFT0C = 0;
  Short_t fNumContrib = 0;
  Float_t fPosX = 0;
  Float_t fPosY = 0;
  Float_t fPosZ = 0;
  Long64_t fSelection = 0;
  Float_t fHadronicRate = 0;

  void Copy(EventInfo other) {
    fMultTPC = other.fMultTPC;
    fMultTracklets = other.fMultTracklets;
    fMultNTracksPV = other.fMultNTracksPV;
    fMultFT0C = other.fMultFT0C;
    fNumContrib = other.fNumContrib;
    fPosX = other.fPosX;
    fPosY = other.fPosY;
    fPosZ = other.fPosZ;
    fSelection = other.fSelection;
    fHadronicRate = other.fHadronicRate;
  }
};

struct JpsiInfo {
  Int_t fPT_size = 0;
  std::vector<Float_t> fPT;

  Int_t fEta_size = 0;
  std::vector<Float_t> fEta;

  Int_t fPhi_size = 0;
  std::vector<Float_t> fPhi;

  Int_t fMass_size = 0;
  std::vector<Float_t> fMass;

  Int_t fSign_size = 0;
  std::vector<Float_t> fSign;

  template <typename T> void Copy(const T &other) {
    fPT_size = other.fPT_size;
    fPT.assign(other.fPT.begin(), other.fPT.end());
    fEta_size = other.fEta_size;
    fEta.assign(other.fEta.begin(), other.fEta.end());
    fPhi_size = other.fPhi_size;
    fPhi.assign(other.fPhi.begin(), other.fPhi.end());
    fMass_size = other.fMass_size;
    fMass.assign(other.fMass.begin(), other.fMass.end());
    fSign_size = other.fSign_size;
    fSign.assign(other.fSign.begin(), other.fSign.end());
  }
};

struct TrackInfo {
  Int_t fPTREF_size = 0;
  std::vector<Float_t> fPTREF;

  Int_t fEtaREF_size = 0;
  std::vector<Float_t> fEtaREF;

  Int_t fPhiREF_size = 0;
  std::vector<Float_t> fPhiREF;

  template <typename T> void Copy(const T &other) {
    fPTREF_size = other.fPTREF_size;
    fPTREF.assign(other.fPTREF.begin(), other.fPTREF.end());
    fEtaREF_size = other.fEtaREF_size;
    fEtaREF.assign(other.fEtaREF.begin(), other.fEtaREF.end());
    fPhiREF_size = other.fPhiREF_size;
    fPhiREF.assign(other.fPhiREF.begin(), other.fPhiREF.end());
  }
};

struct EventData {
  EventInfo event_info;
  EventInfo event_info2;
  JpsiInfo jpsi_info;
  TrackInfo track_info;
};

EventData CreateEventData(Int_t fMultTPC, Int_t fMultTracklets,
                          Int_t fMultNTracksPV, Float_t fMultFT0C,
                          Short_t fNumContrib, Float_t fPosX, Float_t fPosY,
                          Float_t fPosZ, Long64_t fSelection,
                          Float_t fHadronicRate,
                          const ROOT::VecOps::RVec<Float_t> &fPT,
                          const ROOT::VecOps::RVec<Float_t> &fEta,
                          const ROOT::VecOps::RVec<Float_t> &fPhi,
                          const ROOT::VecOps::RVec<Float_t> &fMass,
                          const ROOT::VecOps::RVec<Float_t> &fSign,
                          const ROOT::VecOps::RVec<Float_t> &fPTREF,
                          const ROOT::VecOps::RVec<Float_t> &fEtaREF,
                          const ROOT::VecOps::RVec<Float_t> &fPhiREF) {
  EventData event;

  // 填充固定大小的变量
  event.event_info.fMultTPC = fMultTPC;
  event.event_info.fMultTracklets = fMultTracklets;
  event.event_info.fMultNTracksPV = fMultNTracksPV;
  event.event_info.fMultFT0C = fMultFT0C;
  event.event_info.fNumContrib = fNumContrib;
  event.event_info.fPosX = fPosX;
  event.event_info.fPosY = fPosY;
  event.event_info.fPosZ = fPosZ;
  event.event_info.fSelection = fSelection;
  event.event_info.fHadronicRate = fHadronicRate;

  // 填充动态数组
  event.jpsi_info.fPT_size = fPT.size();
  event.jpsi_info.fPT.assign(fPT.begin(), fPT.end());

  event.jpsi_info.fEta_size = fEta.size();
  event.jpsi_info.fEta.assign(fEta.begin(), fEta.end());

  event.jpsi_info.fPhi_size = fPhi.size();
  event.jpsi_info.fPhi.assign(fPhi.begin(), fPhi.end());

  event.jpsi_info.fMass_size = fMass.size();
  event.jpsi_info.fMass.assign(fMass.begin(), fMass.end());

  event.jpsi_info.fSign_size = fSign.size();
  event.jpsi_info.fSign.assign(fSign.begin(), fSign.end());

  event.track_info.fPTREF_size = fPTREF.size();
  event.track_info.fPTREF.assign(fPTREF.begin(), fPTREF.end());

  event.track_info.fEtaREF_size = fEtaREF.size();
  event.track_info.fEtaREF.assign(fEtaREF.begin(), fEtaREF.end());

  event.track_info.fPhiREF_size = fPhiREF.size();
  event.track_info.fPhiREF.assign(fPhiREF.begin(), fPhiREF.end());

  return event;
};

template <typename T, typename T1>
ROOT::VecOps::RVec<T> MixVec(int id, T1 input, T (*formula)(T1 &, T1 &),
                             int depth = 20) {
  static vector<tuple<int, vector<T1>>> vec_mix;
  ROOT::VecOps::RVec<T> vec_output;
  int index_class = -1;
  for (int i = 0; i < vec_mix.size(); i++) {
    if (get<0>(vec_mix[i]) == id) {
      index_class = i;
      break;
    }
  }

  if (index_class != -1) {
    for (const auto &input_vec : get<1>(vec_mix[index_class])) {
      T output = formula(input, input_vec);
      vec_output.push_back(output);
    }
  }

  if (index_class == -1) {
    vector<T1> vec_input;
    vec_input.push_back(input);
    vec_mix.push_back(make_tuple(id, vec_input));
  } else {
    get<1>(vec_mix[index_class]).push_back(input);
  }

  // check length of vec in vec_mix, if > depth, remove the first element
  for (int i = 0; i < vec_mix.size(); i++) {
    if (get<1>(vec_mix[i]).size() > depth) {
      get<1>(vec_mix[i]).erase(get<1>(vec_mix[i]).begin());
    }
  }

  return vec_output;
}

void MixJpsiTrack(EventData &event_info) {
  auto output_eventMixing = MixVec<EventData, EventData>(
      0, event_info, [](EventData &a, EventData &b) {
        EventData event;
        event.event_info.Copy(a.event_info);
        event.event_info2.Copy(b.event_info);
        event.jpsi_info.Copy(a.jpsi_info);
        event.track_info.Copy(b.track_info);
        return event;
      });
}

ROOT::VecOps::RVec<EventData> MixEvent(const int id,
                                       const EventData &event_info) {
  return MixVec<EventData, EventData>(id, event_info,
                                      [](EventData &a, EventData &b) {
                                        EventData event;
                                        event.event_info.Copy(a.event_info);
                                        event.event_info2.Copy(b.event_info);
                                        event.jpsi_info.Copy(a.jpsi_info);
                                        event.track_info.Copy(b.track_info);
                                        return event;
                                      });
}

//  OBJ: TLeafI    fMultTPC        fMultTPC
//  OBJ: TLeafI    fMultTracklets  fMultTracklets
//  OBJ: TLeafI    fMultNTracksPV  fMultNTracksPV
//  OBJ: TLeafF    fMultFT0C       fMultFT0C
//  OBJ: TLeafS    fNumContrib     fNumContrib
//  OBJ: TLeafF    fPosX   fPosX
//  OBJ: TLeafF    fPosY   fPosY
//  OBJ: TLeafF    fPosZ   fPosZ
//  OBJ: TLeafL    fSelection      fSelection
//  OBJ: TLeafF    fHadronicRate   fHadronicRate
//  OBJ: TLeafI    fPT_size        fPT_size
//  OBJ: TLeafF    fPT     fPT[fPT_size]
//  OBJ: TLeafI    fEta_size       fEta_size
//  OBJ: TLeafF    fEta    fEta[fEta_size]
//  OBJ: TLeafI    fPhi_size       fPhi_size
//  OBJ: TLeafF    fPhi    fPhi[fPhi_size]
//  OBJ: TLeafI    fMass_size      fMass_size
//  OBJ: TLeafF    fMass   fMass[fMass_size]
//  OBJ: TLeafI    fSign_size      fSign_size
//  OBJ: TLeafF    fSign   fSign[fSign_size]
//  OBJ: TLeafI    fPTREF_size     fPTREF_size
//  OBJ: TLeafF    fPTREF  fPTREF[fPTREF_size]
//  OBJ: TLeafI    fEtaREF_size    fEtaREF_size
//  OBJ: TLeafF    fEtaREF fEtaREF[fEtaREF_size]
//  OBJ: TLeafI    fPhiREF_size    fPhiREF_size
//  OBJ: TLeafF    fPhiREF fPhiREF[fPhiREF_size]

#endif // MEventMixing_h