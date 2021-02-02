#include "grn.h"
#include <algorithm>
#include <cassert>
#include <numeric>



GRN::GRN(size_t n_input, size_t n_hidden, size_t n_output, double weights):
    m_ConI2H(n_input,std::vector<double>(n_hidden, weights)),
    m_ConH2H(n_hidden,std::vector<double>(n_hidden, weights)),
    m_ConH2O(n_hidden,std::vector<double>(n_output, weights)),
    m_THidden(n_hidden,0),
    m_TOutput(n_output,0),
    m_ExInput(n_input,0),
    m_ExHidden(n_hidden,true),
    m_ExOutput(n_output,true)
{

}

GRN::GRN(std::vector<std::vector<double> > ConI2H,
         std::vector<std::vector<double> > ConH2H,
         std::vector<std::vector<double> > ConH2O,
         std::vector<double> THidden,
         std::vector<double> TOutput,
         std::vector<bool>ExHidden,
         int n_input,
         int n_output):
    m_ConI2H{ ConI2H},
    m_ConH2H{ ConH2H},
    m_ConH2O{ ConH2O},
    m_THidden{ THidden},
    m_TOutput{ TOutput},
    m_ExInput(n_input, 0),
    m_ExHidden{ExHidden},
    m_ExOutput(n_output, 0)
{

}


void to_json(nlohmann::json& j, const GRN& t)
{
  j["ConI2H"] = t.m_ConI2H;
  j["ConH2H"] = t.m_ConH2H;
  j["ConH2O"] = t.m_ConH2O;
  j["THidden"] = t.m_THidden;
  j["ExInput"] = t.m_ExInput;
  j["ExHidden"] = t.m_ExHidden;
  j["ExOutput"] = t.m_ExOutput;
}


void from_json(const nlohmann::json& j, GRN& t)
{
  j["ConI2H"].get_to(t.m_ConI2H);
  j["ConH2H"].get_to(t.m_ConH2H);
  j["ConH2O"].get_to(t.m_ConH2O);
  j["THidden"].get_to(t.m_THidden);
  j["ExInput"].get_to(t.m_ExInput);
  j["ExHidden"].get_to(t.m_ExHidden);
  j["ExOutput"].get_to(t.m_ExOutput);
}


bool operator==(const GRN& lhs, const GRN& rhs)
{

    bool h2h = compare_weights_with_tolerance(lhs.get_H2H() , rhs.get_H2H());
    bool h2o = compare_weights_with_tolerance(lhs.get_H2O(), rhs.get_H2O());
    bool i2h = compare_weights_with_tolerance(lhs.get_I2H(), rhs.get_I2H());
    bool hid_t = lhs.get_hid_tresh() == rhs.get_hid_tresh();
    bool out_t = lhs.get_out_tresh() == rhs.get_out_tresh();
    bool hid_n = lhs.get_hidden_nodes() == rhs.get_hidden_nodes();
    //We do not look at input and output nodes since they might vary
    //depending on the status of the simulation
    //this might be true also for hidden nodes,
    //but not before the first time step of a cycle
    return h2h && h2o && i2h && hid_n && hid_t && out_t;
}

bool operator!=(const GRN& lhs, const GRN& rhs)
{
    return !(lhs == rhs);
}


void GRN::set_hid_node(int index_node, bool state)
{m_ExHidden[static_cast<unsigned int>(index_node)] = state;}

void GRN::set_all_H2H(double value) noexcept
{
    for (auto& node : get_H2H())
    {
        for(auto& connection_weight : node)
            connection_weight = value;
    }
}

void GRN::set_all_H2O(double value) noexcept
{
    for (auto& node : get_H2O())
    {
        for(auto& connection_weight : node)
            connection_weight = value;
    }
}

void GRN::set_all_I2H(double value) noexcept
{
    for (auto& node : get_I2H())
    {
        for(auto& connection_weight : node)
            connection_weight = value;
    }
}

void GRN::set_all_out_nodes(bool state)
{
    for(int i = 0; i != static_cast<int>(get_output_nodes().size()); i++)
        set_out_node(i, state);
}


void GRN::set_all_hid_nodes(bool state)
{
    for(int i = 0; i != static_cast<int>(get_hidden_nodes().size()); i++)
        set_hid_node(i,state);
}

void GRN::set_all_hid_tresh(double value)
{
    for(auto& treshold : get_hid_tresh())
        treshold = value;
}

void GRN::set_all_out_tresh(double value)
{
    for(auto& treshold : get_out_tresh())
        treshold = value;
}

void GRN::set_inputs(std::vector<double> inputs)
{
    assert(inputs.size() == get_input_nodes().size());
    get_input_nodes().swap(inputs);
}

void GRN::set_out_node(int index_node, bool state)
{m_ExOutput[static_cast<unsigned int>(index_node)] = state;}

bool compare_with_tolerance(const std::vector<double>& lhs,const std::vector<double>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(),
                      [](const double& lhs_w, const double& rhs_w)
    {return lhs_w - rhs_w < 0.0001 && lhs_w - rhs_w > -0.00001;});
}

bool compare_weights_with_tolerance(const std::vector<std::vector<double>>& lhs,
                                    const std::vector<std::vector<double>>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), compare_with_tolerance);
}


