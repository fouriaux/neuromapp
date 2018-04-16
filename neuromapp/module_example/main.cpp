#include "utils/module/module_loader.hpp"
#include "utils/storage/storage.h"
#include <iostream>
/*
 * Testing module pipelining using passive membrane equation
 * We use 3 modules that self register themself when loaded as dll
 */

int main (int argc, char** argv) {
    double t = 0;
    double delta_t = 0.025;
    mapp_module::use("./");
    storage_register ("time",        (void*) &t);

    int fixed_pulse        = mapp_module::from("fixed_pulse")->create("time dV_pulse ");
    int passive_membrane   = mapp_module::from("passive_membrane")->create("time dV_pulse V_passive");
    int validator          = mapp_module::from("basic_validator")->create("time V_passive dV_pulse");

    for (t = 0; t < 1; t+=delta_t) {
        mapp_module::execute (fixed_pulse);
        mapp_module::execute (passive_membrane);
        mapp_module::execute (validator);
    }
    return 0;
}
