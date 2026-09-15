#pragma once
#include "item.h"
#include <vector>

enum class SortAlgo { Insertion, Merge };

void insertionSortItems(std::vector<ItemHandle>& a, bool ascending);
void mergeSortItems(std::vector<ItemHandle>& a, bool ascending);
void sortItems(std::vector<ItemHandle>& a, SortAlgo algo, bool ascending);
