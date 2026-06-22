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
        UNUSED(key);
        return 0;
    }

    /// Insert a separator key and a child pointer.
    /// @param[in] key          The separator that should be inserted.
    /// @param[in] child_id     The id of the child page that should be inserted.
    void insert(const KeyT &key, uint64_t child_id)
    {
        // TODO: Implement this function and remove UNUSED(...) calls.
        UNUSED(key);
        UNUSED(child_id);
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
        // TODO: Implement this function and remove UNUSED(...) calls.
        UNUSED(key);
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
        UNUSED(key);
    }
};

// =============================================================================
// BTree method implementations
// =============================================================================

/// Constructor.
template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
BTree<KeyT, ValueT, ComparatorT, PageSize>::BTree(BufferManager &bm)
    : buffer_manager(bm)
{
    // TODO
    // (Hint: You need a persistent Btree, so your reconstruction logic should
    //  trace parent relationships upward consistently.)
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
