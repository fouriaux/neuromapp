#include "utils/module/module_loader.hpp"
#include "utils/storage/storage.h"
#include <iostream>
/*
 * The Hodgkin-Huxley model
 */

int main (int argc, char** argv) {
    double t = 0;
    double delta_t = 0.025;
    mapp_module::use("./");
    storage_register ("time",        (void*) &t);

    int fixed_pulse        = mapp_module::from("fixed_pulse")->create("time dV_pulse ");
    
    int HH_potatium        = mapp_module::from("HH_potatium")->create("time dV_pulse V");
    int HH_sodium          = mapp_module::from("HH_sodium")->create("time dV_pulse V");
    int HH_leak            = mapp_module::from("HH_leak")->create("time dV_pulse V");
    
    int HH_Voltage         = mapp_module::from("HH_Voltage")->create("time dV_pulse V");
    int HH_Currents        = mapp_module::from("HH_Currents")->create("HH_sodium HH_potatium HH_leak");
    int HH_States          = mapp_module::from("HH_Update")->create("HH_sodium HH_potatium HH_leak");
    
    int Validator          = mapp_module::from("basic_validator")->create("time V dV_pulse");

    for (t = 0; t < 1; t+=delta_t) {
        mapp_module::execute (fixed_pulse);
        
        mapp_module::execute (HH_Currents);
        mapp_module::execute (HH_Voltage);
        mapp_module::execute (HH_States);

        mapp_module::execute (Validator);
    }
    return 0;
}
