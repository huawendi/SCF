#ifndef CUCKOO_FILTER_SMART_CUCKOO_TABLE_H_
#define CUCKOO_FILTER_SMART_CUCKOO_TABLE_H_

#include "bitsutil.h"
#include "debug.h"
#include "printutil.h"

#include <sstream>

namespace cuckoofilter {

template <size_t bits_per_tag>
class SmartCuckooTable {
  static const size_t kBucketBitSize = 32 - 1 - bits_per_tag;
  static const uint32_t kBucketUpperBound = (1 << kBucketBitSize) - 1;
  struct Bucket {
    bool marked : 1;
    uint16_t fingerprint : bits_per_tag;
    uint32_t father : kBucketBitSize;

    Bucket() {
      marked = false;
      father = kBucketUpperBound;
      fingerprint = 0;
    }
  };

  Bucket *buckets_;
  size_t num_buckets_;

  // inline bool MaxmialSubgraph(Bucket &b);

  inline bool MaxmialSubgraphInsert(const size_t cur, const size_t can,
                                    const uint32_t tag);

  // bool Insert(Bucket *, uint16_t, uint16_t, uint32_t &);

 public:
  explicit SmartCuckooTable(const uint32_t &num) : num_buckets_(num) {
    buckets_ = new Bucket[num_buckets_];
  }

  ~SmartCuckooTable() { delete[] buckets_; };

  size_t NumBuckets() const { return num_buckets_; };

  size_t SizeInBytes() const { return sizeof(Bucket) * num_buckets_; }

  std::string Info() const;

  inline bool FindTagInBuckets(const size_t i1, const size_t i2,
                               const uint32_t tag) const;

  inline bool InsertTagToBucket(const size_t i1, const size_t i2,
                                const uint32_t tag);
};

// template <size_t bits_per_tag>
// bool SmartCuckooTable<bits_per_tag>::MaxmialSubgraph(Bucket &b) {
//   if (b.marked) return true;

//   if (b.father != kBucketUpperBound) {
//     b.marked = true;

//     if (MaxmialSubgraph(buckets_[b.father])) return true;
//   }

//   return b.marked = false;
// }

template <size_t bits_per_tag>
bool SmartCuckooTable<bits_per_tag>::MaxmialSubgraphInsert(const size_t cur,
                                                           const size_t can,
                                                           const uint32_t tag) {
  Bucket *b = &buckets_[cur];

  if (b->marked) return false;

  if (b->father != kBucketUpperBound) {
    b->marked = true;

    if (!MaxmialSubgraphInsert(b->father, cur, b->fingerprint))
      return false;
    else
      b->marked = false;
  }

  b->fingerprint = tag;
  b->father = can;
  return true;
}

template <size_t bits_per_tag>
std::string SmartCuckooTable<bits_per_tag>::Info() const {
  std::stringstream ss;

  ss << "Total # of rows: " << num_buckets_ << "\n";

  return ss.str();
}

template <size_t bits_per_tag>
inline bool SmartCuckooTable<bits_per_tag>::FindTagInBuckets(
    const size_t i1, const size_t i2, const uint32_t tag) const {
  return buckets_[i1].fingerprint == tag || buckets_[i2].fingerprint == tag;
}

// template <size_t bits_per_tag>
// inline bool SmartCuckooTable<bits_per_tag>::Insert(Bucket *b, uint16_t i1,
//                                                    uint16_t i2, uint32_t
//                                                    &tag) {
//   while (true) {
//     uint32_t temp;

//     if (b->fingerprint == 0) {  // bucket 为空
//       b->fingerprint = tag;
//       b->father = i2;

//       return true;
//     } else {
//       // 踢出元素
//       temp = b->fingerprint;
//       b->fingerprint = tag;
//       tag = temp;

//       temp = i1;
//       i1 = b->father;
//       b->father = i2;

//       i2 = temp;
//       b = &buckets_[i1];
//     }
//   }

//   return false;
// }

template <size_t bits_per_tag>
bool SmartCuckooTable<bits_per_tag>::InsertTagToBucket(const size_t i1,
                                                       const size_t i2,
                                                       const uint32_t tag) {
  // Bucket *b1 = &buckets_[i1];
  // bool i1_is_max = MaxmialSubgraph(*b1);
  // if (!i1_is_max) return Insert(b1, i1, i2, tag);
  if (MaxmialSubgraphInsert(i1, i2, tag)) return true;

  // Bucket *b2 = &buckets_[i2];
  // bool i2_is_max = MaxmialSubgraph(*b2);
  // if (!i2_is_max) return Insert(b2, i2, i1, tag);
  if (MaxmialSubgraphInsert(i2, i1, tag)) return true;

  return false;
}

}  // namespace cuckoofilter

#endif  // CUCKOO_FILTER_SMART_CUCKOO_TABLE_H_
