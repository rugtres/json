#include <stdexcept>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "ann3.hpp"

namespace sim {

  using namespace ann3;

  using SimpleAnn = Network<float,
    Layer<Neuron<2, activation::sgn::unipolar>, 1>
  >;


  using ComplexAnn = Network<float,
    Layer<Neuron<2, activation::sgn::unipolar>, 8>,
    Layer<Neuron<8, activation::sgn::unipolar>, 18>,
    Layer<Neuron<18, activation::rtlu>, 8>,
    Layer<Neuron<8, activation::sgn::unipolar>, 1>
  >;


  template <typename ANN>
  class Simulation
  {
  public:
    Simulation() {}

  private:
    ANN init_ann_;

    friend void to_json(nlohmann::json& j, const Simulation& sim)
    {
      for (auto first = sim.init_ann_.cbegin(); first != sim.init_ann_.cend(); ++first) {
        j.push_back(*first);
      }
    }

    friend void from_json(const nlohmann::json& j, Simulation& sim)
    {
      const size_t n = j.size();
      if (n != ANN::state_size) throw std::runtime_error("ANN state size mismatch");
      auto first = sim.init_ann_.begin();
      for (size_t i = 0; i < j.size(); ++i, ++first) {
        *first = j.at(i);
      }
    }
  };


  template <typename ANN>
  void run(const nlohmann::json& j)
  {
    std::cout << "running simulation with: " << j.at("ann_type") << std::endl;
    Simulation<ANN> sim = j.at("init_ann");
  }

}


int main()
{
  try {
    nlohmann::json j;
    auto is = std::ifstream("simple_ann.json");
    is >> j;
    // runtime to template dispatch
    auto js = j.at("simulation");
    std::string ann_type = js.at("ann_type");
    if (ann_type == "SimpleAnn") sim::run<sim::SimpleAnn>(js);
    else if (ann_type == "ComplexAnn") sim::run<sim::ComplexAnn>(js);
    else throw std::runtime_error("unknown ANN");
    return 0;
  }
  catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
  }
  return 1;
}
