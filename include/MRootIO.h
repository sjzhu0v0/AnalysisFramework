#ifndef __MRootIO_h__
#define __MRootIO_h__
#include "MHead.h"
#include "THashList.h"
#include "THn.h"
#include "TObjString.h"

namespace MRootIO {
TChain *OpenChain(const char *name_file, const char *name_tree) {
  TFile *f = new TFile(name_file);
  TChain *chain = new TChain(name_tree);

  TList *list = f->GetListOfKeys();
  for (int i = 0; i < list->GetSize(); i++) {
    TKey *key = (TKey *)list->At(i);
    if (strcmp(key->GetClassName(), "TDirectoryFile") == 0) {
      if (string(key->GetName()).find("DF_") != string::npos)
        chain->Add(TString(name_file) + "/" + TString(key->GetName()) +
                   TString("/") + TString(name_tree));
    }
  }
  return chain;
}
TChain *OpenChain(TFile *f, const char *name_tree) {
  TChain *chain = new TChain(name_tree);
  TString name_file = f->GetName();

  TList *list = f->GetListOfKeys();
  for (int i = 0; i < list->GetSize(); i++) {
    TKey *key = (TKey *)list->At(i);
    if (strcmp(key->GetClassName(), "TDirectoryFile") == 0) {
      if (string(key->GetName()).find("DF_") != string::npos)
        chain->Add(name_file + "/" + TString(key->GetName()) + TString("/") +
                   TString(name_tree));
    }
  }
  return chain;
}

vector<TObject *> GetObjectRecursive(TObject *folder,
                                     vector<string> &vec_string) {
  vector<TObject *> vec_obj;

  if (!folder)
    return vec_obj;

  if (!folder->IsFolder()) {
    vec_obj.push_back(folder);
    vec_string.push_back(folder->GetName());
    return vec_obj;
  }

  // TCollection TDirecotry
  if (folder->IsA()->InheritsFrom(TDirectory::Class())) {
    TDirectory *dir = static_cast<TDirectory *>(folder);
    TIter next(dir->GetListOfKeys());
    TKey *key;
    while ((key = (TKey *)next())) {
      TClass *cl = gROOT->GetClass(key->GetClassName());
      if (!cl->InheritsFrom(TObject::Class()))
        continue;
      TObject *obj = key->ReadObj();
      vector<string> vec_string_tmp;
      vector<TObject *> vec_obj_tmp = GetObjectRecursive(obj, vec_string_tmp);
      for (auto &str : vec_string_tmp) {
        str = (string)folder->GetName() + "/" + str;
      }
      vec_obj.insert(vec_obj.end(), vec_obj_tmp.begin(), vec_obj_tmp.end());
      vec_string.insert(vec_string.end(), vec_string_tmp.begin(),
                        vec_string_tmp.end());
    }
  } else if (folder->IsA()->InheritsFrom(TList::Class())) {
    TList *list = static_cast<TList *>(folder);
    TIter next(list);
    TObject *obj;
    while ((obj = next())) {
      vector<string> vec_string_tmp;
      vector<TObject *> vec_obj_tmp = GetObjectRecursive(obj, vec_string_tmp);
      for (auto &str : vec_string_tmp) {
        str = (string)folder->GetName() + "/" + str;
      }
      vec_obj.insert(vec_obj.end(), vec_obj_tmp.begin(), vec_obj_tmp.end());
      vec_string.insert(vec_string.end(), vec_string_tmp.begin(),
                        vec_string_tmp.end());
    }
  } else {
    cout << "Unknown class: " << folder->ClassName() << endl;
  }

  return vec_obj;
}

template <typename T>
vector<T *> GetObjectVector(TDirectory *dir = gDirectory) {
  vector<T *> list;
  TIter next(dir->GetListOfKeys());
  TKey *key;
  while ((key = (TKey *)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom(T::Class()))
      continue;
    T *obj = (T *)key->ReadObj();
    list.push_back(obj);
  }
  return list;
}

template <typename T>
vector<T *>
GetObjectVector(vector<TObject *> vec_obj, vector<string> vec_string_input,
                vector<string> &vec_string_output, string str_tag = "") {
  vector<T *> list;
  for (int i = 0; i < vec_string_input.size(); i++) {
    if (vec_obj[i]->InheritsFrom(T::Class())) {
      if (strcmp(str_tag.c_str(), "") == 0) {
        list.push_back((T *)vec_obj[i]);
        vec_string_output.push_back(vec_string_input[i]);
      } else {
        if (vec_string_input[i].find(str_tag) != string::npos) {
          list.push_back((T *)vec_obj[i]);
          vec_string_output.push_back(vec_string_input[i]);
        }
      }
    }
  }
  return list;
}

template <typename T>
vector<T *> GetObjectVector(vector<TObject *> vec_obj,
                            vector<string> vec_string_input,
                            vector<string> &vec_string_output,
                            vector<string> str_tag = {}) {
  vector<T *> list;
  for (int i = 0; i < vec_string_input.size(); i++) {
    if (vec_obj[i]->InheritsFrom(T::Class())) {
      if (str_tag.size() == 0) {
        list.push_back((T *)vec_obj[i]);
        vec_string_output.push_back(vec_string_input[i]);
      } else {
        bool is_tag = true;
        for (auto &tag : str_tag) {
          if (vec_string_input[i].find(tag) == string::npos) {
            is_tag = false;
            break;
          }
        }
        if (is_tag) {
          list.push_back((T *)vec_obj[i]);
          vec_string_output.push_back(vec_string_input[i]);
          cout << vec_string_input[i] << " found" << endl;
        }
      }
    }
  }
  return list;
}

template <typename T>
vector<T *>
GetObjectVector(vector<TObject *> vec_obj, vector<string> vec_string_input,
                vector<string> vec_string_output, TString str_tag = "") {
  return GetObjectVector<T>(vec_obj, vec_string_input, vec_string_output,
                            (string)str_tag);
}

typedef struct StrObjectRepository {
  vector<string> vec_string;
  vector<TObject *> vec_obj;
} StrObjectRepository;

void PrintObjectRepository(StrObjectRepository repository_obj) {
  for (int i = 0; i < repository_obj.vec_string.size(); i++) {
    cout << repository_obj.vec_string[i] << endl;
  }
}

StrObjectRepository GetObjectRecursive(TString path_file) {
  TFile *f = new TFile(path_file);
  vector<string> vec_string;
  vector<TObject *> vec_obj = GetObjectRecursive(f, vec_string);
  StrObjectRepository storage;
  storage.vec_string = vec_string;
  storage.vec_obj = vec_obj;

  for (int i = 0; i < vec_string.size(); i++) {
    size_t pos = vec_string[i].find(".root/");
    if (pos != string::npos) {
      storage.vec_string[i] = vec_string[i].substr(pos + 6);
    }
  }

  return storage;
}

vector<StrObjectRepository> GetObjectRecursive(vector<TString> vec_path_file) {
  vector<StrObjectRepository> vec_storage;
  for (auto &path : vec_path_file) {
    vec_storage.push_back(GetObjectRecursive(path));
  }
  return vec_storage;
}

vector<TObject *> GetObjectFromRepository(StrObjectRepository repository_obj,
                                          TString string_tag = "") {
  vector<TObject *> vec_obj;
  if (string_tag != "") {
    // check if string_tag contains "*"
    if (string_tag.Contains("*")) {
      // get all strings divided by "*"
      TObjArray *arr = string_tag.Tokenize("*");
      for (int i = 0; i < repository_obj.vec_string.size(); i++) {
        bool is_exist = true;
        for (int j = 0; j < arr->GetEntries(); j++) {
          TString str = ((TObjString *)arr->At(j))->String();
          if (repository_obj.vec_string[i].find(str) == string::npos) {
            is_exist = false;
            break;
          }
        }
        if (is_exist) {
          vec_obj.push_back(repository_obj.vec_obj[i]);
        }
      }
    } else {
      for (int i = 0; i < repository_obj.vec_string.size(); i++) {
        if (repository_obj.vec_string[i].find(string_tag) != string::npos) {
          vec_obj.push_back(repository_obj.vec_obj[i]);
        }
      }
    }
  } else {
    vec_obj = repository_obj.vec_obj;
  }
  return vec_obj;
}

vector<TObject *>
GetObjectFromRepository(vector<StrObjectRepository> repository_obj,
                        TString string_tag) {
  vector<TObject *> vec_obj;
  for (int i = 0; i < repository_obj.size(); i++) {
    vector<TObject *> vec_obj_tmp =
        GetObjectFromRepository(repository_obj[i], string_tag);
    vec_obj.insert(vec_obj.end(), vec_obj_tmp.begin(), vec_obj_tmp.end());
  }
  return vec_obj;
}

template <typename T> vector<T *> ConvertVecTObject(vector<TObject *> vec_obj) {
  vector<T *> vec_t;
  for (auto &obj : vec_obj) {
    if (obj->InheritsFrom(T::Class())) {
      vec_t.push_back((T *)obj);
    }
  }
  return vec_t;
}

TH1D *GetTH1D(TString path) {
  TString path_file = path(0, path.First(":"));
  TString path_hist = path(path.First(":") + 1, path.Length());

  TFile *file = TFile::Open(path_file);
  if (!file || file->IsZombie()) {
    std::cerr << "Error: Could not open file " << path_file << std::endl;
    exit(1);
  }

  auto hist = dynamic_cast<TH1D *>(file->Get(path_hist));
  hist->SetDirectory(0);
  if (!hist) {
    std::cerr << "Error: Could not find histogram " << path_hist << std::endl;
    file->Close();
    exit(1);
  }

  file->Close();
  return hist;
}

TH2D *GetTH2D(TString path) {
  TString path_file = path(0, path.First(":"));
  TString path_hist = path(path.First(":") + 1, path.Length());

  TFile *file = TFile::Open(path_file);
  if (!file || file->IsZombie()) {
    std::cerr << "Error: Could not open file " << path_file << std::endl;
    exit(1);
  }

  auto hist = dynamic_cast<TH2D *>(file->Get(path_hist));
  hist->SetDirectory(0);
  if (!hist) {
    std::cerr << "Error: Could not find histogram " << path_hist << std::endl;
    file->Close();
    exit(1);
  }

  file->Close();
  return hist;
}

THnD *GetTHnD(TString path) {
  TString path_file = path(0, path.First(":"));
  TString path_hist = path(path.First(":") + 1, path.Length());

  TFile *file = TFile::Open(path_file);
  if (!file || file->IsZombie()) {
    std::cerr << "Error: Could not open file " << path_file << std::endl;
    exit(1);
  }

  auto hist = dynamic_cast<THnD *>(file->Get(path_hist));
  if (!hist) {
    std::cerr << "Error: Could not find histogram " << path_hist << std::endl;
    file->Close();
    exit(1);
  }

  file->Close();
  return hist;
}

}; // namespace MRootIO
#ifdef MRDF

