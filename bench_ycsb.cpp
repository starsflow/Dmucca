/*
 * @Author: lxc
 * @Date: 05/16/2023
 */
#include "Global.h"
#include "YCSB/YTable.h"
#include "YCSB/YTransaction.h" 
#include "YCSB/YWorkload.h"
#include "Counter.h"
#include "TwoPL/TwoPL.h"
#include "TwoPL/TwoPLExecutor.h"
#include "TwoPL/TwoPLTransaction.h"

std::atomic<uint32_t> Counter::_counter = 0;

void signal_handler(int signal) {
    google::ShutdownGoogleLogging();
    std::cout << "logs flush into file!" << std::endl;
    std::exit(0);
}

int main(int argc, char* argv[]) {
    //initialize glog
    std::string root_path(argv[1]);
    std::string log_path(root_path + "/log/tpl_");
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::INFO, log_path.c_str());
    signal(SIGINT, signal_handler);

    // initialize the database and table
    auto *db = new Database();
    auto *table = static_cast<YTable *>(db->create_table<YKey, YValue>());
    table->initialize_table();
    // table->print_table();

    // initialize the task queue
    auto* execute_queue = new SafeQueue<YTransaction<TwoPLTransaction>>();
    auto* commit_queue = new SafeQueue<YTransaction<TwoPLTransaction>>();

    //generate workload
    auto workload = YWorkload<TwoPLTransaction>(db, execute_queue);
    workload.generate_workload_thread(10000);

    //start tpl server
    TwoPL<YWorkload<TwoPLTransaction>::TransactionType> tpl(db, execute_queue, commit_queue);
    tpl.run();

    return 0;
}