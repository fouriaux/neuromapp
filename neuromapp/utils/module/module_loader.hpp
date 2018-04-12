#pragma once


#include <string>

namespace mapp_module {
    class Library {
        friend Library* from (const char* filename);
        public:
            int create (const char* args);
            ~Library();
            const int id;
            const std::string lib_name;
        private:
            Library (std::string filename, int id);
    };

    Library* from (const char* filename);
    int  execute  (int);
    void help     (int);
}
