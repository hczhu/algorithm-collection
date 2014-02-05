#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <map>
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
          cout << "Closed file: " << filename << endl;
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
  map<int, FileAndLock> fileMap;
  fileMap.insert(make_pair(0, FileAndLock("/tmp/test-0.txt")));
  fileMap.insert(make_pair(1, FileAndLock("/tmp/test-1.txt")));
  fileMap.insert(make_pair(2, FileAndLock("/tmp/test-2.txt")));
  for (int i = 0; i < 6; ++i) {
    threads.emplace_back([i, &fileMap]() {
      for (int j = 0; j < 5; ++j) {
        lock_guard<mutex> lock(*fileMap[i % 3].fileLock);
        *fileMap[i % 3].fileToAppend << "Thread #" << i << " couter: " << j << endl;
      }
    });
  }
  for (auto& thr : threads) {
    thr.join();
  }
  return 0;
}
