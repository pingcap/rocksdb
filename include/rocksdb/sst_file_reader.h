// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
#pragma once
#ifndef ROCKSDB_LITE

#include <inttypes.h>
#include <memory>
#include <string>
#include <vector>

#include "rocksdb/env.h"
#include "rocksdb/options.h"
#include "rocksdb/table_properties.h"
#include "rocksdb/types.h"

namespace rocksdb {

struct ImmutableCFOptions;
class RandomAccessFileReader;
class InternalKeyComparator;
class TableReader;
struct TableBuilderOptions;

class SstFileReader {
 public:
  explicit SstFileReader(
      const std::string& file_name, bool verify_checksum,
      std::function<void(const Slice&, const Slice&, SequenceNumber,
                         unsigned char)>
          kv_handler = nullptr,
      std::function<void(const std::string&)> info_handler = nullptr,
      std::function<void(const std::string&)> err_handler = nullptr);

  Status ReadSequential(uint64_t read_num, bool has_from,
                        const std::string& from_key, bool has_to,
                        const std::string& to_key,
                        bool use_from_as_prefix = false);

  Status ReadTableProperties(
      std::shared_ptr<const TableProperties>* table_properties);
  uint64_t GetReadNumber() { return read_num_; }
  TableProperties* GetInitTableProperties() { return table_properties_.get(); }

  Status VerifyChecksum();
  Status DumpTable(const std::string& out_filename);
  Status getStatus() { return init_result_; }

  int ShowAllCompressionSizes(size_t block_size,
    const std::vector<std::pair<CompressionType, const char*>> &compression_types);

  ~SstFileReader();

 private:
  // Get the TableReader implementation for the sst file
  Status GetTableReader(const std::string& file_path);
  Status ReadTableProperties(uint64_t table_magic_number,
                             RandomAccessFileReader* file, uint64_t file_size);

  uint64_t CalculateCompressedTableSize(const TableBuilderOptions& tb_options,
                                        size_t block_size);

  Status SetTableOptionsByMagicNumber(uint64_t table_magic_number);
  Status SetOldTableOptions();

  // Helper function to call the factory with settings specific to the
  // factory implementation
  Status NewTableReader(const ImmutableCFOptions& ioptions,
                        const EnvOptions& soptions,
                        const InternalKeyComparator& internal_comparator,
                        uint64_t file_size,
                        std::unique_ptr<TableReader>* table_reader);

  std::string file_name_;
  uint64_t read_num_;
  bool verify_checksum_;
  EnvOptions soptions_;
  std::function<void(const Slice&, const Slice&, SequenceNumber, unsigned char)>
      kv_handler_;
  std::function<void(const std::string&)> info_handler_;
  std::function<void(const std::string&)> err_handler_;

  // options_ and internal_comparator_ will also be used in
  // ReadSequential internally (specifically, seek-related operations)
  Options options_;

  Status init_result_;
  std::unique_ptr<TableReader> table_reader_;
  std::unique_ptr<RandomAccessFileReader> file_;

  const ImmutableCFOptions *ioptions_;
  InternalKeyComparator *internal_comparator_;
  std::unique_ptr<TableProperties> table_properties_;
};

}  // namespace rocksdb

#endif  // ROCKSDB_LITE