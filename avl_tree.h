#pragma once

#include <iostream>
#include <algorithm>

template<typename T>
class Set{
private:
    struct Node{
        T value;
        Node* left;
        Node* right;
        Node* parent;
        int height;
        size_t size;
        Node(const T& value) : value(value), left(nullptr), right(nullptr),
                               parent(nullptr), height(1), size(1) {}
        Node(const T& value, const Node* parent) : value(value), left(nullptr), right(nullptr),
                               parent(parent), height(1), size(1) {}
    };

    size_t GetSize(Node* node) const {
        return node->size;
    }

    static Node* GetRight(Node* node) {
        if (node) {
            while (node->right) {
                node = node->right;
            }
        }
        return node;
    }

    static Node* GetLeft(Node* node) {
        if (node) {
            while (node->left) {
                node = node->left;
            }
        }
        return node;
    }

    int GetHeight(Node* node) const {
        return node->height;
    }

    int ChildDiff(Node* node) {
        if (!node) {
            return 0;
        }
        return GetHeight(node->left) - GetHeight(node->right);
    }

    void Update(Node* node) {
        node->height = std::max(GetHeight(node->left), GetHeight(node->right)) + 1;
        node->size = GetSize(node->left) + GetSize(node->right) + 1;
    }

    void HangLeft(Node* a, Node* b){
        if (a) {
            a->left = b;
        }
        if (b) {
            b->parent = a;
        }
    }

    void HangRight(Node* a, Node* b){
        if (a) {
            a->right = b;
        }
        if (b) {
            b->parent = a;
        }
    }

    void TurnLeft(Node* a) {
        Node* b = a->right;
        Node* parent = a->parent;
        HangRight(a, b->left);
        HangLeft(b, a);
        b->parent = parent;
        if (parent) {
           if (parent->left == a) {
               HangLeft(parent, b);
           }
           else {
               HangRight(parent, b);
           }
        }
        Update(a);
        Update(b);
    }

    void TurnRight(Node* a) {
        Node* b = a->left;
        Node* parent = a->parent;
        HangLeft(a, b->right);
        HangRight(b, a);
        b->parent = parent;
        if (parent) {
            if (parent->left == a) {
                HangLeft(parent, b);
            }
            else {
                HangRight(parent, b);
            }
        }
        Update(a);
        Update(b);
    }

    void StrongTurnLeft(Node* a) {
        TurnRight(a->right);
        TurnLeft(a);
    }

    void StrongTurnRight(Node* a) {
        TurnLeft(a->left);
        TurnRight(a);
    }

    Node* Rebalance(Node* a) {
        if (std::abs(ChildDiff(a)) < 2) {
            return a;
        }
        if (ChildDiff(a) > 0) {
            Node* b = a->left;
            if(ChildDiff(b) >= 0) {
                TurnRight(a);
                return b;
            }
            else {
                Node* br = b->right;
                StrongTurnRight(a);
                return br;
            }
        }
        else {
            Node* b = a->right;
            if(ChildDiff(b) <= 0) {
                TurnLeft(a);
                return b;
            }
            else {
                Node* br = b->left;
                StrongTurnLeft(a);
                return br;
            }
        }
    }

    Node* Insert(Node* a, const T& val) {
        if (!a) {
            return new Node(val);
        }
        if (a->value == val) {
            return a;
        }
        if(a->value < val) {
            HangRight(a, Insert(a->right, val));
        }
        else {
            HangLeft(a, Insert(a->left, val));
        }
        Update(a);
        return Rebalance(a);
    }

    Node* Delete(Node* a) {
        Node* parent = a->parent;
        if (parent) {
            if (parent->left == a) {
                parent->left = nullptr;
            }
            else {
                parent->right = nullptr;
            }
        }
        delete a;
        a = parent;
        if (!a) {
            return nullptr;
        }
        while (true) {
            Update(a);
            a = Rebalance(a);
            if (a->parent) {
                a = a->parent;
            }
            else {
                return a;
            }
        }
    }

    Node* Erase(Node* a, const T& val) {
        if (a->value == val) {
            if (GetSize(a) == 1) {
                return Delete(a);
            }
            else if (a->left) {
                auto b = GetRight(a->left);
                std::swap(b->value, a->value);
                return Erase(b, val);
            }
            else {
                auto b = GetLeft(a->right);
                std::swap(b->value, a->value);
                return Erase(b, val);
            }
        }
    }

