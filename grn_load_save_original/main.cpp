#include "grn.h"
#include <random>
#include <cassert>
int main()
{
    std::minstd_rand0 rng{};
    GRN grn{};

    ///Set network weights to something non trivial

    auto weight_distribution = std::uniform_real_distribution<double>(-1,1);

    /// Input to hidden
    for(auto& node : grn.get_I2H())
        for(auto& weight : node)
        {
            weight = weight_distribution(rng);
        }
    /// Hidden to hidden
    for(auto& node : grn.get_H2H())
        for(auto& weight : node)
        {
            weight = weight_distribution(rng);
        }
    /// hidden to Output
    for(auto& node : grn.get_H2O())
        for(auto& weight : node)
        {
            weight = weight_distribution(rng);
        }

    std::string name{"test.csv"};
    save_grn(grn,name);
    auto copy_grn = load_grn(name);
    assert(grn == copy_grn);

}
