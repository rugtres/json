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

std::ostream& operator<<(std::ostream& os, const GRN& p)
{
    save_I2H_weights(os,p);
    save_H2H_weights(os,p);
    save_H2O_weights(os,p);
    save_THidden(os,p);
    save_TOutput(os,p);
    save_ExHidden(os,p);
    save_n_input_nodes(os, p);
    save_n_output_nodes(os, p);

    return os;
}

std::ifstream& operator>>(std::ifstream& is, GRN& grn)
{
    auto ConI2H = load_I2H_weights( is);
    auto ConH2H = load_H2H_weights( is);
    auto ConH2O = load_H2O_weights( is);
    auto THidden = load_hid_tresh( is);
    auto TOutput = load_out_tresh( is);
    auto ExHidden = load_hid_state( is);
    int n_input = load_n_input_nodes( is);
    int n_output = load_n_output_nodes( is);

    grn = GRN{ConI2H,
            ConH2H,
            ConH2O,
            THidden,
            TOutput,
            ExHidden,
            n_input,
            n_output};

    return is;
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

GRN load_grn( const std::string& filename)
{
    GRN g;
    std::ifstream  i_f(filename);
    i_f >> g;
    return g;
}

std::vector<std::vector<double>> load_I2H_weights(std::ifstream& is)
{
    std::string dummy; // To remove the annotation in the file
    std::string line;
    std::string subline;
    std::vector<std::vector<double> > ConI2H;

    //Get everything until character that signals the end of the layer
    std::getline(is, line, '|');

    std::stringstream iss(line);
    while (std::getline(iss, subline ,'!').good())
    {
        std::stringstream isss(subline);
        std::vector<double> node_weights;
        double tmp;
        while(isss >> tmp)
        {
            node_weights.push_back(tmp);
            isss >> dummy;
        }
        ConI2H.push_back(node_weights);
        iss >> dummy; //Eat the " , " after the " ! "
    }
    is >> dummy; //Eat the " , " after the " | "

    return ConI2H;
}

std::vector<std::vector<double>> load_H2H_weights(std::ifstream& is)
{
    std::string dummy; // To remove the annotation in the file
    std::string line;
    std::string subline;
    std::vector<std::vector<double> > ConH2H;

    //Get everything until character that signals the end of the layer
    std::getline(is, line, '|');

    std::stringstream iss(line);
    while (std::getline(iss, subline ,'!').good())
    {
        std::stringstream isss(subline);
        std::vector<double> node_weights;
        double tmp;
        while(isss >> tmp)
        {
            node_weights.push_back(tmp);
            isss >> dummy;
        }
        ConH2H.push_back(node_weights);
        iss >> dummy; //Eat the " , " after the " ! "
    }
    is >> dummy; //Eat the " , " after the " | "

    return ConH2H;
}

std::vector<std::vector<double>> load_H2O_weights(std::ifstream& is)
{
    std::string dummy; // To remove the annotation in the file
    std::string line;
    std::string subline;
    std::vector<std::vector<double> > ConH2O;

    //Get everything until character that signals the end of the layer
    std::getline(is, line, '|');

    std::stringstream iss(line);
    while (std::getline(iss, subline ,'!').good())
    {
        std::stringstream isss(subline);
        std::vector<double> node_weights;
        double tmp;
        while(isss >> tmp)
        {
            node_weights.push_back(tmp);
            isss >> dummy;
        }
        ConH2O.push_back(node_weights);
        iss >> dummy; //Eat the " , " after the " ! "
    }
    is >> dummy; //Eat the " , " after the " | "

    return ConH2O;
}

std::vector<double> load_hid_tresh(std::ifstream& is)
{
    std::string dummy; // To remove the annotation in the file
    std::string line;
    std::vector<double> THidden;

    //Get everything until character that signals the end of the layer
    std::getline(is, line, '|');
    std::stringstream iss(line);
    double tmp;
    while(iss >> tmp)
    {
        THidden.push_back(tmp);
        iss >> dummy;
    }
    is >> dummy; //Eat the " , " after the " | "

    return THidden;
}

std::vector<bool> load_hid_state(std::ifstream& is)
{
    std::string dummy; // To remove the annotation in the file
    std::string line;
    std::vector<bool> ExHidden;

    //Get everything until character that signals the end of the layer
    std::getline(is, line, '|');
    std::stringstream iss(line);
    bool temporary;

    while(iss >> temporary)
    {
        ExHidden.push_back(temporary);
        iss >> dummy;
    }
    is >> dummy; //Eat the " , " after the " | "

    return ExHidden;
}


std::vector<double> load_out_tresh(std::ifstream& is)
{
    std::string dummy; // To remove the annotation in the file
    std::string line;
    std::vector<double> TOutput;

    //Get everything until character that signals the end of the layer
    std::getline(is, line, '|');
    std::stringstream iss(line);
    double tmp;
    while(iss >> tmp)
    {
        TOutput.push_back(tmp);
        iss >> dummy;
    }
    is >> dummy; //Eat the " , " after the " | "
    return TOutput;
}

int load_n_input_nodes(std::ifstream& is)
{
    std::string dummy; // To remove the annotation in the file
    std::string line;

    //Get everything until character that signals the end of the layer
    std::getline(is, line, '|');
    std::stringstream iss(line);
    int n_input_nodes;
    while(iss >> n_input_nodes)
    {
        iss >> dummy;
    }
    is >> dummy; //Eat the " , " after the " | "
    return n_input_nodes;
}

int load_n_output_nodes(std::ifstream& is)
{
    std::string dummy; // To remove the annotation in the file
    std::string line;

    //Get everything until character that signals the end of the layer
    std::getline(is, line, '|');
    std::stringstream iss(line);
    int n_output_nodes;
    while(iss >> n_output_nodes)
    {
        iss >> dummy;
    }
    is >> dummy; //Eat the " , " after the " | "
    return n_output_nodes;
}

void save_grn(const GRN& grn, const std::string& filename)
{
    std::ofstream f(filename);
    f << grn;
}

std::ostream& save_I2H_weights(std::ostream& os, const GRN& p)
{
    for(const auto& node : p.get_I2H())
    {
        for(const auto& weight : node)
        {
            os << weight << " , ";
        }
        os << " ! , ";
    }
    os << " | , ";
    return os;
}

std::ostream& save_H2H_weights(std::ostream& os, const GRN& p)
{
    for(const auto& node : p.get_H2H())
    {
        for(const auto& weight : node)
        {
            os << weight << " , ";
        }
        os << " ! , ";
    }
    os << " | , ";
    return os;
}

std::ostream& save_H2O_weights(std::ostream& os, const GRN& p)
{
    for(const auto& node : p.get_H2O())
    {
        for(const auto& weight : node)
        {
            os << weight << " , ";
        }
        os << " ! , ";
    }
    os << " | , ";
    return os;
}

std::ostream& save_THidden(std::ostream& os, const GRN& p)
{
    for(const auto& treshold : p.get_hid_tresh())
    {
        os << treshold << " , ";
    }
    os << " | , ";
    return os;
}

std::ostream& save_TOutput(std::ostream& os, const GRN& p)
{
    for(const auto& treshold : p.get_out_tresh())
    {
        os << treshold << " , ";
    }
    os << " | , ";
    return os;
}

std::ostream& save_ExHidden(std::ostream& os, const GRN& p)
{
    int state;
    for(const auto& boolstate : p.get_hidden_nodes())
    {
        state = boolstate;
        os << state << " , ";
    }
    os << " | , ";
    return os;
}

std::ostream& save_n_input_nodes(std::ostream& os, const GRN& grn)
{
    int n_inputs = grn.get_input_nodes().size();
    os << n_inputs << " , "
       << " | " << " , ";
    return  os;
}

std::ostream& save_n_output_nodes(std::ostream& os, const GRN& grn)
{
    int n_outputs = grn.get_output_nodes().size();
    os << n_outputs << " , "
       << " | " << " , ";
    return os;
}

void test_GRN()//!OCLINT , tests may be long
{
#ifndef NDEBUG
    //A grn is initialized with connection weights
    //all to a given value, default  = 1
    {
        double weight_value = 3.14;
        GRN g{3,3,2,weight_value};
        for(const auto& node : g.get_I2H())
        {
            for(const auto& weight : node)
                assert(weight - weight_value < 0.00001 && weight - weight_value > -0.00001);
        }
        for(const auto& node : g.get_H2H())
        {
            for(const auto& weight : node)
                assert(weight - weight_value < 0.00001 && weight - weight_value > -0.00001);
        }
        for(const auto& node : g.get_H2O())
        {
            for(const auto& weight : node)
                assert(weight - weight_value < 0.00001 && weight - weight_value > -0.00001);
        }
    }
    //A GRN is initialized with three layers
    // and a certain amount of nodes in each layer
    //These hyperparameters cannot be altered
    //during the simulation therefore no set() functions.
    //By default 3-3-2
    {
        size_t n_input = 5;
        size_t n_hidden = 5;
        size_t n_output = 5;
        GRN g(n_input,n_hidden,n_output);
        assert(g.get_I2H().size() == n_input);
        assert(g.get_H2H().size() == n_hidden);
        assert(g.get_H2O().size() == n_output);
    }

    //It is possible to get the values of the nodes states
    {
        GRN g;
        for(auto input : g.get_input_nodes())
            assert(input < 0.00001 && input > -0.00001);
        for(auto hidden : g.get_hidden_nodes())
            assert(hidden);
        for(auto output : g.get_output_nodes())
            assert(output);
    }

    //All weights in H2O connection can be set to a value
    {
        GRN g;
        double weight_value = 3.14;
        g.set_all_H2O(weight_value);
        for(const auto& node : g.get_H2O())
            for(const auto& weight : node)
                assert(weight - weight_value < 0.00001 &&
                       weight - weight_value > -0.00001);
    }

    //All weights in H2H connection can be set to a value
    {
        GRN g;
        double weight_value = 3.14;
        g.set_all_H2H(weight_value);
        for(const auto& node : g.get_H2H())
            for(const auto& weight : node)
                assert(weight - weight_value < 0.00001 &&
                       weight - weight_value > -0.00001);
    }

    //All output nodes can be set to true or false
    {
        GRN g;
        bool nodes_state = false;
        g.set_all_out_nodes(nodes_state);
        for(auto node : g.get_output_nodes())
            assert(node == nodes_state);
    }
    //All output nodes treshold can be set to a value
    {
        GRN g;
        double treshold_value = 3.14;
        g.set_all_out_tresh(treshold_value);
        for (const auto& treshold : g.get_out_tresh())
        {
            assert(treshold - treshold_value < 0.00001 &&
                   treshold - treshold_value > -0.00001);
        }
    }
    //All hidden nodes can be set to true or false
    {
        GRN g;
        bool nodes_state = false;
        g.set_all_hid_nodes(nodes_state);
        for(auto node : g.get_hidden_nodes())
            assert(node == nodes_state);
    }

    //A GRN can be printed and loaded from an ifstream and ofstream file
    {
        GRN g{1,2,1};
        g.set_all_H2O(0.12);
        g.set_all_I2H(0.123);
        g.set_all_H2H(0.124);
        const std::string filename = "grn.csv";
        save_grn(g, filename);
        GRN g1;
        g1 = load_grn(filename);
        assert( g == g1);
    }



#endif
}
