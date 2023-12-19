#include "ScapegoatTree.h"

#include <iostream>
#include <stdexcept>

ScapegoatTree::Node::Node(int x)
    : value(x){};

ScapegoatTree::Node::~Node()
{
    delete left;
    delete right;
}

ScapegoatTree::ScapegoatTree(const double alpha)
    : root(nullptr)
    , tree_alpha(alpha)
    , tree_size(0)
{
    {
        if (alpha < 0.5 || alpha > 1) {
            throw std::invalid_argument("Invalid argument, alpha belongs to 0.5 to 1");
        }
    }
}
bool ScapegoatTree::contains(const int value) const
{
    return contains(root, value);
}

bool ScapegoatTree::contains(Node * node, const int value) const
{
    while (node != nullptr) {
        if (node->value == value) {
            return true;
        }
        if (node->value > value) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }
    return false;
}

bool ScapegoatTree::insert(int value)
{
    if (contains(value)) {
        return false;
    }
    root = insert(root, value);
    ++tree_size;
    root = check_rebuild(root, value);
    return true;
}
ScapegoatTree::Node * ScapegoatTree::insert(Node * node, const int value) const
{
    if (node == nullptr) {
        return new Node(value);
    }
    ++node->size;
    if (node->value > value) {
        node->left = insert(node->left, value);
    }
    else {
        node->right = insert(node->right, value);
    }
    return node;
}

ScapegoatTree::Node * ScapegoatTree::build_balanced_tree(Node * v, std::vector<Node *> & vertex, std::size_t start, std::size_t end)
{
    if (end - start < 1) {
        return nullptr;
    }
    if (end - start == 1) {
        Node * node = vertex[start];
        node->left = nullptr;
        node->right = nullptr;
        node->size = 1;
        return node;
    }
    int m = (end + start) / 2;
    v = vertex[m];
    v->size = end - start;
    v->left = build_balanced_tree(v->left, vertex, start, m);
    v->right = build_balanced_tree(v->right, vertex, m + 1, end);
    return v;
}
void ScapegoatTree::collection_values(Node * v, std::vector<Node *> & result) const
{
    if (v == nullptr) {
        return;
    }
    collection_values(v->left, result);
    result.push_back(v);
    collection_values(v->right, result);
}

bool ScapegoatTree::remove(const int value)
{
    if (!contains(value)) {
        return false;
    }
    root = remove(root, value);
    --tree_size;
    root = check_rebuild(root, value);
    return true;
}

ScapegoatTree::Node * ScapegoatTree::remove(Node * node, const int value) const
{
    --node->size;
    if (node->value > value) {
        node->left = remove(node->left, value);
        return node;
    }
    else if (node->value < value) {
        node->right = remove(node->right, value);
        return node;
    }
    if (node->left == nullptr && node->right == nullptr) {
        delete node;
        return nullptr;
    }
    if (node->left == nullptr || node->right == nullptr) {
        Node * tmp;
        if (node->left == nullptr) {
            tmp = node->right;
        }
        else {
            tmp = node->left;
        }
        node->left = node->right = nullptr;
        delete node;
        return tmp;
    }
    Node * new_vertex = node->left;
    if (new_vertex->right != nullptr) {
        find_min(node->right, new_vertex->right->size)->left = new_vertex->right;
    }
    new_vertex->right = node->right;
    node->left = node->right = nullptr;
    delete node;

    new_vertex->size = new_vertex->right->size + 1;
    if (new_vertex->left != nullptr) {
        new_vertex->size += new_vertex->left->size;
    }
    return new_vertex;
}
ScapegoatTree::Node * ScapegoatTree::find_min(Node * node, const int size) const
{
    while (node->left != nullptr) {
        node->size += size;
        node = node->left;
    }
    return node;
}

std::size_t ScapegoatTree::size() const
{
    return tree_size;
}
std::vector<int> ScapegoatTree::values() const
{
    std::vector<int> result;
    result.reserve(tree_size);
    values(root, result);
    return result;
}

void ScapegoatTree::values(Node * node, std::vector<int> & mas) const
{
    if (node == nullptr) {
        return;
    }
    values(node->left, mas);
    mas.push_back(node->value);
    values(node->right, mas);
}
ScapegoatTree::~ScapegoatTree()
{
    delete root;
}
bool ScapegoatTree::empty() const
{
    return tree_size == 0;
}

ScapegoatTree::Node * ScapegoatTree::check_rebuild(Node * node, const int value)
{
    if (node == nullptr || node->value == value) {
        return node;
    }
    int kf = node->size * tree_alpha;
    if ((node->left != nullptr && kf < node->left->size) || (node->right != nullptr && kf < node->right->size)) {
        std::vector<Node *> base;
        base.reserve(node->size);
        collection_values(node, base);
        return build_balanced_tree(node, base, 0, base.size());
    }
    if (node->value > value) {
        node->left = check_rebuild(node->left, value);
    }
    else {
        node->right = check_rebuild(node->right, value);
    }
    return node;
}
