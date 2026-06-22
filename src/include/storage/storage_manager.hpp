#pragma once

#include <mutex>
#include <string>
#include <memory>
#include <atomic>
#include <iostream>
#include <cstring>
#include <cassert>
#include "storage/slotted_page.hpp"

// =============================================================================
// StorageManager
// =============================================================================

const std::string database_filename = "buzzdb.dat";

class StorageManager {
public:
    int fd_;
    std::atomic<size_t> num_pages{0};
    std::mutex io_mutex;

    StorageManager(bool truncate_mode = true);
    ~StorageManager();

    std::unique_ptr<SlottedPage> load(uint16_t page_id);
    void flush(uint16_t page_id, const SlottedPage& page);
    void extend();
    void extend(uint64_t till_page_id);
};
