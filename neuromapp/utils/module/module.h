#ifndef NEUROMAPP_MODULE_HPP
#define NEUROMAPP_MODULE_HPP

typedef int  (*ModuleConfigure)(int argc, char** argv); // expected to be named configure
typedef int  (*ModuleExecute)();                        // expected to be named execute
typedef void (*ModuleHelp)();                           // expected to be named help

#endif
