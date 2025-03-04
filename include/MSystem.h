#ifndef MSystem_h
#define MSystem_h

#include "fstream"
#include "iostream"
#include "nlohmann/json.hpp"
#include "string"

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

#ifdef MultiThread

#include <mutex>
#include <thread>
#include <vector>

std::mutex mtx;

struct gStrParameter4Thread;

void MultiThread(int nThreads, std::function<void(int)> func) {
  std::vector<std::thread> threads;
  for (int i = 0; i < nThreads; i++)
    threads.push_back(std::thread(func, i));
  for (int i = 0; i < nThreads; i++)
    threads[i].join();
}

#endif

#ifdef MRDF

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDFHelpers.hxx>
#include <ROOT/RVec.hxx>

using namespace std;
using namespace ROOT;
using namespace ROOT::RDF;

vector<RResultHandle> gRResultHandles;

#endif
