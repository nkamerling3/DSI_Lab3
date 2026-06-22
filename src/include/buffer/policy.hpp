#pragma once

#include <list>
#include <unordered_map>
#include <iostream>
#include <string>
#include "common/constants.hpp"

// =============================================================================
// Eviction Policy - DO NOT MODIFY
// =============================================================================

class Policy
{
public:
    virtual bool touch(PageID page_id) = 0;
    virtual PageID evict() = 0;
    virtual ~Policy() = default;
};

inline void printList(std::string list_name, const std::list<PageID>& myList) {
    std::cout << list_name << " :: ";
    for (const PageID& value : myList) {
        std::cout << value << ' ';
    }
    std::cout << '\n';
}

class LruPolicy : public Policy
{
private:
    std::list<PageID> lruList;
    std::unordered_map<PageID, std::list<PageID>::iterator> map;
    size_t cacheSize;

public:
    LruPolicy(size_t cacheSize) : cacheSize(cacheSize) {}

    bool touch(PageID page_id) override {
        bool found = false;
        if (map.find(page_id) != map.end()) {
            found = true;
            lruList.erase(map[page_id]);
            map.erase(page_id);
        }

        if (lruList.size() == cacheSize) {
            evict();
        }

        if (lruList.size() < cacheSize) {
            lruList.emplace_front(page_id);
            map[page_id] = lruList.begin();
        }

        return found;
    }

    PageID evict() override {
        PageID evictedPageId = INVALID_VALUE;
        if (lruList.size() != 0) {
            evictedPageId = lruList.back();
            map.erase(evictedPageId);
            lruList.pop_back();
        }
        return evictedPageId;
    }
};
