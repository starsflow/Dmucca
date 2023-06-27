/*
 * @Author: lxc
 * @Date: 05/16/2023
 */
#include "YCSB/YTable.h"
#include "YCSB/YTransaction.h"
#include "YCSB/YWorkload.h"
#include "Counter.h"
#include "TwoPL/TwoPL.h"
#include "TwoPL/TwoPLExecutor.h"
#include "TwoPL/TwoPLTransaction.h"

std::atomic<uint32_t> Counter::_counter = 0;

int main() {
    //initialize the database and table
    auto *db = new Database();
    auto *table = static_cast<YTable *>(db->create_table<YKey, YValue>());
    table->initialize_table();
    // table->print_table();

    //initialize the task queue
    SafeQuene<TwoPLTransaction>* execute_queue = new SafeQuene<TwoPLTransaction>();
    SafeQuene<TwoPLTransaction>* commit_queue = new SafeQuene<TwoPLTransaction>();
    YContext context;
    YWorkload workload = YWorkload<TwoPLTransaction>(db, context, execute_queue);

    //generate workload
    workload.generate_workload_thread(1);

    //start tpl server
    TwoPL tpl(db, execute_queue, commit_queue);
    tpl.run();

    return 0;
}