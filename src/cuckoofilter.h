#ifndef CUCKOO_FILTER_CUCKOO_FILTER_H_
#define CUCKOO_FILTER_CUCKOO_FILTER_H_

#include "debug.h"
#include "hashutil.h"
#include "printutil.h"
#include "smartcuckootable.h"

#include <cassert>
#include <iostream>
#include <sstream>

namespace cuckoofilter {
enum Status {
  Ok = 0,
  NotFound = 1,
  NotEnoughSpace = 2,
  NotSupported = 3,
};

template <typename ItemType, size_t bits_per_item,
          typename HashFamily = TwoIndependentMultiplyShift>
class CuckooFilter {
  SmartCuckooTable<bits_per_item> *table_;

  size_t num_items_;

  typedef struct {
    size_t index;
    uint32_t tag;
    bool used;
  } VictimCache;

  VictimCache victim_;

  HashFamily hasher_;

  inline size_t IndexHash(uint32_t hv) const {
    return hv & (table_->NumBuckets() - 1);
  }

  inline uint32_t TagHash(uint32_t hv) const {
    uint32_t tag;
    tag = hv & ((1ULL << bits_per_item) - 1);
    tag += (tag == 0);
    return tag;
  }

  inline void GenerateIndexTagHash(const ItemType &item, size_t *index,
                                   uint32_t *tag) const {
    const uint64_t hash = hasher_(item);
    *index = IndexHash(hash >> 32);
    *tag = TagHash(hash);
  }

  inline size_t AltIndex(const size_t index, const uint32_t tag) const {
    // return IndexHash((uint32_t)(index ^ (tag * 0x5bd1e995)));
    uint32_t t = tag * 0x5bd1e995;
    t &= table_->NumBuckets() - 1;
    return index ^ t;
  }

  inline Status AddImpl(const size_t i, const uint32_t tag) {
    size_t curindex = i;
    uint32_t curtag = tag;
    size_t canindex = AltIndex(curindex, curtag);

    if (table_->InsertTagToBucket(curindex, canindex, curtag)) {
      num_items_++;
      return Ok;
    }

    victim_.index = curindex;
    victim_.tag = curtag;
    victim_.used = true;
    return Ok;
  }

  double LoadFactor() const { return 1.0 * Size() / table_->NumBuckets(); }

  size_t BitsPerItem() const { return 8.0 * table_->SizeInBytes() / Size(); }

 public:
  explicit CuckooFilter(const size_t max_num_keys)
      : num_items_(0), victim_(), hasher_() {
    size_t num_buckets = upperpower2(max_num_keys);

    if ((double)max_num_keys / num_buckets > 0.96) {
      num_buckets <<= 1;
    }
    victim_.used = false;
    table_ = new SmartCuckooTable<bits_per_item>(num_buckets);
  }

  ~CuckooFilter() { delete table_; }

  Status Add(const ItemType &item);

  Status Contain(const ItemType &item) const;

  std::string Info() const;

  size_t Size() const { return num_items_; }

  size_t SizeInBytes() const { return table_->SizeInBytes(); }
};

template <typename ItemType, size_t bits_per_item, typename HashFamily>
Status CuckooFilter<ItemType, bits_per_item, HashFamily>::Add(
    const ItemType &item) {
  size_t i;
  uint32_t tag;

  if (victim_.used) {
    return NotEnoughSpace;
  }

  GenerateIndexTagHash(item, &i, &tag);
  return AddImpl(i, tag);
}

template <typename ItemType, size_t bits_per_item, typename HashFamily>
Status CuckooFilter<ItemType, bits_per_item, HashFamily>::Contain(
    const ItemType &key) const {
  size_t i1, i2;
  uint32_t tag;

  GenerateIndexTagHash(key, &i1, &tag);
  i2 = AltIndex(i1, tag);

  assert(i1 == AltIndex(i2, tag));

  if ((victim_.used && tag == victim_.tag &&
       (i1 == victim_.index || i2 == victim_.index)) ||
      table_->FindTagInBuckets(i1, i2, tag)) {
    return Ok;
  } else {
    return NotFound;
  }
}

template <typename ItemType, size_t bits_per_item, typename HashFamily>
std::string CuckooFilter<ItemType, bits_per_item, HashFamily>::Info() const {
  std::stringstream ss;

  ss << "CuckooFilter Status:\n"
     << "\t\t" << table_->Info() << "\n"
     << "\t\tKeys stored: " << Size() << "\n"
     << "\t\tLoad factor: " << LoadFactor() << "\n"
     << "\t\tHashtable size: " << (table_->SizeInBytes() >> 10) << " KB\n";

  return ss.str();
}

}  // namespace cuckoofilter

#endif  // CUCKOO_FILTER_CUCKOO_FILTER_H_
