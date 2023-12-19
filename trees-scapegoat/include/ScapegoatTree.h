#pragma once

#include <vector>

class ScapegoatTree
{

public:
    ScapegoatTree() = default;
    ScapegoatTree(double alpha);

    bool contains(int value) const;
    bool insert(int value);
    bool remove(int value);

    std::size_t size() const;

    bool empty() const;

    std::vector<int> values() const;

    ~ScapegoatTree();

private:
    struct Node
    {
        Node * left = nullptr;
        Node * right = nullptr;
        int size = 1;
        int value = 0;

        Node(int x);

        ~Node();
    };
    Node * root = nullptr;
    double tree_alpha = 0.75;
    std::size_t tree_size = 0;

    bool contains(Node * node, int value) const;
    Node * find_min(Node * node, int size) const;
    Node * insert(Node * node, int value) const;
    void collection_values(Node * v, std::vector<Node *> & result) const;
    Node * build_balanced_tree(Node * v, std::vector<Node *> & vertex, std::size_t start, std::size_t end);
    Node * remove(Node * node, int value) const;
    void values(Node * node, std::vector<int> & ordered) const;
    Node * check_rebuild(Node * pNode, int value);
};
