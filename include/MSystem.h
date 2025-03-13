#ifndef MSystem_h
#define MSystem_h

#include "fstream"
#include "iostream"
#include "nlohmann/json.hpp"
#include "string"

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

#include <functional>
#include <iostream>
#include <tuple>
#include <vector>

// 模板递归结构体
template <size_t Level, typename... Ts> struct Loop {
  template <typename Func>
  static void iterate(const std::tuple<std::vector<Ts>...> &vectors,
                      std::tuple<Ts...> &current, Func func) {
    const auto &vec = std::get<Level>(vectors); // 获取当前层级的向量
    for (const auto &item : vec) {
      std::get<Level>(current) = item; // 设置当前层级的值
      Loop<Level + 1, Ts...>::iterate(vectors, current, func); // 递归到下一层级
    }
  }
};

// 模板特化，终止递归
template <typename... Ts> struct Loop<sizeof...(Ts), Ts...> {
  template <typename Func>
  static void iterate(const std::tuple<std::vector<Ts>...> &vectors,
                      std::tuple<Ts...> &current, Func func) {
    func(current); // 调用处理函数
  }
};

// int main() {
//     // 定义输入向量（支持多种类型）
//     std::tuple<std::vector<const char*>, std::vector<int>,
//     std::vector<double>> vectors = {
//         {"Electron", "Pion"},       // const char*
//         {1, 2},                     // int
//         {3.14, 2.71}                // double
//     };

//     // 用于存储当前循环的值
//     std::tuple<const char*, int, double> current;

//     // 定义处理函数
//     auto func = [](const std::tuple<const char*, int, double>& values) {
//         std::cout << std::get<0>(values) << " "
//                   << std::get<1>(values) << " "
//                   << std::get<2>(values) << std::endl;
//     };

//     // 调用模板递归
//     Loop<0, const char*, int, double>::iterate(vectors, current, func);

//     return 0;
// }

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

#include <ROOT/RDFHelpers.hxx>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>

using namespace std;
using namespace ROOT;
using namespace ROOT::RDF;

vector<RResultHandle> gRResultHandles;

#endif
