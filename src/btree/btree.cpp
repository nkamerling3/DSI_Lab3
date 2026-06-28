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
        auto it = std::upper_bound(keys, keys + this->count - 1, key, comparator);
        size_t idx = it - keys;

        return idx;
    }

    /// Insert a separator key and a child pointer.
    /// @param[in] key          The separator that should be inserted.
    /// @param[in] child_id     The id of the child page that should be inserted.
    void insert(const KeyT &key, uint64_t child_id)
    {
        ComparatorT comparator;
        // TODO: Implement this function and remove UNUSED(...) calls.
        UNUSED(key);
        UNUSED(child_id);

        if (this->count >= kCapacity)
        {
            std::cout << "insert: inner node already at capacity!" << std::endl;
            return;
        }
        // 0 case
        if (this->count == 0)
        {
            keys[0] = key;
            children[0] = child_id;
            this->count++;
            return;
        }

        // 1 case and the keys are equal
        if (this->count == 1)
        {
            if (!comparator(key, keys[0]) && !comparator(keys[0], key))
            {
                children[1] = child_id;
                this->count++;
                return;
            }
        }

        // 1 case and the counts aren't equal

        size_t insert_idx = find_child_index(key);
        size_t child_insert_idx = insert_idx + 1;

        // shift key and child arrays
        for (size_t i = this->count - 1; i > insert_idx; i--)
        {
            keys[i] = keys[i - 1];
        }
        for (size_t i = this->count; i > child_insert_idx; i--)
        {
            children[i] = children[i - 1];
        }
        // Implementation may be incorrect, need to fix
        keys[insert_idx] = key;
        children[child_insert_idx] = child_id;
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
            std::cout << "insert: initial key inserted " << std::endl;
            return;
        }

        // find key position with binary search
        auto it = std::lower_bound(keys, keys + this->count, key, comparator);
        uint64_t idx = it - keys;
        if (idx == this->count)
        {
            keys[idx] = key;
            values[idx] = value;
            std::cout << "insert: key inserted at end pos " << idx << std::endl;
            this->count++;
            return;
        }
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
        else if (comparator(key, keys[idx]))
        {
            std::cout << "erase: leaf key not found!" << std::endl;
            return;
        }

        std::cout << "erase: leaf key match found!" << std::endl;
        for (size_t i = idx; i + 1 < this->count; i++)
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

        uint64_t lastNodeID = next_page_id - 1;

        // trace lastNodeID back up to parent
        auto node = getNode(lastNodeID);
        while (node->parent_node_id != INVALID_VALUE)
        {
            auto parentNode = getNode(node->parent_node_id);
            node = parentNode;
        }
        root = node->node_id;
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
        next_page_id = 1;
    }
}

/// Lookup an entry in the tree.
/// @param[in] key      The key that should be searched.
template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
std::optional<ValueT>
BTree<KeyT, ValueT, ComparatorT, PageSize>::lookup(const KeyT &key)
{
    std::cout << "lookup: looking up value: " << key << std::endl;

    // if empty, return immediately
    if (!root.has_value())
    {
        return std::nullopt;
    }
    // TODO
    // start at root

    auto node = getNode(*root);

    // traverse while root is not empty
    while (!node->is_leaf())
    {
        std::cout << "lookup: Node is not a leaf, printing node details " << key << std::endl;
        // printNode(node, 0);
        auto inNode = std::static_pointer_cast<InnerNode>(node);
        uint64_t childIdx = inNode->find_child_index(key);
        uint64_t childPage = inNode->children[childIdx];
        auto childNode = getNode(childPage);
        node = childNode;
    }

    // once it is a leaf, binary search to find key
    std::cout << "lookup: reached leaf node, printing out details " << key << std::endl;
    // printNode(node, 0);
    ComparatorT comparator;
    auto lNode = std::static_pointer_cast<LeafNode>(node);

    auto it = std::lower_bound(lNode->keys, lNode->keys + lNode->count, key, comparator);
    uint64_t idx = it - lNode->keys;
    if (idx >= lNode->count)
    {
        return std::nullopt;
    }
    if (!comparator(key, lNode->keys[idx]))
    {
        ValueT target = lNode->values[idx];
        return target;
    }
    return std::nullopt;
}

