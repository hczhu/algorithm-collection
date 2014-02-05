#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <unordered_map>
#include <cassert>
using namespace std;

struct FileAndLock {
  explicit FileAndLock(const string& filename)
      : fileLock(make_shared<mutex>()) {
    ofstream tmp(filename, ofstream::out | ofstream::trunc);
    tmp.close();
    fileToAppend = shared_ptr<ofstream>(
        new ofstream(filename, ofstream::out | ofstream::app),
        [filename](ofstream* file) {
          file->flush();
          file->close();
          delete file;
          cout << "Closed file: " << filename;
    });
    assert(fileToAppend->is_open());
  }
  FileAndLock(const FileAndLock& other) = default;
  FileAndLock() = default;

  shared_ptr<ofstream> fileToAppend;
  shared_ptr<mutex> fileLock;
};

int main() {
  vector<thread> threads;
  FileAndLock fileAndLock("/tmp/test.txt");
  for (int i = 0; i < 5; ++i) {
    threads.emplace_back([i, &fileAndLock]() {
      for (int j = 0; j < 5; ++j) {
        lock_guard<mutex> lock(*fileAndLock.fileLock);
        *fileAndLock.fileToAppend << "Thread #" << i << " couter: " << j << endl;
      }
    });
  }
  for (auto& thr : threads) {
    thr.join();
  }
  return 0;
}
