#include "btree/btree.hpp"
#include <stdexcept>

// =============================================================================
// InnerNode definition
// =============================================================================

template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
struct BTree<KeyT, ValueT, ComparatorT, PageSize>::InnerNode : public Node
{
    /// TODO: Calculate the maximum capacity of this node.
    /// HINT: Use the template parameter PageSize and sizeof() arithmetic
    /// to determine how many entries can fit in a single page.
    static constexpr uint32_t kCapacity = (PageSize - sizeof(Node) + sizeof(ValueT)) / (sizeof(KeyT) + sizeof(ValueT)); // fill in your own here

    // DO NOT MODIFY — InnerNode layout
    KeyT keys[kCapacity - 1];
    uint64_t children[kCapacity];

    InnerNode() : Node(1, 0) {}

    /// Get the index of the child node which could have the provided key.
    /// @param[in] key          The key that should be searched.
    size_t find_child_index(const KeyT &key) const
    {
        // TODO: Implement this function and remove UNUSED(...) calls.
        ComparatorT comparator;
        // find the position of the idx using binary search
        auto it = std::upper_bound(keys, keys + this->count, key, comparator);
        size_t idx = it - keys;

        return idx;
    }

    /// Insert a separator key and a child pointer.
    /// @param[in] key          The separator that should be inserted.
    /// @param[in] child_id     The id of the child page that should be inserted.
    void insert(const KeyT &key, uint64_t child_id)
    {
        // TODO: Implement this function and remove UNUSED(...) calls.
        UNUSED(key);
        UNUSED(child_id);

        if (this->count >= kCapacity - 1)
        {
            std::cout << "insert: inner node already at capacity!" << std::endl;
            return;
        }

        size_t child_idx = find_child_index(key);

        // TODO: hanlde scenario if key already exists, don't have to update count

        // shift key and child arrays
        for (size_t i = this->count; i > child_idx; i--)
        {
            keys[i] = keys[i - 1];
            children[i + 1] = children[i];
        }
        // Implementation may be incorrect, need to fix
        keys[child_idx] = key;
        children[child_idx + 1] = child_id;
        this->count++;
    }
};

// =============================================================================
// LeafNode definition
// =============================================================================

template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
struct BTree<KeyT, ValueT, ComparatorT, PageSize>::LeafNode : public Node
{
    /// TODO: Calculate the maximum capacity of this node.
    /// HINT: Use the template parameter PageSize and sizeof() arithmetic
    /// to determine how many entries can fit in a single page.
    static constexpr uint32_t kCapacity = (PageSize - sizeof(Node)) / (sizeof(KeyT) + sizeof(ValueT)); // fill in your own here

    // DO NOT MODIFY — LeafNode layout
    KeyT keys[kCapacity];
    ValueT values[kCapacity];
    uint64_t next = INVALID_VALUE;

    LeafNode() : Node(0, 0) {}

    /// Insert a key-value pair.
    /// @param[in] key          The key that should be inserted.
    /// @param[in] value        The value that should be inserted.
    void insert(const KeyT &key, const ValueT &value)
    {
        ComparatorT comparator;

        // TODO: Implement this function and remove UNUSED(...) calls.

        if (this->count >= kCapacity)
        {
            std::cout << "insert: leaf node already at capacity!" << std::endl;
            return;
        }

        // if count is 0
        if (this->count == 0)
        {
            keys[0] = key;
            values[0] = value;
            this->count++;
        }

        // find key position with binary search
        auto it = std::lower_bound(keys, keys + this->count, key, comparator);
        size_t idx = it - keys;
        if (!comparator(key, keys[idx]))
        {
            std::cout << "insert: key updated at pos " << idx << std::endl;

            keys[idx] = key;
            values[idx] = value;
            return;
        }

        for (size_t i = this->count; i > idx; i--)
        {
            keys[i] = keys[i - 1];
            values[i] = values[i - 1];
        }
        std::cout << "insert: key inserted at pos " << idx << std::endl;
        keys[idx] = key;
        values[idx] = value;
        this->count++;

        UNUSED(value);
    }

    /// Erase a key.
    /// NOTE: For this assignment, we do not expect you to implement tree rebalancing
    /// or node merging. A full B-Tree implementation would normally check for
    /// underflow (e.g., node capacity dropping below half) and rebalance.
    /// Here, you just need to remove the entry if it exists.
    void erase(const KeyT &key)
    {
        // TODO: Implement this function and remove UNUSED(...) calls.
        ComparatorT comparator;

        // find key position with binary search
        auto it = std::lower_bound(keys, keys + this->count, key, comparator);
        size_t idx = it - keys;

        // key not found
        if (idx == this->count)
        {
            std::cout << "erase: leaf key not found!" << std::endl;
            return;
        }
        else if (comparator(key, key[idx]))
        {
            std::cout << "erase: leaf key not found!" << std::endl;
            return;
        }

        std::cout << "erase: leaf key match found!" << std::endl;
        for (size_t i = idx; i < this->count - 1; i++)
        {
            keys[i] = keys[i + 1];
            values[i] = values[i + 1];
        }
        this->count--;
    }
};

// =============================================================================
// BTree method implementations
// =============================================================================
static constexpr uint64_t containsRootTag = 123789;
static constexpr uint64_t metaDataPage = 0;
struct MetaData
{
    uint64_t rootTag;
    uint64_t next_page_num;
};

