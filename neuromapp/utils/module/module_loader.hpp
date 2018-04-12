#pragma once


#include <string>

namespace mapp_module {

    typedef int ModuleId;

    struct Library {
        friend Library* from (const char* filename);
        public:
        ModuleId create (const char* args);
        ~Library();
        private:
        Library (const char* filename, int id);
        int id;
        std::string lib_name;
    };

    Library* from (const char* filename);
    int execute   (ModuleId);
    int help      (ModuleId);
}
