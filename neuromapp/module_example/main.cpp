#include "utils/module/module_loader.hpp"
#include "utils/storage/storage.h"
#include <iostream>
/*
 * Testing module pipelining using passive membrane equation
 * We use 3 modules that self register themself when loaded as dll
 */

int main (int argc, char** argv) {
   // contrary to get, create_new return an error if a key with this name already exist
    double t = 0;
    double delta_t = 0.025;
    storage_register ("time",        (void*) &t);
    mapp_module::Library* lib;
    lib = mapp_module::from("./libfixed_pulse.dylib");
    if (! lib) return 0;
    int fixed_pulse        = lib->create("time dV_pulse ");
    
    lib = mapp_module::from("./libpassive_membrane.dylib");
    if (! lib) return 0;
    int passive_membrane   = lib->create("time dV_pulse V_passive");

    lib = mapp_module::from("./libbasic_validator.dylib");
    if (! lib) return 0;
    int validator          = lib->create("time V_passive dV_pulse");
 
    for (t = 0; t < 1; t+=delta_t) {
        mapp_module::execute (fixed_pulse);
        mapp_module::execute (passive_membrane);
        mapp_module::execute (validator);
    }
    return 0;
}
