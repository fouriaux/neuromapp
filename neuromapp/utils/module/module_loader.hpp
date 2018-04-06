#pragma once

#include "utils/module/module.h"

struct LibraryHandler {
    public:
        Module* get (const char* name);
    private:
        int internal_impl;
};

LibraryHandler* from (const char* file);