    void DeleteTree(Node* node) {
        if (!node) {
            return;
        }
        DeleteTree(node->left);
        DeleteTree(node->right);
        delete node;
        node = nullptr;
    }

    Node* CopyTree(Node* node) {
        if (!node) {
            return node;
        }
        Node* result = new Node(*node);
        CopyTree(result->left);
        CopyTree(result->right);
        if (result->left) {
            result->left->parent = result;
        }
        if(result->right) {
            result->right->parent = result;
        }
        return result;
    }


private:
    Node* root_ = nullptr;

public:

    struct iterator {
    public:
        iterator() : v_(nullptr), root_(nullptr) {}
        iterator(Node* v, Node* root) : v_(v), root_(root) {}
        iterator(Node* root) : v_(nullptr), root_(root) {}

        const T& operator * () const {
            return v_->value;
        }

        bool operator == (const Node* b) const {
            return b->value == v_->value;
        }

        bool operator != (const Node* b) const {
            return b->value != v_->value;
        }

        iterator& operator -- () {
            if (!v_) {
                return *this;
            }
            if (v_->left) {
                v_ = GetRight(v_->left);
            }
            else {
                while (v_->parent) {
                    if (v_->parent->right == v_) {
                        v_ = v_->parent;
                        break;
                    }
                    v_ = v_->parent;
                }
            }
            return *this;
        }

        iterator& operator ++ () {
            if (!v_) {
                return *this;
            }
            if (v_->right) {
                v_ = GetLeft(v_->right);
            }
            else {
                while (v_->parent) {
                    if (v_->parent->left == v_) {
                        v_ = v_->parent;
                        break;
                    }
                    v_ = v_->parent;
                }
            }
            return *this;
        }

    private:
        Node* v_;
        Node* root_;
    };

    Set() {}

    Set(std::initializer_list<T> elem) {
        for (const T& el : elem) {
            insert(el);
        }
    }

    Set(const Set& st) {
        root_ = CopyTree(st.root_);
    }

    Set& operator = (const Set& st) {
        if (this != &st) {
            DeleteTree(root_);
            root_ = CopyTree(root_);
        }
        return *this;
    }

    size_t size() const {
        return GetSize(root_);
    }

    bool empty() const {
        return size() == 0;
    }

    bool contains(const T& val) const {
        Node* node = root_;
        while (node) {
            if (node->value == val) {
                return true;
            }
            else if (node->value < val) {
                node = node->right;
            }
            else {
                node = node->left;
            }
        }
        return false;
    }

    void insert(const T& val) {
        root_ = Insert(root_, val);
    }

    void erase(const T& val) {
        if (!contains(val)) {
            return;
        }
        root_ = Erase(root_, val);
    }

    void erase(const iterator& it) {
        bool contains = true;
        const T val = *it;
        Node* res = Erase(root_, val, contains);
        if (contains) {
            root_ = res;
        }
    }

    void clear() {
        DeleteTree(root_);
    }

    iterator begin() const {
        return iterator(GetLeft(root_), root_);
    }

    iterator end() const {
        return iterator(root_);
    }

    iterator find(const T& val) {
        Node* node = root_;
        while (node) {
            if (node->value == val) {
                break;
            }
            else if(node->value > val) {
                node = node->left;
            }
            else {
                node = node->right;
            }
        }
        return iterator(node, root_);
    }

    iterator lower_bound(const T& val) const {
        Node* node = root_;
        Node* res = nullptr;
        while (node) {
            if (node->value >= val) {
                if (!res || node->value < res->value) {
                    res = node;
                }
                node = node->left;
            }
            else {
                node = node->right;
            }
        }
        return iterator(res, root_);
    }

    iterator upper_bound(const T& val) const {
        Node* node = root_;
        Node* res = nullptr;
        while (node) {
            if (node->value > val) {
                if (!res || node->value < res->value) {
                    res = node;
                }
                node = node->left;
            }
            else {
                node = node->right;
            }
        }
        return iterator(res, root_);
    }

    iterator upper_bound(const iterator& it) const {
        const T val = *it;
        Node* node = root_;
        Node* res = nullptr;
        while (node) {
            if (node->value > val) {
                if (!res || node->value < res->value) {
                    res = node;
                }
                node = node->left;
            }
            else {
                node = node->right;
            }
        }
        return iterator(res, root_);
    }

    ~Set() {
        DeleteTree(root_);
    }
};
