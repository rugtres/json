#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>


struct Parameter
{
  double a, b;
};


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Parameter, a, b)


double run(const Parameter& param)
{
  return param.a + param.b;
}


int main()
{
  try {
    std::ifstream is("params.json");
    nlohmann::json jp{};
    is >> jp;

    nlohmann::json jres = jp;
    for (size_t i = 0; i < jp.size(); ++i) {
      double result = run(jp.at(i));
      jres.at(i)["result"] = result;
    }
    std::ofstream os("results.json");
    os << jres;
    return 0;
  }
  catch (const std::exception& err) {
    std::cout << err.what() << std::endl;
  }
  return -1;
}
