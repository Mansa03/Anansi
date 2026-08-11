//
// Created by winig on 8/7/2026.
//

#include <gtest/gtest.h>
#include "../../src/storage/RBTree.h"
#include <string>
#include <iostream>
#include "../../test/TestHelper.h"

class RBTreeTester: public ::testing::Test {
    public:
        RBTreeTester() : tree{}, TEST_DATA{generateTestData()} {
        }
        ~RBTreeTester() override {
            freeAll(TEST_DATA);
        }

        RBTree tree;
        std::vector<AnansiData> TEST_DATA;
};

class IntStringRBTreeTester: public ::testing::Test {
    public:
        IntStringRBTreeTester() : tree{} {
        }

        RBTree tree;
};

TEST_F(RBTreeTester, testOneInsert) {
    tree.insert(TEST_DATA[0]);
    auto result = tree.inorder();
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << "--- Row " << i << " ---\n";
        std::cout << "primary:\n";   printKVPValue(result[i]->primaryKeyAndValue);
        std::cout << "secondary:\n"; printKVPValue(result[i]->secondaryKeyAndValue);
        std::cout << "fields:\n";
        for (auto& f : result[i]->fields) printKVPValue(f);
        std::cout << "\n";
    }
}

TEST_F(RBTreeTester, testInorder) {
    for (const auto& kv : TEST_DATA) {
        tree.insert(kv);
    }
    auto i = 0;
    for (const auto* data : tree.inorder()) {
        i += 1;
        std::cout << "--- Row " << i << " ---\n";
        std::cout << "primary:\n";   printKVPValue(data->primaryKeyAndValue);
        std::cout << "secondary:\n"; printKVPValue(data->secondaryKeyAndValue);
        std::cout << "fields:\n";
        for (auto& f : data->fields) printKVPValue(f);
        std::cout << "\n";
    }
}




