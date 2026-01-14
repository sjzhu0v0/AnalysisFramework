#include <TBranch.h>
#include <TFile.h>
#include <TLeaf.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <ROOT/RVec.hxx>

#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <vector>

using ROOT::VecOps::RVec;

// ============================================================
// 抽象接口
// ============================================================
struct ColumnBase {
  virtual ~ColumnBase() = default;
  virtual bool isVector() const = 0;
  virtual size_t size() = 0;               // 依赖当前 entry
  virtual void fillScalar(size_t idx) = 0; // 标量 broadcast，向量取 idx
  virtual void bookOut(TTree &out) = 0;
  virtual const std::string &name() const = 0;
};

// ============================================================
// 标量列
// ============================================================
template <typename T> struct ScalarColumn : ColumnBase {
  TTreeReaderValue<T> reader;
  T outValue{};
  std::string colName;

  ScalarColumn(TTreeReader &r, const std::string &n)
      : reader(r, n.c_str()), colName(n) {}

  bool isVector() const override { return false; }
  size_t size() override { return 1; }

  void fillScalar(size_t) override { outValue = *(reader.Get()); }

  void bookOut(TTree &out) override { out.Branch(colName.c_str(), &outValue); }

  const std::string &name() const override { return colName; }
};

// ============================================================
// 向量列（RVec<T> -> 标量 T）
// ============================================================
template <typename T> struct VectorColumn : ColumnBase {
  TTreeReaderValue<RVec<T>> reader;
  T outValue{};
  std::string colName;

  VectorColumn(TTreeReader &r, const std::string &n)
      : reader(r, n.c_str()), colName(n) {}

  bool isVector() const override { return true; }

  size_t size() override { return reader.Get()->size(); }

  void fillScalar(size_t idx) override { outValue = reader.Get()->at(idx); }

  void bookOut(TTree &out) override { out.Branch(colName.c_str(), &outValue); }

  const std::string &name() const override { return colName; }
};

// ============================================================
// 工厂函数：自动创建 Column
// ============================================================
static std::unique_ptr<ColumnBase> MakeColumn(TTreeReader &reader, TBranch *br,
                                              const std::string &name) {
  const std::string cls = br->GetClassName();

  // -------- RVec --------
  if (cls.find("ROOT::VecOps::RVec<float>") != std::string::npos)
    return std::make_unique<VectorColumn<float>>(reader, name);
  if (cls.find("ROOT::VecOps::RVec<double>") != std::string::npos)
    return std::make_unique<VectorColumn<double>>(reader, name);
  if (cls.find("ROOT::VecOps::RVec<int>") != std::string::npos)
    return std::make_unique<VectorColumn<int>>(reader, name);
  if (cls.find("ROOT::VecOps::RVec<bool>") != std::string::npos)
    return std::make_unique<VectorColumn<bool>>(reader, name);
  if (cls.find("ROOT::VecOps::RVec<unsigned char>") != std::string::npos)
    return std::make_unique<VectorColumn<unsigned char>>(reader, name);
  if (cls.find("ROOT::VecOps::RVec<ULong64_t>") != std::string::npos ||
      cls.find("ROOT::VecOps::RVec<unsigned long long>") != std::string::npos)
    return std::make_unique<VectorColumn<ULong64_t>>(reader, name);

  // -------- 标量 --------
  TLeaf *leaf = br->GetLeaf(name.c_str());
  if (!leaf)
    return nullptr;

  const std::string t = leaf->GetTypeName();
  if (t == "Float_t")
    return std::make_unique<ScalarColumn<float>>(reader, name);
  if (t == "Double_t")
    return std::make_unique<ScalarColumn<double>>(reader, name);
  if (t == "Int_t")
    return std::make_unique<ScalarColumn<int>>(reader, name);
  if (t == "Bool_t")
    return std::make_unique<ScalarColumn<bool>>(reader, name);
  if (t == "unsigned char")
    return std::make_unique<ScalarColumn<unsigned char>>(reader, name);
  if (t == "ULong64_t")
    return std::make_unique<ScalarColumn<ULong64_t>>(reader, name);

  std::cerr << "[WARN] Unsupported branch skipped: " << name << " (" << t
            << ")\n";
  return nullptr;
}

