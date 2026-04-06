#include "TFile.h"
#include "iostream"

using namespace std;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cout << "Usage: IsRootFileGood <filename>" << endl;
    return 1;
  }

  for (int i_arg = 1; i_arg < argc; i_arg++) {
    // 1. 打印进度条
    int percent = (i_arg * 100) / (argc - 1);
    int n_bars = percent / 2; 
    
    cout << "\rProcessing: [";
    for (int j = 0; j < n_bars; j++) cout << "=";
    for (int j = n_bars; j < 50; j++) cout << " ";
    cout << "] " << percent << "%" << flush;

    // 2. 打开文件
    TFile *f = TFile::Open(argv[i_arg]);

    // 3. 检查文件是否存在 (空指针检查)
    if (f == 0) {
      // 使用 endl 会换行，这样不会破坏进度条的显示格式
      cout << endl << "File " << argv[i_arg] << " does not exist" << endl;
      // 注意：这里没有 return，程序会继续执行下面的代码
      continue; // 跳过后续操作，直接进入下一次循环
    }

    // 4. 检查文件是否损坏 (Zombie)
    if (f->IsZombie()) {
      cout << endl << argv[i_arg] << " is a Zombie (corrupted)" << endl;
    }

    // 5. 关闭文件
    if (f->IsOpen()) {
      f->Close();
    }
    
    // 释放内存 (ROOT 的好习惯)
    delete f; 
  }

  cout << endl << "All files processed." << endl;
  return 0; // 正常退出
}