/// Returns the range of values between low and high, both inclusive.
/// @param[in] low The low key from which range starts
/// @param[in] high The high key where the range ends
template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
std::vector<std::pair<KeyT, ValueT>>
BTree<KeyT, ValueT, ComparatorT, PageSize>::rangeQuery(const KeyT &low, const KeyT &high)
{
    std::vector<std::pair<KeyT, ValueT>> range;
    // TODO
    // if empty, return immediately
    if (!root.has_value())
    {
        return range;
    }

    auto node = getNode(*root);

    // vertical traversal to low
    while (!node->is_leaf())
    {
        auto inNode = std::static_pointer_cast<InnerNode>(node);
        uint64_t childIdx = inNode->find_child_index(low);
        uint64_t childPage = inNode->children[childIdx];
        auto childNode = getNode(childPage);
        node = childNode;
    }

    // once it is a leaf, binary search to find key
    ComparatorT comparator;
    auto lNode = std::static_pointer_cast<LeafNode>(node);
    auto it = std::lower_bound(lNode->keys, lNode->keys + lNode->count, low, comparator);
    uint64_t idx = it - lNode->keys;
    if (idx >= lNode->count)
    {
        return range;
    }

    // get values from first leaf node
    for (uint64_t i = idx; i < lNode->count; i++)
    {
        if (comparator(high, lNode->keys[i]))
        {
            return range;
        }
        std::pair<KeyT, ValueT> p = {lNode->keys[i], lNode->values[i]};
        range.push_back(p);
    }

    // horizontal traversal to high
    uint64_t nextNodeID = lNode->next;
    while (nextNodeID != INVALID_VALUE)
    {
        auto leNode = std::static_pointer_cast<LeafNode>(getNode(nextNodeID));
        for (uint64_t i = 0; i < leNode->count; i++)
        {
            if (comparator(high, leNode->keys[i]))
            {
                return range;
            }
            std::pair<KeyT, ValueT> p = {lNode->keys[i], lNode->values[i]};
            range.push_back(p);
        }
        nextNodeID = leNode->next;
    }

    return range;
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
    auto node = getNode(*root);

    // traverse while root is not empty
    while (!node->is_leaf())
    {
        auto inNode = std::static_pointer_cast<InnerNode>(node);
        uint64_t childIdx = inNode->find_child_index(key);
        uint64_t childPage = inNode->children[childIdx];
        auto childNode = getNode(childPage);
        node = childNode;
    }

    // once it is a leaf, binary search to find key
    auto lNode = std::static_pointer_cast<LeafNode>(node);
    lNode->erase(key);
}

