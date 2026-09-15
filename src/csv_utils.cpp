#include "csv_utils.h"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace csv_utils {

bool parseCategory(const std::string& s, Category& out) {
    if (s == "Book") { out = Category::Book; return true; }
    if (s == "Clothing") { out = Category::Clothing; return true; }
    if (s == "Electronics") { out = Category::Electronics; return true; }
    return false;
}

static bool parseLine(const std::string& line, ItemHandle& out) {
    // Expect: name,category,price
    std::stringstream ss(line);
    std::string name, catStr, priceStr;
    if (!std::getline(ss, name, ',')) return false;
    if (!std::getline(ss, catStr, ',')) return false;
    if (!std::getline(ss, priceStr)) return false;
    // trim spaces (basic)
    auto trim = [](std::string& t){
        while (!t.empty() && (t.front()==' '||t.front()=='\t')) t.erase(t.begin());
        while (!t.empty() && (t.back()==' '||t.back()=='\t' || t.back()=='\r')) t.pop_back();
    };
    trim(name); trim(catStr); trim(priceStr);
    Category cat;
    if (!parseCategory(catStr, cat)) return false;
    try {
        double price = std::stod(priceStr);
        Item v{ name, cat, price };
        out = ItemHandle(v);
        return true;
    } catch (...) {
        return false;
    }
}

bool readCSV(const std::string& path, std::vector<ItemHandle>& out, size_t& skipped) {
    skipped = 0;
    std::ifstream fin(path);
    if (!fin) return false;
    std::string line;
    while (std::getline(fin, line)) {
        if (line.size() == 0) continue;
        ItemHandle h(std::string(""));
        if (parseLine(line, h)) out.push_back(h);
        else ++skipped;
    }
    return true;
}

} // namespace csv_utils
