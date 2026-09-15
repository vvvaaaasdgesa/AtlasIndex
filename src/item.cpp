#include "item.h"
#include <ostream>

ItemHandle::ItemHandle(std::string name) : sentinelName_(std::move(name)) {}
ItemHandle::ItemHandle(Item v) : ptr_(std::make_unique<Item>(std::move(v))) {}

std::unique_ptr<Item> ItemHandle::clonePtr(const std::unique_ptr<Item>& p) {
    if (!p) return nullptr;
    return std::make_unique<Item>(*p); // deep copy
}

ItemHandle::ItemHandle(const ItemHandle& other)
: ptr_(clonePtr(other.ptr_)), sentinelName_(other.sentinelName_) {}

ItemHandle& ItemHandle::operator=(const ItemHandle& other) {
    if (this == &other) return *this;
    ptr_ = clonePtr(other.ptr_);
    sentinelName_ = other.sentinelName_;
    return *this;
}

const std::string& ItemHandle::name() const {
    return ptr_ ? ptr_->name : sentinelName_;
}
Category ItemHandle::category() const {
    return ptr_ ? ptr_->cat : Category::Book;
}
double ItemHandle::price() const {
    return ptr_ ? ptr_->price : 0.0;
}
const Item* ItemHandle::get() const { return ptr_.get(); }

void ItemHandle::print(std::ostream& os) const {
    const char* catStr = (category() == Category::Book ? "Book" :
                         (category() == Category::Clothing ? "Clothing" : "Electronics"));
    os << "Category: " << catStr
       << " | Name: "  << name()
       << " | Price : " << price();
}

bool operator<(const ItemHandle& a, const ItemHandle& b) {
    if (a.name() != b.name()) return a.name() < b.name();
    if (a.category() != b.category()) return static_cast<int>(a.category()) < static_cast<int>(b.category());
    return a.price() < b.price();
}
bool operator==(const ItemHandle& a, const ItemHandle& b) {
    return a.name() == b.name() && a.category() == b.category();
}
std::ostream& operator<<(std::ostream& os, const ItemHandle& h) {
    h.print(os);
    return os;
}
