
#include <sys/stat.h>
#include "global.h"
#include "db/Database.h"
#include "gr/Router.h"

void signalHandler(int signum) {
    std::cout << "Signal (" << signum << ") received. Exiting...\n";

    std::exit(signum);
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    srand (time(NULL));

    std::cout << std::boolalpha;  // set std::boolalpha to std::cout
    
    log() << "-----------start-------------" << std::endl;

    //-------------------------- input ---------------------------
    string netFile      = string(argv[1]);
    string output_path      = string(argv[2]);

    std::string preprefix = netFile.substr(0,netFile.find_last_of('.'));
    std::string prefix = preprefix.substr(0,preprefix.find_last_of('.'));
    std::string design = prefix.substr(prefix.find_last_of('/')+1);

    utils::timer runtime;

    //------------------------ setup log file --------------------- 
    const std::string& result = "results";
    const std::string& result_dir = "../" + result + "/";

    struct stat sb;
    stat(result_dir.c_str(), &sb);
    if (!S_ISDIR(sb.st_mode)) {
        mkdir(result_dir.c_str(), 0777);
    }
    
    utils::logger logger(result_dir, design);

    //-------------------------- read -----------------------------
    db::Database database;
    database.logger = &logger;
    database.designName = design;
    database.read(netFile);

    //-------------------------- load -----------------------------
    gr::Router router(&database);

    // db::Net* net = router.net_queue[1];
    // router.single_net_pattern(net);

    // for(db::Net* net : router.net_queue) {
    //     router.single_net_pattern(net);
    // }
    // router.print_demand();
    // // router.write(output_path);
    
    // // router.unroute_net(net);
    // // for(db::Net* net : database.nets) {
    // //     cout << 
    // //     router.unroute_net(net);
    // // }

    // router.break_ovfl();
    // router.print_demand();
    // // router.write(output_path);

    gr::Point a(3,2);
    gr::Point b(5,1);
    cout << a-b<< endl;

    db::Net* net = router.net_queue[687];
    router.single_net_maze(net);

    return 0;
}