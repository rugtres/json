#include "grn.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <random>
#include <cassert>


template <typename RENG>
GRN make_grn(RENG& rng)
{
  GRN grn{};
  auto weight_distribution = std::uniform_real_distribution<double>(-1, 1);

  /// Input to hidden
  for (auto& node : grn.get_I2H()) {
    for (auto& weight : node) {
      weight = weight_distribution(rng);
    }
  }

  /// Hidden to hidden
  for (auto& node : grn.get_H2H()) {
    for (auto& weight : node) {
      weight = weight_distribution(rng);
    }
  }
  /// hidden to Output
  for (auto& node : grn.get_H2O()) {
    for (auto& weight : node) {
      weight = weight_distribution(rng);
    }
  }
  return grn;
}


void pretty_print(const GRN& grn)
{
  nlohmann::json j = grn;
  std::cout << j.dump(4) << std::endl;
  //std::cout << std::setw(4) << j << std::endl;
}


int main()
{
    std::minstd_rand0 rng{};
    auto grn_out = make_grn(rng);
    {
      nlohmann::json json_out;
      json_out = grn_out;
      std::ofstream os("grn.json");
      os << json_out;
    }
    // read back from json
    nlohmann::json json_in;
    std::ifstream is("grn.json");
    is >> json_in;
    GRN grn_in = json_in.get<GRN>();

    // shouldn't trigger
    assert(grn_out == grn_in);

    // population
    std::vector<GRN> pop;
    for (size_t i = 0; i < 10; ++i) {
      pop.emplace_back(make_grn(rng));
    }
    nlohmann::json json_pop;
    json_pop["population"] = pop;
    std::ofstream os("pop.json");
    os << json_pop;

    // pretty print
    std::cout << json_pop["population"][0].dump(4) << std::endl;
}
