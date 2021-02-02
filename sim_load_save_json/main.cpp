#include <cassert>
#include <iostream>
#include <fstream>
#include "parameters.h"


int main()
{
  // write default parameter set to json
  sim_param sim_par_out;
  {
    nlohmann::json json_out;
    json_out = sim_par_out;
    std::ofstream os("test.json");
    os << json_out;
  }

  // read back from json
  nlohmann::json json_in;
  std::ifstream is("test.json");
  is >> json_in;
  sim_param sim_par_in = json_in.get<sim_param>();

  // shouldn't trigger
  assert(sim_par_out == sim_par_in);
  return 0;
}