/// Inserts a new entry into the tree.
/// @param[in] key      The key that should be inserted.
/// @param[in] value    The value that should be inserted.
template <typename KeyT, typename ValueT, typename ComparatorT, size_t PageSize>
void BTree<KeyT, ValueT, ComparatorT, PageSize>::insert(const KeyT &key, const ValueT &value)
{
    std::cout << "insert: attempt to insert key " << key << " and value " << value << std::endl;

    // TODO
    // initial Insert, get page 1, initialize metadata on page 0, create leaf node
    if (!root.has_value())
    {
        root = next_page_id;
        SlottedPage &page = buffer_manager.fix_page(next_page_id);
        auto leaf = new (page.page_data.get()) LeafNode();
        leaf->node_id = next_page_id;
        leaf->insert(key, value);
        std::cout << "inert initialize root case: noxt_page_id is now: " << next_page_id << std::endl;

        next_page_id++;
        // write next page ID to page 0
        SlottedPage &metaPage = this->buffer_manager.fix_page(0);
        // set contains tag and metadata on page 0
        MetaData *meta = reinterpret_cast<MetaData *>(metaPage.page_data.get());
        meta->rootTag = containsRootTag;
        meta->next_page_num = next_page_id;
        return;
    }

    auto insert_internal = [&](auto &&self,
                               std::shared_ptr<Node> node,
                               const KeyT &inner_key,
                               const ValueT &inner_value,
                               std::vector<std::shared_ptr<Node>> &path) -> void
    {
        // add in special logic for root
        uint64_t nodeID = node->node_id;
        if (nodeID == root)
        {
            if (node->is_leaf())
            {
                auto leafRoot = std::static_pointer_cast<LeafNode>(node);
                if (leafRoot->count >= LeafNode::kCapacity)
                {
                    SlottedPage &page = buffer_manager.fix_page(next_page_id);
                    auto newRoot = new (page.page_data.get()) InnerNode();
                    newRoot->node_id = next_page_id;
                    leafRoot->parent_node_id = next_page_id;
                    root = next_page_id;
                    next_page_id++;
                    std::cout << "inert new root leaf case: noxt_page_id is now: " << next_page_id << std::endl;
                    SlottedPage &metaPage = this->buffer_manager.fix_page(0);
                    MetaData *meta = reinterpret_cast<MetaData *>(metaPage.page_data.get());
                    meta->next_page_num = next_page_id;

                    std::vector<std::shared_ptr<Node>> path;
                    splitNode(path, leafRoot);
                    self(self, getNode(*root), key, value, path);
                }
                else
                {
                    leafRoot->insert(key, value);
                }
            }
            else
            {
                auto innerRoot = std::static_pointer_cast<InnerNode>(node);
                if (innerRoot->count >= InnerNode::kCapacity)
                {
                    SlottedPage &page = buffer_manager.fix_page(next_page_id);
                    auto newRoot = new (page.page_data.get()) InnerNode();
                    newRoot->node_id = next_page_id;
                    innerRoot->parent_node_id = next_page_id;
                    root = next_page_id;
                    next_page_id++;

                    std::cout << "inert new root inner case: noxt_page_id is now: " << next_page_id << std::endl;
                    SlottedPage &metaPage = this->buffer_manager.fix_page(0);
                    MetaData *meta = reinterpret_cast<MetaData *>(metaPage.page_data.get());
                    meta->next_page_num = next_page_id;

                    std::vector<std::shared_ptr<Node>> path;
                    splitNode(path, innerRoot);
                    self(self, getNode(*root), inner_key, inner_value, path);
                }
                else
                {
                    size_t childIdx = innerRoot->find_child_index(inner_key);
                    auto childNode = getNode(innerRoot->children[childIdx]);
                    self(self, childNode, inner_key, inner_value, path);
                }
            }
        }

        else if (node->is_leaf())
        {
            auto lNode = std::static_pointer_cast<LeafNode>(node);
            if (lNode->count >= LeafNode::kCapacity)
            {
                splitNode(path, lNode);
                uint64_t parentPage = lNode->parent_node_id;
                auto parentNode = getNode(parentPage);
                self(self, parentNode, inner_key, inner_value, path);
            }
            else
            {
                lNode->insert(inner_key, inner_value);
            }
        }
        else
        {
            auto inNode = std::static_pointer_cast<InnerNode>(node);
            if (inNode->count >= InnerNode::kCapacity)
            {
                splitNode(path, inNode);
                uint64_t parentPage = inNode->parent_node_id;
                auto parentNode = getNode(parentPage);
                self(self, parentNode, inner_key, inner_value, path);
            }
            else
            {
                size_t childIdx = inNode->find_child_index(inner_key);
                auto childNode = getNode(inNode->children[childIdx]);
                self(self, childNode, inner_key, inner_value, path);
            }
        }
    };
    std::vector<std::shared_ptr<Node>> path;
    insert_internal(insert_internal, getNode(*root), key, value, path);
    // if (rootNode->is_leaf())
    // {
    //     auto leafRoot = std::static_pointer_cast<LeafNode>(rootNode);
    //     if (leafRoot->count >= LeafNode::kCapacity)
    //     {
    //         SlottedPage &page = buffer_manager.fix_page(next_page_id);
    //         auto newRoot = new (page.page_data.get()) InnerNode();
    //         newRoot->node_id = next_page_id;
    //         leafRoot->parent_node_id = next_page_id;
    //         root = next_page_id;
    //         next_page_id++;
    //         std::cout << "inert new root leaf case: noxt_page_id is now: " << next_page_id << std::endl;
    //         SlottedPage &metaPage = this->buffer_manager.fix_page(0);
    //         MetaData *meta = reinterpret_cast<MetaData *>(metaPage.page_data.get());
    //         meta->next_page_num = next_page_id;

    //         std::vector<std::shared_ptr<Node>> path;
    //         splitNode(path, leafRoot);
    //         insert_internal(insert_internal, getNode(*root), key, value, path);
    //         return;
    //     }
    //     else
    //     {
    //         leafRoot->insert(key, value);
    //         return;
    //     }
    // }
    // else
    // {
    //     auto innerRoot = std::static_pointer_cast<InnerNode>(rootNode);
    //     if (innerRoot->count >= InnerNode::kCapacity - 1)
    //     {
    //         SlottedPage &page = buffer_manager.fix_page(next_page_id);
    //         auto newRoot = new (page.page_data.get()) InnerNode();
    //         newRoot->node_id = next_page_id;
    //         innerRoot->parent_node_id = next_page_id;
    //         root = next_page_id;
    //         next_page_id++;

    //         std::cout << "inert new root inner case: noxt_page_id is now: " << next_page_id << std::endl;
    //         SlottedPage &metaPage = this->buffer_manager.fix_page(0);
    //         MetaData *meta = reinterpret_cast<MetaData *>(metaPage.page_data.get());
    //         meta->next_page_num = next_page_id;

    //         std::vector<std::shared_ptr<Node>> path;
    //         splitNode(path, innerRoot);
    //         insert_internal(insert_internal, getNode(*root), key, value, path);
    //         return;
    //     }
    //     else
    //     {
    //         std::vector<std::shared_ptr<Node>> path;
    //         insert_internal(insert_internal, rootNode, key, value, path);
    //         return;
    //     }
    // }
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

    // std::cout << "printing tree info" << std::endl;

    uint64_t median = node->count / 2;
    if (node->is_leaf())
    {
        // set up newLeaf node
        auto oldLeaf = std::static_pointer_cast<LeafNode>(node);
        uint64_t oldCount = oldLeaf->count;
        SlottedPage &page = buffer_manager.fix_page(next_page_id);
        auto newLeaf = new (page.page_data.get()) LeafNode();
        newLeaf->node_id = next_page_id;
        newLeaf->parent_node_id = node->parent_node_id;
        newLeaf->next = oldLeaf->next;
        oldLeaf->next = newLeaf->node_id;
        std::cout << "print new node and old nodes leaf case " << next_page_id << std::endl;
        std::cout << "new node: " << next_page_id << std::endl;

        // redistribute keys to new leaf node
        for (uint64_t i = median; i < oldCount; i++)
        {
            newLeaf->keys[i - median] = oldLeaf->keys[i];
            newLeaf->values[i - median] = oldLeaf->values[i];
        }
        oldLeaf->count = median;
        newLeaf->count = oldCount - median;
        printNode(getNode(next_page_id), 0);

        // insert into parent
        // TODO: handle case when parent is empty
        auto parentNode = std::static_pointer_cast<InnerNode>(getNode(node->parent_node_id));

        KeyT medKey = newLeaf->keys[0];
        // put 0 count parent case here
        if (parentNode->count == 0)
        {
            parentNode->insert(medKey, oldLeaf->node_id);
        }
        parentNode->insert(medKey, newLeaf->node_id);

        next_page_id++;

        std::cout << "splitNode leaf: noxt_page_id is now: " << next_page_id << std::endl;
        SlottedPage &metaPage = buffer_manager.fix_page(0);
        MetaData *meta = reinterpret_cast<MetaData *>(metaPage.page_data.get());
        meta->next_page_num = next_page_id;
    }
    // node is inner node
    else
    {
        // set up newInner node
        auto oldInner = std::static_pointer_cast<InnerNode>(node);
        uint64_t oldCount = oldInner->count;
        SlottedPage &page = buffer_manager.fix_page(next_page_id);
        auto newInner = new (page.page_data.get()) InnerNode();
        newInner->node_id = next_page_id;
        newInner->parent_node_id = node->parent_node_id;

        std::cout << "print new node and old nodes inner case" << next_page_id << std::endl;
        std::cout << "new node: " << next_page_id << std::endl;
        printNode(getNode(next_page_id), 0);

        // get the median key to insert into parent
        KeyT medKey = oldInner->keys[median];

        // redistribute keys to new leaf node
        for (uint64_t i = median + 1; i < oldCount - 1; i++)
        {
            newInner->keys[i - (median + 1)] = oldInner->keys[i];
        }

        // store oldInner and newInner Ids in memory so we can get them again
        uint64_t oldInnerID = oldInner->node_id;
        uint64_t newInnerID = newInner->node_id;
        // redistribute children to new inner nodes
        for (uint64_t i = median + 1; i < oldCount; i++)
        {
            oldInner = std::static_pointer_cast<InnerNode>(getNode(oldInnerID));
            newInner = std::static_pointer_cast<InnerNode>(getNode(newInnerID)).get();
            newInner->children[i - (median + 1)] = oldInner->children[i];
            // Update the child's parent_node_id to reflect its new parent
            std::cout << "redistribute child path" << std::endl;
            auto child = getNode(oldInner->children[i]);
            child->parent_node_id = newInner->node_id;
        }

        // get the correct values of oldInner and new Inner back
        oldInner = std::static_pointer_cast<InnerNode>(getNode(oldInnerID));
        newInner = std::static_pointer_cast<InnerNode>(getNode(newInnerID)).get();

        // Edge case:  if capacity is 3 or less, then algorithm breaks (assume edge case not hit for now)
        oldInner->count = median + 1;
        newInner->count = oldCount - median - 1;

        // insert into parent
        std::cout << "splitNode inner: getting parent node " << node->parent_node_id << std::endl;
        auto parentNode = std::static_pointer_cast<InnerNode>(getNode(node->parent_node_id));

        // put 0 count parent case here
        if (parentNode->count == 0)
        {
            parentNode->insert(medKey, oldInner->node_id);
        }
        parentNode->insert(medKey, newInner->node_id);

        next_page_id++;

        std::cout << "splitNode inner: noxt_page_id is now: " << next_page_id << std::endl;

        SlottedPage &metaPage = buffer_manager.fix_page(0);
        MetaData *meta = reinterpret_cast<MetaData *>(metaPage.page_data.get());
        meta->next_page_num = next_page_id;
    }

    UNUSED(path);

    return;
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
        for (size_t i = 0; i + 1 < inner->count; i++)
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
