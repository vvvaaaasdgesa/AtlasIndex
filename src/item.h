#pragma once
#include <string>
#include <memory>
#include <iosfwd>

enum class Category { Book, Clothing, Electronics };

struct Item {
    std::string name;
    Category    cat;
    double      price;
};

class ItemHandle {
public:
    // Constructors
    explicit ItemHandle(std::string name);
    ItemHandle(Item v);

    // Copy: deep copy to support use in STL containers, BST, sorts, etc.
    ItemHandle(const ItemHandle& other);
    ItemHandle& operator=(const ItemHandle& other);

    // Move: default is fine
    ItemHandle(ItemHandle&&) noexcept = default;
    ItemHandle& operator=(ItemHandle&&) noexcept = default;

    ~ItemHandle() = default;

    const std::string& name() const;
    Category           category() const;   // if name-only, returns Category::Book (sentinel)
    double             price() const;      // 0.0 if name-only
    const Item*        get() const;
    void               print(std::ostream& os) const;

private:
    std::unique_ptr<Item> ptr_;
    std::string           sentinelName_;

    static std::unique_ptr<Item> clonePtr(const std::unique_ptr<Item>& p);
};

bool operator<(const ItemHandle& a, const ItemHandle& b);
bool operator==(const ItemHandle& a, const ItemHandle& b);
inline bool operator!=(const ItemHandle& a, const ItemHandle& b) { return !(a==b); }

std::ostream& operator<<(std::ostream& os, const ItemHandle& h);
