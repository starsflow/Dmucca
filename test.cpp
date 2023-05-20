//
// Created by lxc on 5/16/23.
//

#include "YTable.h"

int main() {
    auto *db = new Database();
    auto *table = static_cast<YTable *>(db->create_table<int, int>());
    table->initialize_table();
    // table->print_table();

    auto *res = static_cast<YValue *>(table->search_value(99));
    table->erase(200);
    table->print_table();
    return 0;
}