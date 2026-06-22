#pragma once

#include <unordered_map>
#include <memory>
#include "storage/storage_manager.hpp"
#include "buffer/policy.hpp"

// =============================================================================
// BufferManager
// =============================================================================

class BufferManager
{
public:
    static constexpr size_t MAX_PAGES = 1000;
    static constexpr size_t MAX_PAGES_IN_MEMORY = 10;

private:
    using PageMap = std::unordered_map<PageID, SlottedPage>;

    StorageManager storage_manager;
    PageMap pageMap;
    std::unique_ptr<Policy> policy;

public:
    BufferManager(bool storage_manager_truncate_mode = true);
    ~BufferManager();

    SlottedPage& fix_page(int page_id);
    void flushPage(int page_id);
    void extend();
    size_t getNumPages();
};