#include <ROOT/RDFHelpers.hxx>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>

using namespace std;
using namespace ROOT;
using namespace ROOT::RDF;

void RResultWrite(vector<RResultHandle> gRResultHandlesFast) {
  vector<TString> vec_name;
  vector<int> time;
  for (auto handle : gRResultHandlesFast) {
    TString name;
    bool is_th1 = false;
    bool is_th2 = false;
    bool is_th3 = false;
    bool is_thn = false;
    bool is_pro1 = false;
    try {
      auto th1 = handle.GetPtr<TH1D>();
      is_th1 = true;
    } catch (const std::exception &e) {
      is_th1 = false;
    }
    try {
      auto th2 = handle.GetPtr<TH2D>();
      is_th2 = true;
    } catch (const std::exception &e) {
      is_th2 = false;
    }
    try {
      auto th3 = handle.GetPtr<TH3D>();
      is_th3 = true;
    } catch (const std::exception &e) {
      is_th3 = false;
    }
    try {
      auto thn = handle.GetPtr<THnD>();
      is_thn = true;
    } catch (const std::exception &e) {
      is_thn = false;
    }

    if (is_th1) {
      name = handle.GetPtr<TH1D>()->GetName();
    } else if (is_th2) {
      name = handle.GetPtr<TH2D>()->GetName();
    } else if (is_th3) {
      name = handle.GetPtr<TH3D>()->GetName();
    } else if (is_thn) {
      name = handle.GetPtr<THnD>()->GetName();
    } else {
      cout << "Error: Unknown histogram type" << endl;
      continue;
    }

    bool doExist = false;
    int index_exist = -1;
    for (int i = 0; i < vec_name.size(); i++) {
      if (name == vec_name[i]) {
        doExist = true;
        index_exist = i;
        break;
      }
    }
    if (doExist) {
      time[index_exist]++;
    } else {
      vec_name.push_back(name);
      time.push_back(0);
    }

    if (!doExist) {
      if (is_th1) {
        handle.GetPtr<TH1D>()->Write();
      } else if (is_th2) {
        handle.GetPtr<TH2D>()->Write();
      } else if (is_th3) {
        handle.GetPtr<TH3D>()->Write();
      } else if (is_thn) {
        handle.GetPtr<THnD>()->Write();
      }
    } else {
      if (is_th1) {
        handle.GetPtr<TH1D>()->SetName(
            Form("%s_%d", handle.GetPtr<TH1D>()->GetName(), time[index_exist]));
        cout << time[index_exist] << endl;
        handle.GetPtr<TH1D>()->Write();
      } else if (is_th2) {
        handle.GetPtr<TH2D>()->SetName(
            Form("%s_%d", handle.GetPtr<TH2D>()->GetName(), time[index_exist]));
        handle.GetPtr<TH2D>()->Write();
      } else if (is_th3) {
        handle.GetPtr<TH3D>()->SetName(
            Form("%s_%d", handle.GetPtr<TH3D>()->GetName(), time[index_exist]));
        handle.GetPtr<TH3D>()->Write();
      } else if (is_thn) {
        handle.GetPtr<THnD>()->SetName(
            Form("%s_%d", handle.GetPtr<THn>()->GetName(), time[index_exist]));
        handle.GetPtr<THnD>()->Write();
      }
    }
  }
}

#endif

#endif