/// Constructor.
template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
BTree<KeyT, ValueT, ComparatorT, PageSize>::BTree(BufferManager &bm)
    : buffer_manager(bm)
{
    // TODO
    // (Hint: You need a persistent Btree, so your reconstruction logic should
    //  trace parent relationships upward consistently.)

    // get page 0
    SlottedPage &initPage = bm.fix_page(metaDataPage);

    // check if page 0 contains tag and metadata
    MetaData *meta = reinterpret_cast<MetaData *>(initPage.page_data.get());
    if (meta->rootTag == containsRootTag)
    {
        // TODO:: Fix persistence
        // root = meta->root_page;

        // 1) get next_page_id from persistence and set it
        // 2) last known node is next_page_id - 1
        // 3) trace this node all the way back up to root
        // 4) set root
    }
    else
    {
        //
        // SlottedPage &rootPage = bm.fix_page(1);
        // char* rootPtr = rootPage.page_data.get();
        // InnerNode* innerRootPtr = new (rootPtr) InnerNode();

        // meta->isInitialized = initializedTag;
        // meta->root_page = 1;
        std::cout << "BTREE not yet initialized" << std::endl;
    }

    // if yes then get the root node

    // if no then create the root node and initialize

    next_page_id = 1;
}

/// Lookup an entry in the tree.
/// @param[in] key      The key that should be searched.
template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
std::optional<ValueT>
BTree<KeyT, ValueT, ComparatorT, PageSize>::lookup(const KeyT &key)
{
    // TODO
    UNUSED(key);
    throw std::logic_error("BTree::lookup is not implemented");
}

/// Returns the range of values between low and high, both inclusive.
/// @param[in] low The low key from which range starts
/// @param[in] high The high key where the range ends
template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
std::vector<std::pair<KeyT, ValueT>>
BTree<KeyT, ValueT, ComparatorT, PageSize>::rangeQuery(const KeyT &low, const KeyT &high)
{
    // TODO
    UNUSED(low);
    UNUSED(high);
    throw std::logic_error("BTree::rangeQuery is not implemented");
}

/// Erase an entry in the tree.
/// @param[in] key      The key that should be erased.
template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
void BTree<KeyT, ValueT, ComparatorT, PageSize>::erase(const KeyT &key)
{
    // TODO
    // NOTE: For this assignment, we do not expect you to implement tree rebalancing
    // or node merging. A full B-Tree implementation would normally check for
    // underflow (e.g., node capacity dropping below half) and rebalance.
    // Here, you just need to remove the entry if it exists.
    UNUSED(key);
    throw std::logic_error("BTree::erase is not implemented");
}

/// Inserts a new entry into the tree.
/// @param[in] key      The key that should be inserted.
/// @param[in] value    The value that should be inserted.
template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
void BTree<KeyT, ValueT, ComparatorT, PageSize>::insert(const KeyT &key, const ValueT &value)
{
    // TODO
    // initial Insert, get page 1, initialize metadata on page 0, create leaf node
    if (!root.has_value())
    {
        root = next_page_id;
        SlottedPage &page = buffer_manager.fix_page(next_page_id);
        auto leaf = new (page.page_data.get()) LeafNode();
        leaf->node_id = next_page_id;
        leaf->insert(key, value);

        next_page_id++;
        // write next page ID to page 0
        SlottedPage &metaPage = this->buffer_manager.fix_page(0);
        // check if page 0 contains tag and metadata
        MetaData *meta = reinterpret_cast<MetaData *>(metaPage.page_data.get());
        meta->rootTag = containsRootTag;
        meta->next_page_num = next_page_id;
        return;
    }

    // root has value path:

    UNUSED(key);
    UNUSED(value);
    throw std::logic_error("BTree::insert is not implemented");
}

template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
void BTree<KeyT, ValueT, ComparatorT, PageSize>::splitNode(
    std::vector<std::shared_ptr<Node>> path,
    std::shared_ptr<Node> node)
{
    // Call this from insert(...).
    // HINT: Split a full node into two, redistribute keys (and children/values),
    // and propagate the key upward to the parent. Handle both leaf and inner
    // cases here. Root might require special handling.
    // NOTE: We do not allow overfilling! You must perform the split
    // so you never exceed the node's maximum capacity during insertion.
    // TODO: Implement this function and remove UNUSED(...) calls.
    UNUSED(path);
    UNUSED(node);
    throw std::logic_error("BTree::splitNode is not implemented");
}

template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
void BTree<KeyT, ValueT, ComparatorT, PageSize>::printTree()
{
    if (!root)
    {
        std::cout << "(empty tree)\n";
        return;
    }
    printNode(getNode(*root), 0);
}

template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
void BTree<KeyT, ValueT, ComparatorT, PageSize>::printNode(
    std::shared_ptr<Node> node, int depth)
{
    const std::string indent(depth * 4, ' ');

    if (node->is_leaf())
    {
        auto leaf = std::static_pointer_cast<LeafNode>(node);
        std::cout << indent << "LeafNode (id=" << leaf->node_id
                  << ", count=" << leaf->count << "): ";
        for (size_t i = 0; i < leaf->count; i++)
        {
            std::cout << "[" << leaf->keys[i] << "->" << leaf->values[i] << "] ";
        }
        std::cout << "\n";
    }
    else
    {
        auto inner = std::static_pointer_cast<InnerNode>(node);
        std::cout << indent << "InnerNode (id=" << inner->node_id
                  << ", count=" << inner->count << "): ";
        for (size_t i = 0; i < inner->count - 1; i++)
        {
            std::cout << "|" << inner->keys[i] << "| ";
        }
        std::cout << "\n";

        for (size_t i = 0; i < inner->count; i++)
        {
            auto child = getNode(inner->children[i]);
            printNode(child, depth + 1);
        }
    }
}
