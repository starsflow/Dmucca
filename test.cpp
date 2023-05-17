//
// Created by lxc on 5/16/23.
//

#include "YTable.h"

int main() {
    Database *db;
    auto* table = reinterpret_cast<YTable *>(new Table<int, int>(db, 0));
    table->initialize_table();
}