// ============================================================
// 核心函数
// ============================================================
void flatten_tree_reader_full(const char *infile, const char *intree,
                              const char *outfile, const char *outtree,
                              const char *explode_regex) {
  TFile fin(infile, "READ");
  if (fin.IsZombie()) {
    std::cerr << "[ERROR] Cannot open input file\n";
    return;
  }

  TTree *tin = (TTree *)fin.Get(intree);
  if (!tin) {
    std::cerr << "[ERROR] Cannot find input tree\n";
    return;
  }

  std::regex explode_pat(explode_regex);
  TTreeReader reader(tin);

  std::vector<std::unique_ptr<ColumnBase>> columns;
  std::vector<ColumnBase *> vectorCols;

  TObjArray *brs = tin->GetListOfBranches();
  for (int i = 0; i < brs->GetEntries(); ++i) {
    TBranch *br = (TBranch *)brs->At(i);
    const std::string name = br->GetName();
    const std::string cls = br->GetClassName();

    const bool isRVec =
        (!cls.empty() && cls.find("ROOT::VecOps::RVec") != std::string::npos);

    if (isRVec && !std::regex_match(name, explode_pat))
      continue;

    auto col = MakeColumn(reader, br, name);
    if (!col)
      continue;

    if (col->isVector())
      vectorCols.push_back(col.get());
    columns.emplace_back(std::move(col));
  }

  if (vectorCols.empty()) {
    std::cerr << "[ERROR] No vector columns selected for explode\n";
    return;
  }

  TFile fout(outfile, "RECREATE");
  TTree tout(outtree, outtree);
  for (auto &c : columns)
    c->bookOut(tout);

  Long64_t outEntries = 0;

  while (reader.Next()) {
    const size_t n = vectorCols.front()->size();
    if (n == 0)
      continue;

    for (auto *v : vectorCols) {
      if (v->size() != n) {
        std::cerr << "[ERROR] Vector size mismatch\n";
        return;
      }
    }

    for (size_t i = 0; i < n; ++i) {
      for (auto &c : columns)
        c->fillScalar(i);
      tout.Fill();
      ++outEntries;
    }
  }

  fout.Write();
  std::cout << "[DONE] Written " << outEntries << " entries\n";
}

// ============================================================
// main()
// ============================================================
int main(int argc, char **argv) {
  const char *default_regex = ".*";

  if (argc < 5) {
    std::cerr << "Usage:\n"
              << "  " << argv[0]
              << " input.root inTree output.root outTree [explode_regex]\n\n"
              << "Examples:\n"
              << "  " << argv[0]
              << " input.root jpsi_ref_pairs out.root jpsi_ref_pairs_flat\n"
              << "  " << argv[0]
              << " input.root jpsi_ref_pairs out.root jpsi_ref_pairs_flat "
                 "\"^(jpsi_|e1_|e2_)\"\n\n"
              << "Default explode_regex:\n"
              << "  " << default_regex << "\n";
    return 1;
  }

  const char *infile = argv[1];
  const char *intree = argv[2];
  const char *outfile = argv[3];
  const char *outtree = argv[4];

  const char *explode_regex = (argc >= 6) ? argv[5] : default_regex;

  std::cout << "[INFO] Input file      : " << infile << "\n"
            << "[INFO] Input tree      : " << intree << "\n"
            << "[INFO] Output file     : " << outfile << "\n"
            << "[INFO] Output tree     : " << outtree << "\n"
            << "[INFO] Explode regex   : " << explode_regex << "\n";

  flatten_tree_reader_full(infile, intree, outfile, outtree, explode_regex);

  return 0;
}
