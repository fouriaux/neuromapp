#pragma once


#include <string>

namespace mapp_module {

    typedef int ModuleId;

    struct Library {
        friend Library from (const char* filename);
        public:
        //TODO        void createAs(const char* name);
        ModuleId create (const char* args);
        ~Library();
        private:
        Library (const char* filename, int id);
        int id;
        std::string lib_name;
    };

    Library* from (const char* filename);
    mapp_error execute   (ModuleId);
    mapp_error configure (ModuleId);
    mapp_error help      (ModuleId);
}
