#ifndef NEUROMAPP_PLUGIN_HPP
#define NEUROMAPP_PLUGIN_HPP

typedef int  (*ModuleConfigure)(int argc, char** argv); // expected to be named configure
typedef int  (*ModuleRun)();                            // expected to be named run
typedef void (*ModuleHelp)();                           // expected to be named help

