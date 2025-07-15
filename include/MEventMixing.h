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
  thread_local vector<tuple<int, vector<T1>>> vec_mix;
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

#endif // MEventMixing_h