#ifndef MEventMixing_h
#define MEventMixing_h

#include "ROOT/RVec.hxx"
#include "iostream"
#include "tuple"
#include "vector"

using namespace std;

template <typename T, typename T1>
vector<T> MixVec(int id, const T1 &input, T (*formula)(const T1 &, const T1 &),
                 int depth = 20) {
  static vector<tuple<int, vector<T1>>> vec_mix;
  vector<T> vec_output;
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