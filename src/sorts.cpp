#include "sorts.h"

namespace {

bool comesBefore(const ItemHandle& a, const ItemHandle& b, bool ascending) {
    return ascending ? a < b : b < a;
}

void mergeRange(std::vector<ItemHandle>& a, size_t begin, size_t mid,
                size_t end, bool ascending) {
    std::vector<ItemHandle> merged;
    merged.reserve(end - begin);
    size_t left = begin;
    size_t right = mid;

    while (left < mid && right < end) {
        // Take from the left on a tie to keep the sort stable.
        if (comesBefore(a[right], a[left], ascending)) {
            merged.push_back(a[right++]);
        } else {
            merged.push_back(a[left++]);
        }
    }
    while (left < mid) merged.push_back(a[left++]);
    while (right < end) merged.push_back(a[right++]);

    for (size_t i = 0; i < merged.size(); ++i) {
        a[begin + i] = merged[i];
    }
}

void mergeSortRange(std::vector<ItemHandle>& a, size_t begin, size_t end,
                    bool ascending) {
    if (end - begin < 2) return;
    const size_t mid = begin + (end - begin) / 2;
    mergeSortRange(a, begin, mid, ascending);
    mergeSortRange(a, mid, end, ascending);
    mergeRange(a, begin, mid, end, ascending);
}

} // namespace

void insertionSortItems(std::vector<ItemHandle>& a, bool ascending) {
    for (size_t i = 1; i < a.size(); ++i) {
        ItemHandle key = a[i];
        size_t j = i;
        // Only shift on a strict comparison so equal items keep their order.
        while (j > 0 && comesBefore(key, a[j - 1], ascending)) {
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;
    }
}

void mergeSortItems(std::vector<ItemHandle>& a, bool ascending) {
    mergeSortRange(a, 0, a.size(), ascending);
}

void sortItems(std::vector<ItemHandle>& a, SortAlgo algo, bool ascending) {
    switch (algo) {
        case SortAlgo::Insertion: insertionSortItems(a, ascending); break;
        case SortAlgo::Merge:     mergeSortItems(a,     ascending); break;
    }
}
