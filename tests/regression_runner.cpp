//
// Lightweight regression runner for the core data structures and engine.
//
// Build from the project root:
//   cmake -S . -B build && cmake --build build
//
// Run from the project root so data/sample_small.csv can be found:
//   ./build/atlasindex_regression
//
// This executable complements the focused GoogleTest unit suite with an
// end-to-end smoke test and human-readable output.
//
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <sstream>
#include <iomanip>
#include <cassert>

#include "item.h"
#include "hash_table.h"
#include "bst.h"
#include "sorts.h"
#include "engine.h"

struct Suite {
    int passed = 0;
    int failed = 0;
    void ok(bool cond, const std::string& name) {
        if (cond) { ++passed; std::cout << "  [PASS] " << name << "\n"; }
        else      { ++failed; std::cout << "  [FAIL] " << name << "\n"; }
    }
    void section(const std::string& title) {
        std::cout << "\n== " << title << " ==\n";
    }
    void summary() const {
        std::cout << "\n==== SUMMARY ====  Passed: " << passed << "  Failed: " << failed << "\n";
    }
};

static std::string toString(const ItemHandle& h) {
    std::ostringstream os;
    os.setf(std::ios::fmtflags(0), std::ios::floatfield);
    h.print(os);
    return os.str();
}

int main() {
    Suite T;

    // -------- Item & printing --------
    T.section("ItemHandle & Printing");
    {
        ItemHandle a(Item{ "The AI Era", Category::Book, 12.50 });
        T.ok(a.name()=="The AI Era", "name getter");
        T.ok(a.category()==Category::Book, "category getter");
        T.ok(a.price()==12.50, "price getter");
        T.ok(toString(a)=="Category: Book | Name: The AI Era | Price : 12.5", "print format");
        ItemHandle sentinel(std::string("OnlyName"));
        T.ok(sentinel.get()==nullptr, "name-only sentinel has null ptr");
        T.ok(toString(sentinel)=="Category: Book | Name: OnlyName | Price : 0", "print sentinel");
        ItemHandle b(Item{ "ABC", Category::Clothing, 10.0 });
        T.ok( (b < a) == (std::string("ABC") < std::string("The AI Era")), "operator< by name");
    }

    // -------- HashTable --------
    T.section("HashTable basic ops");
    {
        HashTable ht(1); // force collisions
        ItemHandle x(Item{ "X", Category::Book, 1.0 });
        ItemHandle y(Item{ "Y", Category::Clothing, 2.0 });
        ItemHandle y2(Item{ "Y", Category::Electronics, 3.0 });
        T.ok(ht.insert("X", x) == true, "insert new X");
        T.ok(ht.insert("Y", y) == true, "insert new Y");
        T.ok(ht.insert("Y", y2) == false, "update existing Y");
        auto* pY = ht.find("Y");
        T.ok(pY && pY->price()==3.0, "find updated Y");
        T.ok(ht.erase("X") == true, "erase X");
        T.ok(ht.erase("X") == false, "erase X again -> false");
        T.ok(ht.find("X") == nullptr, "find X after erase");
    }

    // -------- BST --------
    T.section("BST insert/contains/inorder/erase");
    {
        BST bst;
        ItemHandle a(Item{ "A", Category::Book, 1.0 });
        ItemHandle b(Item{ "B", Category::Book, 1.0 });
        ItemHandle c(Item{ "C", Category::Book, 1.0 });
        ItemHandle d(Item{ "D", Category::Book, 1.0 });
        // Build tree: insert B, A, D, C to cover various shapes
        bst.insert(b);
        bst.insert(a);
        bst.insert(d);
        bst.insert(c);
        T.ok(bst.contains(a), "contains A");
        T.ok(bst.contains(b), "contains B");
        T.ok(bst.contains(c), "contains C");
        T.ok(bst.contains(d), "contains D");
        std::vector<ItemHandle> v;
        bst.inorder(v);
        T.ok(v.size()==4 && v[0].name()=="A" && v[3].name()=="D", "inorder sorted");
        // Erase leaf (C is leaf under D-left)
        T.ok(bst.erase(c)==true, "erase leaf C");
        T.ok(!bst.contains(c), "after erase C not found");
        // Erase one-child (D now likely has only right child or none depending)
        T.ok(bst.erase(d)==true, "erase node D");
        // Erase root with two children (B has A as left; currently may be one-child; rebuild to ensure two-child)
        BST bst2;
        ItemHandle n10(Item{ "M", Category::Book, 1.0 });
        ItemHandle n05(Item{ "F", Category::Book, 1.0 });
        ItemHandle n15(Item{ "T", Category::Book, 1.0 });
        ItemHandle n12(Item{ "R", Category::Book, 1.0 });
        ItemHandle n20(Item{ "Z", Category::Book, 1.0 });
        bst2.insert(n10); bst2.insert(n05); bst2.insert(n15); bst2.insert(n12); bst2.insert(n20);
        T.ok(bst2.isValidBST(), "valid before erase(two-child)");
        T.ok(bst2.erase(n15)==true, "erase two-child node (successor path)");
        T.ok(bst2.isValidBST(), "valid after erase(two-child)");
    }

    // -------- Sorts --------
    T.section("Insertion & Merge sort");
    {
        std::vector<ItemHandle> a = {
            ItemHandle(Item{ "B", Category::Book, 2.0 }),
            ItemHandle(Item{ "A", Category::Book, 3.0 }),
            ItemHandle(Item{ "A", Category::Book, 1.0 }), // same key as previous except price -> tests tie-break
            ItemHandle(Item{ "C", Category::Book, 0.5 })
        };
        auto run = [&](SortAlgo algo, bool asc) {
            auto v = a;
            sortItems(v, algo, asc);
            return v;
        };
        auto v1 = run(SortAlgo::Insertion, true);
        auto v2 = run(SortAlgo::Merge, true);
        T.ok(v1.size()==4 && v1[0].name()=="A" && v1[3].name()=="C", "insertion asc order");
        T.ok(v2.size()==4 && v2[0].name()=="A" && v2[3].name()=="C", "merge asc order");
        auto v3 = run(SortAlgo::Insertion, false);
        T.ok(v3[0].name()=="C" && v3[3].name()=="A", "insertion desc order");
    }

    // -------- Engine & CSV --------
    T.section("Engine load/find/erase/list");
    {
        Engine e(8);
        e.loadCSV("data/sample_small.csv");
        const ItemHandle* p = e.findByName("Hoodie");
        T.ok(p != nullptr, "find Hoodie after load");
        std::vector<ItemHandle> out;
        e.listInOrder(out);
        T.ok(out.size() >= 3, "list after load has at least 3");
        T.ok(e.eraseByName("Hoodie")==true, "erase Hoodie");
        T.ok(e.findByName("Hoodie")==nullptr, "not found after erase");
    }

    T.summary();
    return T.failed == 0 ? 0 : 1;
}
