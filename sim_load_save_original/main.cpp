#include "sim_parameters.h"
#include <cassert>
int main()
{

    env_param e;
    ind_param i;
    pop_param p;
    meta_param m;

    sim_param sim_par{e,i,m,p};

    auto name = "test.csv";
    save_sim_parameters(sim_par,name);

    auto copy_sim_par = load_sim_parameters(name);

    assert(copy_sim_par == sim_par);
}
