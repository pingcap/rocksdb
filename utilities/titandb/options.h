#pragma once

#include "rocksdb/options.h"

namespace rocksdb {
namespace titandb {

struct TitanDBOptions : public DBOptions {
  // The directory to store data specific to TitanDB alongside with
  // the base DB.
  //
  // Default: {dbname}/titandb
  std::string dirname;

  // Maximum number of concurrent background GC jobs.
  //
  // Default: 4
  int max_background_gc{4};

  // Disable background GC
  //
  // Default: true
  bool disable_background_gc{false};

  TitanDBOptions() = default;
  explicit TitanDBOptions(const DBOptions& options) : DBOptions(options) {}
};

struct TitanCFOptions : public ColumnFamilyOptions {
  // The smallest value to store in blob files. Value smaller than
  // this threshold will be inlined in base DB.
  //
  // Default: 4096
  uint64_t min_blob_size{4096};

  // The compression algorithm used to compress data in blob files.
  //
  // Default: kNoCompression
  CompressionType blob_file_compression{kNoCompression};

  // The desirable blob file size. This is not a hard limit but a wish.
  //
  // Default: 256MB
  uint64_t blob_file_target_size{256 << 20};

  // If non-NULL use the specified cache for blob records.
  //
  // Default: nullptr
  std::shared_ptr<Cache> blob_cache;

  // Max batch size for gc
  //
  // Default: 1GB
  uint64_t max_gc_batch_size{1 << 30};

  // Min batch size for gc
  //
  // Default: 512MB
  uint64_t min_gc_batch_size{512 << 20};

  // The ratio of how much discardable size of a blob file can be GC
  //
  // Default: 0.5
  float blob_file_discardable_ratio{0.5};

  // The ratio of how much size of a blob file need to be sample before GC
  //
  // Default: 0.1
  float sample_flie_size_ratio{0.1};

  // The blob file size less than this option will be mark gc
  //
  // Default: 8MB
  uint64_t merge_small_file_threashold{8 << 20};

  TitanCFOptions() = default;
  explicit TitanCFOptions(const ColumnFamilyOptions& options)
      : ColumnFamilyOptions(options) {}

  std::string ToString() const;
};

struct TitanOptions : public TitanDBOptions, public TitanCFOptions {};

}  // namespace titandb
}  // namespace rocksdb
