#pragma once

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>

#include "db/db_iter.h"
#include "utilities/titandb/version.h"

namespace rocksdb {
namespace titandb {

// Wraps the current version together with the snapshot from base DB
// so that we can safely recycle a steal version when it is dropped.
// This also implies a guarantee that the current version must contain
// all the data accessible from base DB.
class TitanSnapshot : public Snapshot {
 public:
  TitanSnapshot(Version* _current, const Snapshot* _snapshot,
                std::map<ColumnFamilyData*, SuperVersion*>* _svs)
      : current_(_current), snapshot_(_snapshot), svs_(std::move(*_svs)) {}

  Version* current() const { return current_; }

  const Snapshot* snapshot() const { return snapshot_; }

  SuperVersion* GetSuperVersion(ColumnFamilyData* cfd) const {
    auto iter = svs_.find(cfd);
    if (iter != svs_.end()) {
      return iter->second;
    }
    return nullptr;
  }

  const std::map<ColumnFamilyData*, SuperVersion*> svs() const { return svs_; }

  SequenceNumber GetSequenceNumber() const override {
    return snapshot_->GetSequenceNumber();
  }

 private:
  Version* const current_;
  const Snapshot* const snapshot_;
  const std::map<ColumnFamilyData*, SuperVersion*> svs_;
};

class TitanDBIterator : public Iterator {
 public:
  TitanDBIterator(const ReadOptions& options, BlobStorage* storage,
                  std::shared_ptr<ManagedSnapshot> snap,
                  std::unique_ptr<ArenaWrappedDBIter> iter)
      : options_(options),
        storage_(storage),
        snap_(snap),
        iter_(std::move(iter)) {}

  bool Valid() const override { return iter_->Valid() && status_.ok(); }

  Status status() const override {
    // assume volatile inner iter
    if(status_.ok()) {
      return iter_->status();
    } else {
      return status_;
    }
  }

  void SeekToFirst() override {
    iter_->SeekToFirst();
    GetBlobValue();
  }

  void SeekToLast() override {
    iter_->SeekToLast();
    GetBlobValue();
  }

  void Seek(const Slice& target) override {
    iter_->Seek(target);
    GetBlobValue();
  }

  void SeekForPrev(const Slice& target) override {
    iter_->SeekForPrev(target);
    GetBlobValue();
  }

  void Next() override {
    assert(Valid());
    iter_->Next();
    GetBlobValue();
  }

  void Prev() override {
    assert(Valid());
    iter_->Prev();
    GetBlobValue();
  }

  Slice key() const override {
    assert(Valid());
    return iter_->key();
  }

  Slice value() const override {
    assert(Valid());
    if (!iter_->IsBlob()) return iter_->value();
    return record_.value;
  }

 private:
  void GetBlobValue() {
    if (!iter_->Valid() || !iter_->IsBlob()) {
      status_ = iter_->status();
      return;
    }
    assert(iter_->status().ok());

    BlobIndex index;
    status_ = DecodeInto(iter_->value(), &index);
    if (!status_.ok()) {
      fprintf(stderr, "GetBlobValue decode blob index err:%s\n",
              status_.ToString().c_str());
      abort();
    }

    auto it = files_.find(index.file_number);
    if (it == files_.end()) {
      std::unique_ptr<BlobFilePrefetcher> prefetcher;
      status_ = storage_->NewPrefetcher(index.file_number, &prefetcher);
      if (status_.IsCorruption()) {
        fprintf(stderr,
                "key:%s GetBlobValue err:%s with sequence number:%" PRIu64 "\n",
                iter_->key().ToString(true).c_str(), status_.ToString().c_str(),
                options_.snapshot->GetSequenceNumber());
      }
      if (!status_.ok()) return;
      it = files_.emplace(index.file_number, std::move(prefetcher)).first;
    }

    buffer_.Reset();
    status_ = it->second->Get(options_, index.blob_handle, &record_, &buffer_);
  }

  Status status_;
  BlobRecord record_;
  PinnableSlice buffer_;

  ReadOptions options_;
  BlobStorage* storage_;
  std::shared_ptr<ManagedSnapshot> snap_;
  std::unique_ptr<ArenaWrappedDBIter> iter_;
  std::map<uint64_t, std::unique_ptr<BlobFilePrefetcher>> files_;
};

}  // namespace titandb
}  // namespace rocksdb
