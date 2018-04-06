#ifndef NEUROMAPP_PLUGIN_HPP
#define NEUROMAPP_PLUGIN_HPP

typedef int (*moduleConfigure)(int argc, char** argv);
typedef int (*moduleRun)();

typedef struct Module {
    moduleConfigure configure;
    moduleRun run;
};

#endif
