#include <iostream>
#include "utils/module/module_loader.hpp"

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
    //TODO display allowed commands
}
int main (int argc, char** argv) {
 while (1) {
    // listening commands
    // extract command and args from string filled by std::cin
    switch (command) {
        case 'q':
            return 0;
            break;
        case 'l':
            addModule(argv[1], argv[2]);
            break;
        case 'h':
            showModuleHelp(argv[1], argv[2]);
            break;
        case 's':
            showLoadedModules(argv[1], argv[2]);
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
