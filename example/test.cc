#include "cuckoofilter.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>

using cuckoofilter::CuckooFilter;
using namespace std;

void random_gen(const size_t n, vector<size_t> &store, mt19937 &rd) {
  store.resize(n);
  for (size_t i = 0; i < n; i++) {
    store[i] = (size_t(rd()) << 32) + rd();
  }
}

int main(int argc, char **argv) {
  auto start = chrono::steady_clock::now();
  auto end = chrono::steady_clock::now();

  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  mt19937 rd(seed);

  vector<size_t> keys, aliens;
  random_gen(1 << 19, keys, rd);
  random_gen(1 << 19, aliens, rd);

  for (size_t length = (1 << 9); length < (1 << 19); length <<= 1) {
    // for (size_t length = 12; length <= (1 << 19); ) {
    const size_t items = length * 0.95;
    CuckooFilter<size_t, 12> filter(items);

    cout << "length is " << items << "\n";
    cout << "size of the filter in bytes is "
         << (double)filter.SizeInBytes() / 1024 / 1024 << " MB\n";

    size_t num_inserted = 0;

    // 插入操作吞吐率
    chrono::duration<double> add_time;
    start = chrono::steady_clock::now();
    for (size_t i = 0; i < items; i++, num_inserted++) {
      // if (filter.Add(i) != cuckoofilter::Ok) {
      if (filter.Add(keys[i]) != cuckoofilter::Ok) {
        break;
      }
    }
    end = chrono::steady_clock::now();
    add_time = end - start;

    cout << "inserted length is " << num_inserted << "\n";
    cout << "\033[33mthroughput of adding is "
         << (double)num_inserted / add_time.count() / 1e+6 << " MOPS\n\033[0m";

    // 查找操作吞吐率
    // chrono::duration<double> find_time;
    // start = chrono::steady_clock::now();
    for (size_t i = 0; i < num_inserted; i++) {
      // assert(filter.Contain(i) == cuckoofilter::Ok);
      assert(filter.Contain(keys[i]) == cuckoofilter::Ok);
    }
    // end = chrono::steady_clock::now();
    // find_time = end - start;

    // cout << "\033[33mthroughput of finding is "
    //  << (double)num_inserted / find_time.count() / 1e+6 << " MOPS\n\033[0m";

    cout << "\n";
  }

  return 0;
}
