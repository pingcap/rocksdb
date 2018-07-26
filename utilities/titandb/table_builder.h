#pragma once

#include "table/table_builder.h"
#include "utilities/titandb/options.h"
#include "utilities/titandb/blob_file_builder.h"
#include "utilities/titandb/blob_file_manager.h"

namespace rocksdb {
namespace titandb {

class TitanTableBuilder : public TableBuilder {
 public:
  TitanTableBuilder(const TitanDBOptions& options,
                    uint64_t column_family_id,
                    std::unique_ptr<TableBuilder> base_builder,
                    std::shared_ptr<BlobFileManager> blob_manager)
      : options_(options),
        column_family_id_(column_family_id),
        base_builder_(std::move(base_builder)),
        blob_manager_(blob_manager) {}

  void Add(const Slice& key, const Slice& value) override;

  Status status() const override;

  Status Finish() override;

  void Abandon() override;

  uint64_t NumEntries() const override;

  uint64_t FileSize() const override;

  bool NeedCompact() const override;

  TableProperties GetTableProperties() const override;

 private:
  bool ok() const { return status().ok(); }

  void AddBlob(const Slice& key, const Slice& value, std::string* index_value);

  Status status_;
  TitanDBOptions options_;
  uint64_t column_family_id_;
  std::unique_ptr<TableBuilder> base_builder_;
  std::unique_ptr<BlobFileHandle> blob_handle_;
  std::shared_ptr<BlobFileManager> blob_manager_;
  std::unique_ptr<BlobFileBuilder> blob_builder_;
};

}  // namespace titandb
}  // namespace rocksdb
