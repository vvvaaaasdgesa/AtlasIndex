#include "bst.h"

BST::BST() = default;
BST::~BST() { clear(root_); }

void BST::clear(Node* n) {
    if (!n) return;
    clear(n->left);
    clear(n->right);
    delete n;
}

BST::Node* BST::insertNode(Node* n, const ItemHandle& v, bool& added) {
    if (!n) {
        added = true;
        return new Node(v);
    }
    // Duplicates use name and category only, so price is ignored here.
    if (v == n->val) return n;
    if (v < n->val) n->left = insertNode(n->left, v, added);
    else n->right = insertNode(n->right, v, added);
    return n;
}

void BST::insert(const ItemHandle& v) {
    bool added = false;
    root_ = insertNode(root_, v, added);
    if (added) ++size_;
}

bool BST::containsNode(Node* n, const ItemHandle& v) {
    if (!n) return false;
    if (v == n->val) return true;
    return v < n->val ? containsNode(n->left, v)
                      : containsNode(n->right, v);
}
bool BST::contains(const ItemHandle& v) const { return containsNode(root_, v); }

void BST::inorderWalk(Node* n, std::vector<ItemHandle>& out) {
    if (!n) return;
    inorderWalk(n->left, out);
    out.push_back(n->val);
    inorderWalk(n->right, out);
}
void BST::inorder(std::vector<ItemHandle>& out) const { out.clear(); inorderWalk(root_, out); }

void BST::rangeWalk(Node* n, const ItemHandle& low, const ItemHandle& high,
                    std::vector<ItemHandle>& out) {
    if (!n) return;
    if (low < n->val) rangeWalk(n->left, low, high, out);
    if (!(n->val < low) && !(high < n->val)) out.push_back(n->val);
    if (n->val < high) rangeWalk(n->right, low, high, out);
}

void BST::range(const ItemHandle& low, const ItemHandle& high,
                std::vector<ItemHandle>& out) const {
    out.clear();
    if (high < low) return;
    rangeWalk(root_, low, high, out);
}

void BST::nameRangeWalk(Node* n, const std::string& first,
                        const std::string& last,
                        std::vector<ItemHandle>& out) {
    if (!n) return;
    if (first < n->val.name()) nameRangeWalk(n->left, first, last, out);
    if (first <= n->val.name() && n->val.name() <= last) out.push_back(n->val);
    if (n->val.name() < last) nameRangeWalk(n->right, first, last, out);
}

void BST::rangeByName(const std::string& first, const std::string& last,
                      std::vector<ItemHandle>& out) const {
    out.clear();
    if (last < first) return;
    nameRangeWalk(root_, first, last, out);
}

bool BST::isValid(Node* n, const ItemHandle* lo, const ItemHandle* hi) {
    if (!n) return true;
    // Both bounds are exclusive because the BST ordering is strict.
    if (lo && !(*lo < n->val)) return false;
    if (hi && !(n->val < *hi)) return false;
    return isValid(n->left, lo, &n->val) &&
           isValid(n->right, &n->val, hi);
}
bool BST::isValidBST() const { return isValid(root_, nullptr, nullptr); }

// Detach the smallest node and return the new root of this subtree.
BST::Node* BST::detachMin(Node* n, Node*& minNode) {
    if (!n->left) {
        minNode = n;
        return n->right;
    }
    n->left = detachMin(n->left, minNode);
    return n;
}

BST::Node* BST::eraseNode(Node* n, const ItemHandle& v, bool& erased) {
    if (!n) return nullptr;

    if (v == n->val) {
        erased = true;
        if (!n->left) {
            Node* right = n->right;
            delete n;
            return right;
        }
        if (!n->right) {
            Node* left = n->left;
            delete n;
            return left;
        }

        // Replace this node with the smallest node from its right subtree.
        Node* successor = nullptr;
        Node* updatedRight = detachMin(n->right, successor);
        successor->left = n->left;
        successor->right = updatedRight;
        delete n;
        return successor;
    }

    if (v < n->val) n->left = eraseNode(n->left, v, erased);
    else n->right = eraseNode(n->right, v, erased);
    return n;
}

bool BST::erase(const ItemHandle& v) {
    bool erased = false;
    root_ = eraseNode(root_, v, erased);
    if (erased) --size_;
    return erased;
}
