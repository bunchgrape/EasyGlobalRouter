

#include "global.h"
#include <sys/stat.h>
#include <iostream>

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


    const std::string& result = "results";
    const std::string& result_dir = "../" + result + "/";
    
    log() << result_dir << endl;

    struct stat sb;
    struct stat buf;
    stat(result_dir.c_str(), &sb);

    log() << S_ISDIR(sb.st_mode) << endl;

    if (!S_ISDIR(sb.st_mode)) {
        mkdir(result_dir.c_str(), 0777);
    }

    utils::logger logger(result_dir, "test");

    
    logger.info() << "test" << "test2\n";

    logger.info() << "test3" << "test4\n";

    

    return 0;
}