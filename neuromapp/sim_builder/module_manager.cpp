#include <iostream>
#include "utils/module/module_loader.hpp"
#ifdef NEUROMAPP_CURSOR
    #include <readline/readline.h>
    #include <readline/history.h>
#endif

/*
 * Module Manager act on top of module loader and abstract communications with driver
 * The idea is: 
 *              1. to sandbox loaded modules
 *              2. to be able to remote execute => Daemon + cli with Local domain sockets
 *
 */

#include <vector>

static int t;
static int dt;
static int final_t;
std::vector<int> exec_queue_handler;
std::vector<ModuleExecute> exec_queue;
int run () {
       while (t < final_t) {
            for (auto& exec : exec_queue)
                exec();
            final_t+=dt;
       }
}

int addModule (const char* library_name, const char* options) {
    // TODO do configure and enqueue exec into exec_queue
}

void showModuleHelp (int handler) {
    get_help (handler)();
}

void showLoadedModules () {
    for (int i = 0; i < exec_queue_handler.size(); i++) {
        std::cout << exec_queue_handler[i] << "> " << get_library_name (exec_queue_handler[i]) << std::endl;
    }
}

void help () {
    std::cout << "avail commands: " << std::endl;
    std::cout << "\tl" << std::endl;
    std::cout << "\th" << std::endl;
    std::cout << "\ts" << std::endl;
    std::cout << "\tr" << std::endl;
    std::cout << "\t?" << std::endl;
    std::cout << "\tq" << std::endl;
}

int main (int argc, char** argv) {
    help();
    while (1) {
    // listening commands
#ifdef NEUROMAPP_CURSOR
    char* input = readline("");
    add_history(input);
    std::string command(input);
#else
    std::string command;
    std::getline(std::cin, command);
#endif
    // extract command and args from string filled by std::cin
    std::vector<std::string> command_v;
    command_v.push_back(argv[0]);
    std::istringstream command_stream(command);
    std::istream_iterator<std::string> wb(command_stream),we;
    std::copy(wb,we,std::back_inserter(command_v));
    mapp::argv_data A(command_v.begin(),command_v.end());
    switch (commands_v[1][0]) {
        case 'q':
            return 0;
            break;
        case 'l':
            addModule(A.argc(), A.argv());
            break;
        case 'h':
            showModuleHelp(A.argc(), A.argv());
            break;
        case 's':
            showLoadedModules(A.argc(), A.argv());
            break;
       case  'r':
            run ();
            break;
       case '?':
       default:
            help();
    }
 }
}
