#include "buffer/buffer_manager.hpp"

BufferManager::BufferManager(bool storage_manager_truncate_mode):
    storage_manager(storage_manager_truncate_mode),
    policy(std::make_unique<LruPolicy>(MAX_PAGES_IN_MEMORY)) {
    storage_manager.extend(MAX_PAGES);
}

BufferManager::~BufferManager() {
    for (auto& pair : pageMap) {
        flushPage(pair.first);
    }
}

SlottedPage& BufferManager::fix_page(int page_id) {
    auto it = pageMap.find(page_id);
    if (it != pageMap.end()) {
        policy->touch(page_id);
        return pageMap.find(page_id)->second;
    }

    if (pageMap.size() >= MAX_PAGES_IN_MEMORY) {
        auto evictedPageId = policy->evict();
        if (evictedPageId != INVALID_VALUE) {
            storage_manager.flush(evictedPageId,
                                  pageMap[evictedPageId]);
            pageMap.erase(evictedPageId);
        }
    }

    auto page = storage_manager.load(page_id);
    policy->touch(page_id);
    pageMap[page_id] = std::move(*page);
    return pageMap[page_id];
}

void BufferManager::flushPage(int page_id) {
    storage_manager.flush(page_id, pageMap[page_id]);
}

void BufferManager::extend() {
    storage_manager.extend();
}

size_t BufferManager::getNumPages() {
    return storage_manager.num_pages;
}
