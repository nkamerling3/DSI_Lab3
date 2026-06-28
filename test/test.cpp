#include "../src/btree/btree.cpp"

int main(int argc, char *argv[])
{
    bool execute_all = false;
    std::string selected_test = "-1";

    if (argc < 2)
    {
        execute_all = true;
    }
    else
    {
        selected_test = argv[1];
    }

    using BTree = BTree<uint64_t, uint64_t, std::less<uint64_t>, 1024>;

    // Test 1: InsertEmptyTree
    if (execute_all || selected_test == "1")
    {
        std::cout << "...Starting Test 1" << std::endl;
        BufferManager buffer_manager;
        BTree tree(buffer_manager);

        ASSERT_WITH_MESSAGE(tree.root.has_value() == false,
                            "tree.root is not nullptr");

        tree.insert(42, 21);

        ASSERT_WITH_MESSAGE(tree.root.has_value(),
                            "tree.root is still nullptr after insertion");

        std::string test = "inserting an element into an empty B-Tree";

        SlottedPage *root_page = &buffer_manager.fix_page(*tree.root);
        auto root_node = reinterpret_cast<BTree::Node *>(root_page->page_data.get());

        ASSERT_WITH_MESSAGE(root_node->is_leaf() == true,
                            test + " does not create a leaf node.");
        ASSERT_WITH_MESSAGE(root_node->count == 1,
                            test + " does not create a leaf node with count = 1.");

        std::cout << "Passed: Test 1" << std::endl;
    }

    // Test 2: InsertLeafNode
    if (execute_all || selected_test == "2")
    {
        std::cout << "...Starting Test 2" << std::endl;
        BufferManager buffer_manager;
        BTree tree(buffer_manager);

        ASSERT_WITH_MESSAGE(tree.root.has_value() == false,
                            "tree.root is not nullptr");

        for (auto i = 0ul; i < BTree::LeafNode::kCapacity; ++i)
        {
            tree.insert(i, 2 * i);
        }
        ASSERT_WITH_MESSAGE(tree.root.has_value(),
                            "tree.root is still nullptr after insertion");

        std::string test = "inserting BTree::LeafNode::kCapacity elements into an empty B-Tree";

        SlottedPage *root_page = &buffer_manager.fix_page(*tree.root);
        auto root_node = reinterpret_cast<BTree::Node *>(root_page->page_data.get());
        auto root_inner_node = static_cast<BTree::InnerNode *>(root_node);

        ASSERT_WITH_MESSAGE(root_node->is_leaf() == true,
                            test + " creates an inner node as root.");
        ASSERT_WITH_MESSAGE(root_inner_node->count == BTree::LeafNode::kCapacity,
                            test + " does not store all elements.");

        std::cout << "Passed: Test 2" << std::endl;
    }

    // Test 3: InsertLeafNodeSplit
    if (execute_all || selected_test == "3")
    {
        std::cout << "...Starting Test 3" << std::endl;
        BufferManager buffer_manager;
        BTree tree(buffer_manager);

        ASSERT_WITH_MESSAGE(tree.root.has_value() == false,
                            "tree.root is not nullptr");

        for (auto i = 0ul; i < BTree::LeafNode::kCapacity; ++i)
        {
            tree.insert(i, 2 * i);
        }
        ASSERT_WITH_MESSAGE(tree.root.has_value(),
                            "tree.root is still nullptr after insertion");

        SlottedPage *root_page = &buffer_manager.fix_page(*tree.root);
        auto root_node = reinterpret_cast<BTree::Node *>(root_page->page_data.get());
        auto root_inner_node = static_cast<BTree::InnerNode *>(root_node);

        assert(root_inner_node->is_leaf());
        assert(root_inner_node->count == BTree::LeafNode::kCapacity);

        tree.insert(424242, 42);

        std::string test =
            "inserting BTree::LeafNode::kCapacity + 1 elements into an empty B-Tree";

        ASSERT_WITH_MESSAGE(tree.root.has_value() != false, test + " removes the root :-O");

        SlottedPage *root_page1 = &buffer_manager.fix_page(*tree.root);
        root_node = reinterpret_cast<BTree::Node *>(root_page1->page_data.get());
        root_inner_node = static_cast<BTree::InnerNode *>(root_node);

        ASSERT_WITH_MESSAGE(root_inner_node->is_leaf() == false,
                            test + " does not create a root inner node");
        ASSERT_WITH_MESSAGE(root_inner_node->count == 2,
                            test + " creates a new root with count != 2");

        std::cout << "Passed: Test 3" << std::endl;
    }

    // Test 4: LookupEmptyTree
    if (execute_all || selected_test == "4")
    {
        std::cout << "...Starting Test 4" << std::endl;
        BufferManager buffer_manager;
        BTree tree(buffer_manager);

        std::string test = "searching for a non-existing element in an empty B-Tree";

        ASSERT_WITH_MESSAGE(tree.lookup(42).has_value() == false,
                            test + " seems to return something :-O");

        std::cout << "Passed: Test 4" << std::endl;
    }

    // Test 5: LookupSingleLeaf
    if (execute_all || selected_test == "5")
    {
        std::cout << "...Starting Test 5" << std::endl;
        BufferManager buffer_manager;
        BTree tree(buffer_manager);

        for (auto i = 0ul; i < BTree::LeafNode::kCapacity; ++i)
        {
            tree.insert(i, 2 * i);
            ASSERT_WITH_MESSAGE(tree.lookup(i).has_value(),
                                "searching for the just inserted key k=" + std::to_string(i) + " yields nothing");
        }

        for (auto i = 0ul; i < BTree::LeafNode::kCapacity; ++i)
        {
            auto v = tree.lookup(i);
            ASSERT_WITH_MESSAGE(v.has_value(), "key=" + std::to_string(i) + " is missing");
            ASSERT_WITH_MESSAGE(*v == 2 * i, "key=" + std::to_string(i) + " should have the value v=" + std::to_string(2 * i));
        }

        std::cout << "Passed: Test 5" << std::endl;
    }

    // Test 6: LookupSingleSplit
    if (execute_all || selected_test == "6")
    {
        std::cout << "...Starting Test 6" << std::endl;
        BufferManager buffer_manager;
        BTree tree(buffer_manager);

        for (auto i = 0ul; i < BTree::LeafNode::kCapacity; ++i)
        {
            tree.insert(i, 2 * i);
        }

        tree.insert(BTree::LeafNode::kCapacity, 2 * BTree::LeafNode::kCapacity);
        ASSERT_WITH_MESSAGE(tree.lookup(BTree::LeafNode::kCapacity).has_value(),
                            "searching for the just inserted key k=" + std::to_string(BTree::LeafNode::kCapacity + 1) + " yields nothing");

        for (auto i = 0ul; i < BTree::LeafNode::kCapacity + 1; ++i)
        {
            auto v = tree.lookup(i);
            ASSERT_WITH_MESSAGE(v.has_value(), "key=" + std::to_string(i) + " is missing");
            ASSERT_WITH_MESSAGE(*v == 2 * i,
                                "key=" + std::to_string(i) + " should have the value v=" + std::to_string(2 * i));
        }

        std::cout << "Passed: Test 6" << std::endl;
    }

    // Test 7: LookupMultipleSplitsIncreasing
    if (execute_all || selected_test == "7")
    {
        std::cout << "...Starting Test 7" << std::endl;
        BufferManager buffer_manager;
        BTree tree(buffer_manager);
        auto n = 40 * BTree::LeafNode::kCapacity;

        for (auto i = 0ul; i < n; ++i)
        {
            tree.insert(i, 2 * i);
            ASSERT_WITH_MESSAGE(tree.lookup(i).has_value(),
                                "searching for the just inserted key k=" + std::to_string(i) + " yields nothing");
        }

        for (auto i = 0ul; i < n; ++i)
        {
            auto v = tree.lookup(i);
            ASSERT_WITH_MESSAGE(v.has_value(), "key=" + std::to_string(i) + " is missing");
            ASSERT_WITH_MESSAGE(*v == 2 * i,
                                "key=" + std::to_string(i) + " should have the value v=" + std::to_string(2 * i));
        }
        std::cout << "Passed: Test 7" << std::endl;
    }

    // Test 8: LookupMultipleSplitsDecreasing
    if (execute_all || selected_test == "8")
    {
        std::cout << "...Starting Test 8" << std::endl;
        BufferManager buffer_manager;
        BTree tree(buffer_manager);
        auto n = 10 * BTree::LeafNode::kCapacity;

        for (auto i = n; i > 0; --i)
        {
            tree.insert(i, 2 * i);
            ASSERT_WITH_MESSAGE(tree.lookup(i).has_value(),
                                "searching for the just inserted key k=" + std::to_string(i) + " yields nothing");
        }

        for (auto i = n; i > 0; --i)
        {
            auto v = tree.lookup(i);
            ASSERT_WITH_MESSAGE(v.has_value(), "key=" + std::to_string(i) + " is missing");
            ASSERT_WITH_MESSAGE(*v == 2 * i,
                                "key=" + std::to_string(i) + " should have the value v=" + std::to_string(2 * i));
        }

        std::cout << "Passed: Test 8" << std::endl;
        std::cout << "Range Query: test" << std::endl;
        std::vector<std::pair<uint64_t, uint64_t>> range = tree.rangeQuery(5, 600);
        for (auto x : range)
        {
            std::cout << x.first << ":" << x.second << std::endl;
        }
        std::cout << std::endl;
    }

    // Test 9: LookupRandomNonRepeating
    if (execute_all || selected_test == "9")
    {
        std::cout << "...Starting Test 9" << std::endl;
        BufferManager buffer_manager;
        BTree tree(buffer_manager);
        auto n = 10 * BTree::LeafNode::kCapacity;

        std::vector<uint64_t> keys(n);
        std::iota(keys.begin(), keys.end(), n);
        std::mt19937_64 engine(0);
        std::shuffle(keys.begin(), keys.end(), engine);

        for (auto i = 0ul; i < n; ++i)
        {
            tree.insert(keys[i], 2 * keys[i]);
            ASSERT_WITH_MESSAGE(tree.lookup(keys[i]).has_value(),
                                "searching for the just inserted key k=" + std::to_string(keys[i]) +
                                    " after i=" + std::to_string(i) + " inserts yields nothing");
        }

        for (auto i = 0ul; i < n; ++i)
        {
            auto v = tree.lookup(keys[i]);
            ASSERT_WITH_MESSAGE(v.has_value(), "key=" + std::to_string(keys[i]) + " is missing");
            ASSERT_WITH_MESSAGE(*v == 2 * keys[i],
                                "key=" + std::to_string(keys[i]) + " should have the value v=" + std::to_string(2 * keys[i]));
        }

        std::cout << "Passed: Test 9" << std::endl;
    }

    // Test 10: LookupRandomRepeating
    if (execute_all || selected_test == "10")
    {
        std::cout << "...Starting Test 10" << std::endl;
        BufferManager buffer_manager;
        BTree tree(buffer_manager);
        auto n = 10 * BTree::LeafNode::kCapacity;

        std::mt19937_64 engine{0};
        std::uniform_int_distribution<uint64_t> key_distr(0, 99);
        std::vector<uint64_t> values(100);

        for (auto i = 1ul; i < n; ++i)
        {
            uint64_t rand_key = key_distr(engine);
            values[rand_key] = i;
            tree.insert(rand_key, i);

            auto v = tree.lookup(rand_key);
            ASSERT_WITH_MESSAGE(v.has_value(),
                                "searching for the just inserted key k=" + std::to_string(rand_key) +
                                    " after i=" + std::to_string(i - 1) + " inserts yields nothing");
            ASSERT_WITH_MESSAGE(*v == i,
                                "overwriting k=" + std::to_string(rand_key) + " with value v=" + std::to_string(i) +
                                    " failed");
        }

        for (auto i = 0ul; i < 100; ++i)
        {
            if (values[i] == 0)
            {
                continue;
            }
            auto v = tree.lookup(i);
            ASSERT_WITH_MESSAGE(v.has_value(), "key=" + std::to_string(i) + " is missing");
            ASSERT_WITH_MESSAGE(*v == values[i],
                                "key=" + std::to_string(i) + " should have the value v=" + std::to_string(values[i]));
        }

        std::cout << "Passed: Test 10" << std::endl;
    }

    return 0;
}
