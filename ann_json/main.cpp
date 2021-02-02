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
  class Individual
  {
  public:
    Individual() {}

  private:
    ANN ann_;

    friend void to_json(nlohmann::json& j, const Individual& ind)
    {
      for (auto first = ind.ann_.cbegin(); first != ind.ann_.cend(); ++first) {
        j.push_back(*first);
      }
    }

    friend void from_json(const nlohmann::json& j, Individual& ind)
    {
      const size_t n = j.size();
      if (n != ANN::state_size) throw std::runtime_error("ANN state size mismatch");
      auto first = ind.ann_.begin();
      for (size_t i = 0; i < j.size(); ++i, ++first) {
        *first = j.at(i);
      }
    }
  };


  template <typename ANN>
  void run(const nlohmann::json& j)
  {
    std::cout << "running simulation with: " << j["ann"] << std::endl;
    Individual<ANN> ind = j["individual"];    // throws if something is wrong
  }

}


int main()
{
  try {
    nlohmann::json j;
    auto is = std::ifstream("simple_ann.json");
    is >> j;
    // runtime to template
    std::string ann_type = j["ann"];
    if (ann_type == "SimpleAnn") sim::run<sim::SimpleAnn>(j);
    else if (ann_type == "ComplexAnn") sim::run<sim::ComplexAnn>(j);
    else throw std::runtime_error("unknown ANN");
    return 0;
  }
  catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
  }
  return 1;
}
