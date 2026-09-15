#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "item.h"

namespace csv_utils {

// Parse category from string (case-sensitive exact match: Book/Clothing/Electronics).
// Returns true on success; false on bad category.
bool parseCategory(const std::string& s, Category& out);

// Read CSV file of lines: name,category,price
// Appends parsed items to out; sets skipped for malformed lines.
bool readCSV(const std::string& path, std::vector<ItemHandle>& out, size_t& skipped);

// Return current time point for simple timing
inline std::chrono::steady_clock::time_point now() {
    return std::chrono::steady_clock::now();
}
// Milliseconds between two time points
inline long long ms_between(std::chrono::steady_clock::time_point a,
                            std::chrono::steady_clock::time_point b) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count();
}

} // namespace csv_utils
