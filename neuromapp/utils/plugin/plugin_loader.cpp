#include "utils/plugin/plugin.h"
#include "utils/plugin/plugin_loader.hpp"

#include <string>
#include <iostream>
#include <dlfcn.h>

static std::vector <void*> lib_handles;
static std::map <std::string, LibraryHandler> lib_handlers;
Module* LibraryHandler::get(const char* module_name) {
    Module* result = (Module*) dlsym (lib_handles[internal_impl], module_name);
    if (! result)
        std::cerr << dlerror() << std::endl;
    return result;
}

LibraryHandler::LibraryHandler() {
    internal_impl = lib_handles.size()-1; // last element index of lib_handles
}

LibraryHandler::~LibraryHandler() {
    dlclose (lib_handles[internal_impl]);
}

LibraryHandler* from (const char* filename) {
    std::string key_name = "";
    if (filename != NULL) key_name.assign(filename);
    if (! lib_handlers.find(key_name)) {
        void* lib =dlopen (filename, RTLD_LOCAL| RTLD_LAZY);
        if (! lib) {
            std::cerr << dlerror() << std::endl;
            return nullptr;
        }
        lib_handles.push_back(lib);
        lib_handlers[key_name] = LibraryHandler();
    }
    return &lib_handlers[filename];
}
