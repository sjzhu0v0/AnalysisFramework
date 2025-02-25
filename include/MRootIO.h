#ifndef __MRootIO_h__
#define __MRootIO_h__
#include "MHead.h"
#include "THashList.h"
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

} // namespace MRootIO
#endif