#include "utils/module/module.h"
#include "utils/module/module_loader.hpp"

#include <vector>
#include <map>
#include <stack>
#include <string>
#include <iostream>
#include <dlfcn.h>


/*
 * Test case: 
 *  Module m = from (filename).module();
 *  int status = m.configure(m, argc, argv);
 *  if (status == MMAPP_OK)
 *      m.execute();
 */


static std::map <std::string, Library>  libraries;
static std::stack <int>                 freehandles;
static std::vector<void*>               lib_handles;
static std::vector<ModuleConfigure>     configures;
static std::vector<ModuleExecute>       executes;
static std::vector<ModuleHelp>          helps;


Library* from (const char* filename) {
    std::string key_name = "";
    if (filename == NULL) {
            std::cerr << "ERROR: Library from empty filename (global scope) is not supported" << std::endl;
            return nullptr;
    }
    key_name.assign(filename);
    if (! libs.find(key_name)) {
        void* lib = dlopen (filename, RTLD_LOCAL| RTLD_LAZY);
        if (! lib) {
            std::cerr << dlerror() << std::endl;
            return nullptr;
        }
        ModuleConfigure conf = (ModuleConfigure) dlsym(lib, "configure");
        ModuleExecute   exec = (ModuleExecute) dlsym(lib, "execute");
        ModuleHelp      help = (ModuleHelp) dlsym(lib, "help");
        int handle = -1;
        if (freehandles.size()) {
            handle = freehandles.pop();
            lib_handles[handle] = lib;
            configures[handle]  = conf;
            executes[handle]    = exec;
            helps[handle]       = help;
        } else {
            lib_handles.push_back(lib);
            configures.push_back(conf);
            executes.push_back(exec);
            helps.push_back(help);
            handle = lib_handles.size() -1;
        }
        libs[key_name] = Library(key_name, handle);
    }
    return &libs[key_name];
}

static void close (Library& lib) {
    dlclose (lib_handles[lib.id]);
    lib_handles[lib.id] = nullptr;
    configures[lib.id]  = nullptr;
    executes[lib.id]    = nullptr;
    helps[lib.id]       = nullptr;
    libraries.erase(lib.lib_name);
    freehandles.push(lib.id);
}


Module Library::module () {
    return Module { .configure = configures[id], 
                    .execute = executes[id], 
                    .help = helps[id]
                  };
}

Library::Library(const char* filename, int id): lib_name(filename), id(id) {
}

Library::~Library() {
    close (*this);
}
