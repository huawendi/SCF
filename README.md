Smart Cuckoo Filter
============

Overview
--------

Smart Cuckoo filter(SCF) is a variant of Cuckoo filter with one slot in a bucket. SCF use the idea of disjoint-set to solve the endless loop problem. Quickly determine whether an element will fall into an endless loop before inserting it.

For details about the algorithm and citations please use:

["A Smart Cuckoo Filter with Disjoint-set to Detect the Endless loop"](https://www.raitconf.info/index.html) in proceedings of GCRAIT 2022 by Wendi Hua and Ping Xie

API
--------

A Smart Cuckoo Filter supports following operations:

* `Add(item)`: insert an item to the filter
* `Contain(item)`: return if item is already in the filter. Note that this method may return false positive results like Cuckoo filters
* `Delete(item)`: delete the given item from the filter. Note that to use this method, it must be ensured that this item is in the filter (e.g., based on records on external storage); otherwise, a false item may be deleted.
* `Size()`: return the total number of items currently in the filter
* `SizeInBytes()`: return the filter size in bytes

Here is a simple example in C++ for the basic usage of Smart Cuckoo Filter.
More examples can be found in `example/` directory.

```cpp
// Create a Smart Cuckoo Filter where each item is of type size_t and
// use 12 bits for each item, with capacity of total_items
CuckooFilter<size_t, 12> filter(total_items);
// Insert item 12 to this Smart Cuckoo Filter
filter.Add(12);
// Check if previously inserted items are in the filter
assert(filter.Contain(12) == cuckoofilter::Ok);
```

Repository structure
--------------------

* `src/`: the C++ header and implementation of Smart Cuckoo Filter
* `example/test.cc`: an example of using Smart Cuckoo Filter

Build
-------

This libray depends on openssl library. Note that on MacOS 10.12, the header
files of openssl are not available by default. It may require to install openssl
and pass the path to `lib` and `include` directories to gcc, for example:

```bash
$ brew install openssl
# Replace 1.0.2j with the actual version of the openssl installed
$ export LDFLAGS="-L/usr/local/Cellar/openssl/1.0.2j/lib"
$ export CFLAGS="-I/usr/local/Cellar/openssl/1.0.2j/include"
```

To build the example (`example/test.cc`):

```bash
make test
```
