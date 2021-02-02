#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <nlohmann/json.hpp>


struct env_param
{
  int grid_side = 1;
  double diff_coeff = 0.1;
  double init_food = 10.0;
  double metab_degrad_rate = 0.1;
  double mean_diff_coeff = 0.1;
  double mean_degr_rate = 0.1;
  double var_diff_coeff = 0.02;
  double var_degr_coeff = 0.0;

  friend bool operator==(const env_param& a, const env_param& b) = default;
};


struct ind_param
{
  double radius = 0.8;
  double treshold_energy = 10;
  double uptake_rate = 0.1;
  double uptake_rate_mean = 0.1;
  double uptake_rate_var = 0.01;
  double metabolic_rate = 0.01;
  double reproduction_prob = 0.5;
  double reproduction_prob_mean = 0.5;
  double reproduction_prob_var = 0.01;
  double spor_metabolic_rate = 0.5;
  double spor_metabolic_rate_mean = 0.5;
  double spor_metabolic_rate_var = 0.01;
  int transformation_time = 5;
  int transformation_time_mean = 5;
  int transformation_range = 1;
  double metab_secretion_rate = 1;

  friend bool operator==(const ind_param& a, const ind_param& b) = default;
};


struct meta_param
{
  int n_cycles = 1;
  int cycle_duration = 200;
  int seed = 1;
  int change_frequency = 0;   // no default ??
  int pop_max = 1000000;
  int collision_check_interval = 0;

  friend bool operator==(const meta_param& a, const meta_param& b) = default;
};


struct pop_param
{
  unsigned int start_pop_size = 1;
  unsigned int exp_new_pop_size = 1;
  double min_dist = 0.1;
  double mutation_prob = 0.0015;
  double mutation_step = 0.1;
  double base_disp_prob = 0.01;
  double spore_advantage = 10.0;
  double death_rate = 0.0;

  friend bool operator==(const pop_param& a, const pop_param& b) = default;
};


class sim_param
{
public:
    sim_param() {};
    sim_param(env_param e, ind_param i, meta_param m, pop_param p) :
      m_env_param(e), m_ind_param(i), m_meta_param(m), m_pop_param(p)
    {}

    ///Gets const reference to population parameter
    const env_param& get_env_param() const noexcept {return m_env_param;}

    ///Gets const reference to metaparameters
    const meta_param& get_meta_param() const noexcept {return  m_meta_param;}

    ///Gets const reference to population parameter
    const pop_param& get_pop_param() const noexcept {return m_pop_param;}

    ///Gets const reference to ind parameter
    const ind_param& get_ind_param() const noexcept {return m_ind_param;}

    friend bool operator==(const sim_param& a, const sim_param& b) = default;

private:
    ///Parameters for the environment
    env_param m_env_param;

    ///Parameters pertaining individuals
    ind_param m_ind_param;

    ///Parameters concerning the simulation, such as duration, number of replicates etc
    meta_param m_meta_param;

    ///Parameters concerning the population of individuals
    pop_param m_pop_param;
};


// declare to_json / from_json pair for sim_param
void to_json(nlohmann::json& j, const sim_param& p);
void from_json(const nlohmann::json& j, sim_param& p);


#endif // SIM_PARAMETERS_H
