//
// Created by lxc on 5/16/23.
//

#include "YTable.h"

int main() {
    auto *db = new Database();
    auto *table = static_cast<YTable *>(db->create_table<int, int>());
    table->initialize_table();
    table->print_table();

    auto *res = static_cast<YValue *>(table->search(99));
    LOG(INFO) << res->Y_F01;
    return 0;
}