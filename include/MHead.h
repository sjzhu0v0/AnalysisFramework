#ifndef __MHead_h__
#define __MHead_h__

#include "TCanvas.h"
#include "TChain.h"
#include "TColor.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THashList.h"
#include "TKey.h"
#include "TLatex.h"
#include "TLine.h"
#include "TList.h"
#include "TROOT.h"
#include "TTree.h"
#include "iostream"
#include "fstream"
#include "string"
#include "nlohmann/json.hpp"

using namespace std;

int GenerateUID() {
  static int uid = 0;
  return uid++;
}

using std::cout;
using std::endl;
using std::string;
using json = nlohmann::json;

json gJson;
bool doVarExist(std::string nameNode1, std::string nameNode2,
                json json4read = gJson) {
  return json4read[nameNode1][nameNode2].is_null();
}

template <typename T>
T GetJsonData(std::string nameNode1, std::string nameNode2,
              json json4read = gJson) {
  try {
    T data = json4read[nameNode1][nameNode2].get<T>();
    return data;
  } catch (json::type_error &e) {
    cout << "Error: " << e.what() << endl;
    cout << "Error at json file: { " << nameNode1 << " : " << nameNode2 << " }"
         << endl;
    exit(1);
  }
  return T();
}

#define ConfigurableDefine(name)                                               \
  template <typename T> struct Configurable##name {                            \
    std::string fNameNode = #name;                                             \
    T data;                                                                    \
    Configurable##name(string d, T value_default) {                            \
      data = value_default;                                                    \
      if (!doVarExist(fNameNode, d))                                           \
        data = GetJsonData<T>(fNameNode, d);                                   \
    }                                                                          \
  };

#define funcWithJson(type, name)                                               \
  ConfigurableDefine(name) template <typename T>                               \
  using Configurable = Configurable##name<T>;                                  \
  type name

void SetUpJson(string config) {
  // if json is empty, read from file
  if (gJson.empty()) {
    std::ifstream i(config);
    i >> gJson;
  }
}

#endif