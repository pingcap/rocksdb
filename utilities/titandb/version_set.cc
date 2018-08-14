#include "utilities/titandb/version_set.h"

#include <inttypes.h>

#include "util/filename.h"
#include "utilities/titandb/version_builder.h"

namespace rocksdb {
namespace titandb {

const size_t kMaxFileCacheSize = 1024 * 1024;

VersionSet::VersionSet(const TitanDBOptions& options)
    : dirname_(options.dirname),
      env_(options.env),
      env_options_(options),
      db_options_(options) {
  auto file_cache_size = db_options_.max_open_files;
  if (file_cache_size < 0) {
    file_cache_size = kMaxFileCacheSize;
  }
  file_cache_ = NewLRUCache(file_cache_size);
}

Status VersionSet::Open(
    const std::map<uint32_t, TitanCFOptions>& column_families) {
  // Sets up the first version for the specified column families.
  auto v = new Version;
  for (auto& cf : column_families) {
    auto file_cache = std::make_shared<BlobFileCache>(
        db_options_, cf.second, file_cache_);
    auto storage = std::make_shared<BlobStorage>(file_cache);
    v->column_families_.emplace(cf.first, storage);
  }
  versions_.Append(v);

  Status s = env_->FileExists(CurrentFileName(dirname_));
  if (s.ok()) {
    return Recover();
  }
  if (!s.IsNotFound()) {
    return s;
  }
  if (!db_options_.create_if_missing) {
    return Status::InvalidArgument(
        dirname_, "does't exist (create_if_missing is false)");
  }
  return OpenManifest(NewFileNumber());
}

Status VersionSet::Recover() {
  struct LogReporter : public log::Reader::Reporter {
    Status* status;
    void Corruption(size_t, const Status& s) override {
      if (status->ok()) *status = s;
    }
  };

  // Reads "CURRENT" file, which contains the name of the current manifest file.
  std::string manifest;
  Status s = ReadFileToString(env_, CurrentFileName(dirname_), &manifest);
  if (!s.ok()) return s;
  if (manifest.empty() || manifest.back() != '\n') {
    return Status::Corruption("CURRENT file does not end with newline");
  }
  manifest.resize(manifest.size() - 1);

  // Opens the current manifest file.
  auto file_name = dirname_ + "/" + manifest;
  std::unique_ptr<SequentialFileReader> file;
  {
    std::unique_ptr<SequentialFile> f;
    s = env_->NewSequentialFile(file_name, &f,
                                env_->OptimizeForManifestRead(env_options_));
    if (!s.ok()) return s;
    file.reset(new SequentialFileReader(std::move(f), file_name));
  }

  bool has_next_file_number = false;
  uint64_t next_file_number = 0;

  // Reads edits from the manifest and applies them one by one.
  VersionBuilder builder(current());
  {
    LogReporter reporter;
    reporter.status = &s;
    log::Reader reader(nullptr, std::move(file), &reporter,
                       true /*checksum*/, 0 /*initial_offset*/, 0);
    Slice record;
    std::string scratch;
    while (reader.ReadRecord(&record, &scratch) && s.ok()) {
      VersionEdit edit;
      s = DecodeInto(record, &edit);
      if (!s.ok()) return s;
      builder.Apply(&edit);
      if (edit.has_next_file_number_) {
        next_file_number = edit.next_file_number_;
        has_next_file_number = true;
      }
    }
  }

  if (!has_next_file_number) {
    return Status::Corruption("no next file number in manifest file");
  }
  next_file_number_.store(next_file_number);

  auto v = new Version;
  {
    builder.SaveTo(v);
    versions_.Append(v);
  }
  return OpenManifest(NewFileNumber());
}

Status VersionSet::OpenManifest(uint64_t file_number) {
  Status s;

  auto file_name = DescriptorFileName(dirname_, file_number);
  std::unique_ptr<WritableFileWriter> file;
  {
    std::unique_ptr<WritableFile> f;
    s = env_->NewWritableFile(file_name, &f, env_options_);
    if (!s.ok()) return s;
    file.reset(new WritableFileWriter(std::move(f), env_options_));
  }

  manifest_.reset(new log::Writer(std::move(file), 0, false));

  // Saves current snapshot
  s = WriteSnapshot(manifest_.get());
  if (s.ok()) {
    ImmutableDBOptions ioptions(db_options_);
    s = SyncManifest(env_, &ioptions, manifest_->file());
  }
  if (s.ok()) {
    // Makes "CURRENT" file that points to the new manifest file.
    s = SetCurrentFile(env_, dirname_, file_number, nullptr);
  }

  if (!s.ok()) {
    manifest_.reset();
    env_->DeleteFile(file_name);
  }
  return s;
}

Status VersionSet::WriteSnapshot(log::Writer* log) {
  Status s;
  // Saves global information
  {
    VersionEdit edit;
    edit.SetNextFileNumber(next_file_number_.load());
    std::string record;
    edit.EncodeTo(&record);
    s = log->AddRecord(record);
    if (!s.ok()) return s;
  }
  // Saves column families information
  for (auto& it : current()->column_families_) {
    VersionEdit edit;
    edit.SetColumnFamilyID(it.first);
    for (auto& file : it.second->files_) {
      edit.AddBlobFile(file.second);
    }
    std::string record;
    edit.EncodeTo(&record);
    s = log->AddRecord(record);
    if (!s.ok()) return s;
  }
  return s;
}

Status VersionSet::LogAndApply(VersionEdit* edit, port::Mutex* mutex) {
  mutex->AssertHeld();

  // TODO(@huachao): write manifest file unlocked
  std::string record;
  edit->SetNextFileNumber(next_file_number_.load());
  edit->EncodeTo(&record);
  Status s = manifest_->AddRecord(record);
  if (s.ok()) {
    ImmutableDBOptions ioptions(db_options_);
    s = SyncManifest(env_, &ioptions, manifest_->file());
  }
  if (!s.ok()) return s;

  auto v = new Version;
  {
    VersionBuilder builder(current());
    builder.Apply(edit);
    builder.SaveTo(v);
    versions_.Append(v);
  }
  return s;
}

}  // namespace titandb
}  // namespace rocksdb
