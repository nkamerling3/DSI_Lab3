#pragma once

#include <algorithm>
#include <optional>
#include <vector>
#include <memory>
#include <random>
#include <numeric>
#include "common/constants.hpp"
#include "buffer/buffer_manager.hpp"

// =============================================================================
// BTree - DO NOT MODIFY this header
// =============================================================================

template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
class BTree
{
public:
    // =========================================================================
    // Node base struct — fully defined here (invariant)
    // =========================================================================
    struct Node {
        uint64_t parent_node_id = INVALID_VALUE;
        uint64_t node_id = INVALID_VALUE;
        uint16_t level;
        uint16_t count;

        Node(uint16_t level = 0, uint16_t count = 0)
            : level(level), count(count) {}
        bool is_leaf() const { return level == 0; }
    };

    // =========================================================================
    // InnerNode / LeafNode — forward-declared here, defined in btree.cpp
    // =========================================================================
    struct InnerNode;
    struct LeafNode;

    // =========================================================================
    // BTree members (invariant)
    // =========================================================================
    std::optional<uint64_t> root;
    BufferManager& buffer_manager;
    uint64_t next_page_id;

    // =========================================================================
    // Public API
    // =========================================================================
    explicit BTree(BufferManager& bm);

    std::optional<ValueT> lookup(const KeyT& key);
    std::vector<std::pair<KeyT, ValueT>> rangeQuery(const KeyT& low, const KeyT& high);
    void erase(const KeyT& key);
    void insert(const KeyT& key, const ValueT& value);
    void printTree();

private:
    // =========================================================================
    // getNode helper (invariant)
    // =========================================================================
    std::shared_ptr<Node> getNode(uint64_t page_id) {
        SlottedPage* page = &buffer_manager.fix_page(page_id);
        return std::shared_ptr<Node>(
            reinterpret_cast<Node*>(page->page_data.get()),
            [](Node*) {});
    }

    void splitNode(std::vector<std::shared_ptr<Node>> path,
                   std::shared_ptr<Node> node);

    void printNode(std::shared_ptr<Node> node, int depth);
};
