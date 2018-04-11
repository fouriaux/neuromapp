#pragma once


#include <string>
#include "module.h"

struct Module {
        ModuleConfigure configure;
        ModuleExecute  execute;
        ModuleHelp     help;
};

struct Library {
friend Library from (const char* filename);
    public:
        Module module ();
        ~Library();
    private:
        Library (const char* filename, int id);
        int id;
        std::string lib_name;
};

Library* from (const char* filename);
