//
// Created by winig on 8/4/2026.
//

#ifndef ANANSI_RBTREE_H
#define ANANSI_RBTREE_H
#include "../types/AnansiTypes.h"
#include "Memtable.h"
#include "Nodes.h"
#include <memory>
#include <utility>
#include <cstring>
enum RBColor {RED, BLACK};

struct RBNode : Node {
    RBColor color;
    RBNode *parent;
    std::shared_ptr<RBNode> left, right;
    RBNode():
    color{RED},
    parent{nullptr},
    left{nullptr},
    right{nullptr}{}
    RBNode(const RBColor color, RBNode *parent, std::shared_ptr<RBNode> left, std::shared_ptr<RBNode> right,const AnansiData& data):
    color{color},
    parent{parent},
    left{std::move(left)},
    right{std::move(right)}{this->data = data;}

};

class RBTree : public Memtable{
    std::shared_ptr<RBNode> NIL = std::make_shared<RBNode>(RBNode{RBColor::BLACK,nullptr,nullptr,nullptr,AnansiData{}});
    std::shared_ptr<RBNode> root;
    public:
        RBTree() : root{NIL} {}
        ~RBTree() override {root = nullptr;};

        bool insert(AnansiData data) override {
            // Empty tree: new node becomes the (black) root.
            if (root == NIL) {
                auto temp = RBNode{RBColor::BLACK, NIL.get(), NIL, NIL, data};
                root = std::make_shared<RBNode>(temp);
                return true;
            }
            //data.primaryKeyAndValue.value
            RBNode* parentNode = findInsert(data, root.get());
            auto node = std::make_shared<RBNode>(RBNode(RBColor::RED, *&parentNode, NIL, NIL, data));
            int length = bytesNeeded(data.primaryKeyAndValue.type);
            if (std::memcmp(data.primaryKeyAndValue.value, parentNode->data.primaryKeyAndValue.value, length) < 0) {
                parentNode->left = node;
            } else {
                parentNode->right = node;
            }

            insertFixUp(node.get());

            // Root must always be black.
            root->color = RBColor::BLACK;
            return true;
        }

        RBNode* findInsert(AnansiData data, RBNode *node) {
                int length = bytesNeeded(data.primaryKeyAndValue.type);
                if (std::memcmp(data.primaryKeyAndValue.value, node->data.primaryKeyAndValue.value, length) < 0) {
                    if (node->left == NIL) {
                        return node;
                    }
                    return findInsert(data, node->left.get());
                }
                if (node->right == NIL) {
                    return node;
                }
                return findInsert(data, node->right.get());
        }


        void leftRotation(RBNode *node) {
                RBNode *parent = node->parent;
                RBNode *grandParent = parent->parent; // save before it gets overwritten
                auto tempNode = parent->right; // shared_ptr to `node`, used to relink grandParent below
                std::shared_ptr<RBNode> parentSP =
                    (grandParent == NIL.get()) ? root :
                    (grandParent->left.get() == parent) ? grandParent->left : grandParent->right;

                node->parent = grandParent;
                parent->right = node->left;
                if (node->left != NIL) {
                    node->left->parent = parent;
                }
                node->left = parentSP;      // was: node->left = grandParent->left;
                parent->parent = node;

                if (grandParent == NIL.get()) {
                    root = tempNode;
                }
                else if (grandParent->left.get() == parent) {
                    grandParent->left = tempNode;
                } else {
                    grandParent->right = tempNode;
                }
            }

        void rightRotation(RBNode *node) {
                RBNode *parent = node->parent;
                RBNode *grandParent = parent->parent; // save before it gets overwritten
                auto tempNode = parent->left; // shared_ptr to `node`, used to relink grandParent below
                std::shared_ptr<RBNode> parentSP =
                    (grandParent == NIL.get()) ? root :
                    (grandParent->left.get() == parent) ? grandParent->left : grandParent->right;

                node->parent = grandParent;
                parent->left = node->right;
                if (node->right != NIL) {
                    node->right->parent = parent;
                }
                node->right = parentSP;     // was: node->right = grandParent->right;
                parent->parent = node;

                if (grandParent == NIL.get()) {
                    root = tempNode;
                }
                else if (grandParent->left.get() == parent) {
                    grandParent->left = tempNode;
                } else {
                    grandParent->right = tempNode;
                }
            }

        void insertFixUp(RBNode *node) {
                while (node != root.get() && node->parent->color == RBColor::RED) {
                    RBNode *parent = node->parent;
                    RBNode *grandParent = parent->parent;

                    if (grandParent->left.get() == parent) {
                        RBNode *uncle = grandParent->right.get();
                        if (uncle->color == RBColor::RED) {
                            parent->color = RBColor::BLACK;
                            uncle->color = RBColor::BLACK;
                            grandParent->color = RBColor::RED;
                            node = grandParent;
                        } else {
                            if (node == parent->right.get()) {
                                // Triangle case: promote the ORIGINAL node past its
                                // parent first. node really is parent->right here,
                                // which is exactly what leftRotation requires.
                                leftRotation(node);
                            }
                            // Whichever node now sits at grandParent->left (the
                            // original `parent` in the line case, or the promoted
                            // `node` in the triangle case) becomes black;
                            // grandParent becomes red and rotates down below it.
                            grandParent->left->color = RBColor::BLACK;
                            grandParent->color = RBColor::RED;
                            rightRotation(grandParent->left.get());
                            break; // subtree top is black now; fixup is complete
                        }
                    } else {
                        RBNode *uncle = grandParent->left.get();
                        if (uncle->color == RBColor::RED) {
                            parent->color = RBColor::BLACK;
                            uncle->color = RBColor::BLACK;
                            grandParent->color = RBColor::RED;
                            node = grandParent;
                        } else {
                            if (node == parent->left.get()) {
                                rightRotation(node);
                            }
                            grandParent->right->color = RBColor::BLACK;
                            grandParent->color = RBColor::RED;
                            leftRotation(grandParent->right.get());
                            break;
                        }
                    }
                    root->color = RBColor::BLACK;
                }
        }


        RBNode* find(std::byte *value) override {
            std::shared_ptr<RBNode> curr = root;
            int length = bytesNeeded(curr->data.primaryKeyAndValue.type);
            while (curr != NIL) {
                if (std::memcmp(value,curr->data.primaryKeyAndValue.value,length) == 0) {
                    return curr.get();
                }
                if (std::memcmp(value,curr->data.primaryKeyAndValue.value,length) < 0) {
                    curr = curr->left;
                } else {
                    curr = curr->right;
                }
            }
            return curr.get();
        }

        std::vector<AnansiData*> inorder() {
            std::vector<AnansiData*> result;
            recursiveInorder(result, root.get());
            return result;
        }


    void recursiveInorder(std::vector<AnansiData*>& lis, RBNode *node) {
        if (node == NIL.get()) return;

        recursiveInorder(lis, node->left.get());
        lis.push_back(&node->data);
        recursiveInorder(lis, node->right.get());
    }


    RBNode* remove(std::byte *value) override {
        RBNode* element = find(value);
        return element;
    }

};

#endif //ANANSI_RBTREE_H