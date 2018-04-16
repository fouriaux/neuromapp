#include "utils/module/module.h"
#include "utils/module/module_loader.hpp"

#include <vector>
#include <map>
#include <list>
#include <stack>
#include <string>
#include <iostream>
#include <sstream>
#include <dlfcn.h>


static std::map <std::string, mapp_module::Library*>  libraries;
static std::stack <int>                 freehandles;
static std::vector<void*>               lib_handles;
static std::vector<ModuleConfigure>     configures;
static std::vector<ModuleExecute>       executes;
static std::vector<ModuleHelp>          helps;
static std::string module_path = std::string("./");

static std::string get_lib_path (const char* filename) {
    std::string result = "";
#ifdef __APPLE__

#else // Linux world
    
#endif
    result += module_path;
    result += "lib";
    result += filename;
    result += ".so";
    return result;
}

namespace mapp_module {
    int execute   (int id)  {return executes[id](); }
    void help      (int id) {return helps[id](); }
    void use (const char* path) {
        module_path = std::string(path);
    }
    Library* from (const char* filename) {
        std::string key_name = "";
        if (filename == NULL) {
            std::cerr << "ERROR: Library from empty filename (global scope) is not supported" << std::endl;
            return nullptr;
        }
        key_name.assign(filename);
        if (libraries.find(key_name) == libraries.end()) {
            void* lib = dlopen (get_lib_path(filename).c_str(), RTLD_LOCAL| RTLD_LAZY);
            if (! lib) {
                std::cerr << dlerror() << std::endl;
                return nullptr;
            }
            ModuleConfigure conf = (ModuleConfigure) dlsym(lib, "configure");
            ModuleExecute   exec = (ModuleExecute) dlsym(lib, "execute");
            ModuleHelp      help = (ModuleHelp) dlsym(lib, "help");
            int handle = -1;
            if (freehandles.size()) {
                handle = freehandles.top();
                freehandles.pop();
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
            Library* l = new Library(key_name, handle);
            libraries[key_name] = l;
        }
        return libraries[key_name];
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

    int Library::create (const char* args) {
        std::vector<char *> argv;
        std::istringstream ss(args);
        std::string arg;
        std::list<std::string> ls;
        while (ss >> arg)
        {
            ls.push_back(arg);
            argv.push_back(const_cast<char*>(ls.back().c_str()));
        }
        argv.push_back(0);  // need terminating null pointer 
        configures[id] (argv.size(), argv.data());
        return id;
    }

    Library::Library(std::string filename, int id): lib_name(filename), id(id){}

    Library::~Library() {
        close (*this);
    }
} // namespace mapp_module
