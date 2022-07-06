#include <cassert>
#include <iostream>

#include "cuckoofilter.h"

using cuckoofilter::CuckooFilter;
using namespace std;

int main(int argc, char **argv) {
  size_t total_items = 1000000;

  // Create a Smart Cuckoo Filter where each item is of type size_t and use 12
  // bits for each item:
  CuckooFilter<size_t, 12> filter(total_items);

  // Insert items to this Smart Cuckoo Filter
  size_t num_inserted = 0;
  for (size_t i = 0; i < total_items; i++, num_inserted++) {
    if (filter.Add(i) != cuckoofilter::Ok) {
      break;
    }
  }

  // Check if previously inserted items are in the filter,
  // expected true for all items
  for (size_t i = 0; i < num_inserted; i++) {
    assert(filter.Contain(i) == cuckoofilter::Ok);
  }

  // Check non-existing items, a few false positives expected
  size_t total_queries = 0, false_queries = 0;
  for (size_t i = total_items; i < 2 * total_items; i++) {
    if (filter.Contain(i) == cuckoofilter::Ok) {
      false_queries++;
    }
    total_queries++;
  }

  // Output the measured false positive rate
  cout << "false positive rate is " << 100.0 * false_queries / total_queries
       << "%\n";

  return 0;
}
