#include "utils/module/module_loader.hpp"


/*
 * Testing module pipelining using passive membrane equation
 * We use 3 modules that self register themself when loaded as dll
 */

int first_pipeline () {
   // contrary to get, create_new return an error if a key with this name already exist
    double t = 0;
    double delta_t = 0.025;
    double tau = 0.10;
    double Vrest = -70.0;;
    double V_int;
    double dV_pulse;
    storage_register ("time",        (void*) &t);
    storage_register ("tau",         (void*) &tau);
    storage_register ("Vrest",       (void*) &Vrest);
    storage_register ("Vintegrator", (void*) &V_int);
    storage_register ("dV_pulse",    (void*) &dV_pulse);

    Module fixed_pulse    = module_loader::from("fixed_pulse").createModule("-t time -o dV_pulse ");
    Module mbrane         = module_loader::from("passive_membrane").createModule("-t time -o Vintegrator -rest Vrest -tau tau -dv dV_pulse");
    Module validator      = module_loader::from("basic_validator").createModule("-t time -v Vintegrator");
    for (t = 0; t < t_end; t+=delta_t) {
        module::execute (fixed_pulse);
        module::execute (passive_membrane);
        module::execute (validator);
    }
}



/*
int create_new () {
   // contrary to get, create_new return an error if a key with this name already exist
    double* t        = storage_new<double>("time",  1);
    double* tau      = storage_new<double>("tau",   1);
    double* Vrest    = storage_new<double>("Vrest", 1);
    double* V_int    = storage_new<double>("Vintegrator", 1);
    double* dV_pulse = storage_new<double>("dV_pulse", 1);
    double  delta_t = 0.025;
    Module fixed_pulse    = module_loader::from("fixed_pulse").createModule("-t time -o dV_pulse ");
    Module mbrane         = module_loader::from("passive_membrane").createModule("-t time -o Vintegrator -rest Vrest -tau tau -dv dV_pulse");
    Module validator      = module_loader::from("basic_validator").createModule("-t time -v Vintegrator");
    *tau   = 0.10;
    *Vrest = -70;
    for (*t = 0; *t < t_end; *t+=delta_t) {
        module::execute (fixed_pulse);
        module::execute (passive_membrane);
        module::execute (validator); // std cout : time : Vintegrator
    }
}
*/
