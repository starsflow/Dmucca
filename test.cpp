//
// Created by lxc on 5/16/23.
//

#include "YTable.h"

int main() {
    auto *db = new Database();
    auto* table = reinterpret_cast<YTable *>(db->create_table<int, int>());
    table->initialize_table();
    return 0;
}