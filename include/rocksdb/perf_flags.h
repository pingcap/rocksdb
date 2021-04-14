#pragma once
#include <stdint.h>

namespace rocksdb {
struct PerfFlags {
  // represent original Level 2
  // TODO: think a better design for context_by_level structure
  uint8_t enable_perf_context_by_level_count_bit : 1;        // 2
  uint8_t enable_user_key_comparison_count_bit : 1;          // 2
  uint8_t enable_block_cache_hit_count_bit : 1;              // 2
  uint8_t enable_block_read_count_bit : 1;                   // 2
  uint8_t enable_block_read_byte_bit : 1;                    // 2
  uint8_t enable_block_cache_index_hit_count_bit : 1;        // 2
  uint8_t enable_index_block_read_count_bit : 1;             // 2
  uint8_t enable_block_cache_filter_hit_count_bit : 1;       // 2
  uint8_t enable_filter_block_read_count_bit : 1;            // 2
  uint8_t enable_compression_dict_block_read_count_bit : 1;  // 2
  uint8_t enable_get_read_bytes_bit : 1;                     // 2
  uint8_t enable_multiget_read_bytes_bit : 1;                // 2
  uint8_t enable_iter_read_bytes_bit : 1;                    // 2
  uint8_t enable_internal_key_skipped_count_bit : 1;         // 2
  uint8_t enable_internal_delete_skipped_count_bit : 1;      // 2
  uint8_t enable_internal_recent_skipped_count_bit : 1;      // 2
  uint8_t enable_internal_merge_count_bit : 1;               // 2
  uint8_t enable_get_from_memtable_count_bit : 1;            // 2
  uint8_t enable_seek_on_memtable_count_bit : 1;             // 2
  uint8_t enable_next_on_memtable_count_bit : 1;             // 2
  uint8_t enable_prev_on_memtable_count_bit : 1;             // 2
  uint8_t enable_seek_child_seek_count_bit : 1;              // 2
  uint8_t enable_bloom_memtable_hit_count_bit : 1;           // 2
  uint8_t enable_bloom_memtable_miss_count_bit : 1;          // 2
  uint8_t enable_bloom_sst_hit_count_bit : 1;                // 2
  uint8_t enable_bloom_sst_miss_count_bit : 1;               // 2
  uint8_t enable_key_lock_wait_count_bit : 1;                // 2

  // represent original Level 3
  uint8_t enable_measure_cpu_time_bit : 1;                           // 3
  uint8_t enable_block_read_time_bit : 1;                            // 3
  uint8_t enable_block_checksum_time_bit : 1;                        // 3
  uint8_t enable_block_decompress_time_bit : 1;                      // 3
  uint8_t enable_get_snapshot_time_bit : 1;                          // 3
  uint8_t enable_get_from_memtable_time_bit : 1;                     // 3
  uint8_t enable_get_post_process_time_bit : 1;                      // 3
  uint8_t enable_get_from_output_files_time_bit : 1;                 // 3
  uint8_t enable_seek_on_memtable_time_bit : 1;                      // 3
  uint8_t enable_seek_child_seek_time_bit : 1;                       // 3
  uint8_t enable_seek_min_heap_time_bit : 1;                         // 3
  uint8_t enable_seek_max_heap_time_bit : 1;                         // 3
  uint8_t enable_seek_internal_seek_time_bit : 1;                    // 3
  uint8_t enable_find_next_user_entry_time_bit : 1;                  // 3
  uint8_t enable_write_wal_time_bit : 1;                             // 3
  uint8_t enable_write_memtable_time_bit : 1;                        // 3
  uint8_t enable_write_delay_time_bit : 1;                           // 3
  uint8_t enable_write_scheduling_flushes_compactions_time_bit : 1;  // 3
  uint8_t enable_write_pre_and_post_process_time_bit : 1;            // 3
  uint8_t enable_write_thread_wait_nanos_bit : 1;                    // 3
  uint8_t enable_merge_operator_time_nanos_bit : 1;                  // 3
  uint8_t enable_read_index_block_nanos_bit : 1;                     // 3
  uint8_t enable_read_filter_block_nanos_bit : 1;                    // 3
  uint8_t enable_new_table_block_iter_nanos_bit : 1;                 // 3
  uint8_t enable_new_table_iterator_nanos_bit : 1;                   // 3
  uint8_t enable_block_seek_nanos_bit : 1;                           // 3
  uint8_t enable_find_table_nanos_bit : 1;                           // 3
  uint8_t enable_key_lock_wait_time_bit : 1;                         // 3
  uint8_t enable_env_new_sequential_file_nanos_bit : 1;              // 3
  uint8_t enable_env_new_random_access_file_nanos_bit : 1;           // 3
  uint8_t enable_env_new_writable_file_nanos_bit : 1;                // 3
  uint8_t enable_env_reuse_writable_file_nanos_bit : 1;              // 3
  uint8_t enable_env_new_random_rw_file_nanos_bit : 1;               // 3
  uint8_t enable_env_new_directory_nanos_bit : 1;                    // 3
  uint8_t enable_env_file_exists_nanos_bit : 1;                      // 3
  uint8_t enable_env_get_children_nanos_bit : 1;                     // 3
  uint8_t enable_env_get_children_file_attributes_nanos_bit : 1;     // 3
  uint8_t enable_env_delete_file_nanos_bit : 1;                      // 3
  uint8_t enable_env_create_dir_nanos_bit : 1;                       // 3
  uint8_t enable_env_create_dir_if_missing_nanos_bit : 1;            // 3
  uint8_t enable_env_delete_dir_nanos_bit : 1;                       // 3
  uint8_t enable_env_get_file_size_nanos_bit : 1;                    // 3
  uint8_t enable_env_get_file_modification_time_nanos_bit : 1;       // 3
  uint8_t enable_env_rename_file_nanos_bit : 1;                      // 3
  uint8_t enable_env_link_file_nanos_bit : 1;                        // 3
  uint8_t enable_env_lock_file_nanos_bit : 1;                        // 3
  uint8_t enable_env_unlock_file_nanos_bit : 1;                      // 3
  uint8_t enable_env_new_logger_nanos_bit : 1;                       // 3
  uint8_t enable_encrypt_data_nanos_bit : 1;                         // 3
  uint8_t enable_decrypt_data_nanos_bit : 1;                         // 3

  // represent original Level 4
  // TODO: think a better design for iostats
  uint8_t enable_iostats_cpu_timer_bit : 1;    // 4
  uint8_t enable_get_cpu_nanos_bit : 1;        // 4
  uint8_t enable_iter_next_cpu_nanos_bit : 1;  // 4
  uint8_t enable_iter_prev_cpu_nanos_bit : 1;  // 4
  uint8_t enable_iter_seek_cpu_nanos_bit : 1;  // 4

  // represent original Level 5
  uint8_t enable_db_mutex_lock_nanos_bit : 1;      // 5
  uint8_t enable_db_condition_wait_nanos_bit : 1;  // 5
};
extern const  PerfFlags PERF_LEVEL2;
extern const  PerfFlags PERF_LEVEL3;
extern const  PerfFlags PERF_LEVEL4;
extern const  PerfFlags PERF_LEVEL5;
// set the perf flags for current thread
void SetPerfFlags(PerfFlags pbf);

// get current perf flags for current thread
PerfFlags GetPerfFlags();

}  // namespace rocksdb
