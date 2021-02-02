#include "parameters.h"


// define to_json / from_json pair for pop_param
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(pop_param,
  start_pop_size,
  exp_new_pop_size,
  min_dist,
  mutation_prob,
  mutation_step,
  base_disp_prob,
  spore_advantage,
  death_rate
)


// define to_json / from_json pair for meta_param
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(env_param,
  grid_side,
  diff_coeff,
  init_food,
  metab_degrad_rate,
  mean_diff_coeff,
  mean_degr_rate,
  var_diff_coeff,
  var_degr_coeff
)


// define to_json / from_json pair for meta_param
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(meta_param,
  n_cycles,
  cycle_duration,
  seed,
  change_frequency,
  pop_max,
  collision_check_interval
)


// define to_json / from_json pair for ind_param
// we can't use the fancy NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE macro
// because we have too many members :(
// However NLOHMANN_JSON_TO/FROM is nearly as convenient
void to_json(nlohmann::json& j, const ind_param& t) 
{
  NLOHMANN_JSON_TO(radius);
  NLOHMANN_JSON_TO(treshold_energy);
  NLOHMANN_JSON_TO(uptake_rate);
  NLOHMANN_JSON_TO(uptake_rate_mean);
  NLOHMANN_JSON_TO(uptake_rate_var);
  NLOHMANN_JSON_TO(metabolic_rate);
  NLOHMANN_JSON_TO(reproduction_prob);
  NLOHMANN_JSON_TO(reproduction_prob_mean);
  NLOHMANN_JSON_TO(reproduction_prob_var);
  NLOHMANN_JSON_TO(spor_metabolic_rate);
  NLOHMANN_JSON_TO(spor_metabolic_rate_mean);
  NLOHMANN_JSON_TO(spor_metabolic_rate_var);
  NLOHMANN_JSON_TO(transformation_time);
  NLOHMANN_JSON_TO(transformation_time_mean);
  NLOHMANN_JSON_TO(transformation_range);
  NLOHMANN_JSON_TO(metab_secretion_rate);
}


void from_json(const nlohmann::json& j, ind_param& t) 
{
  NLOHMANN_JSON_FROM(radius);
  NLOHMANN_JSON_FROM(treshold_energy);
  NLOHMANN_JSON_FROM(uptake_rate);
  NLOHMANN_JSON_FROM(uptake_rate_mean);
  NLOHMANN_JSON_FROM(uptake_rate_var);
  NLOHMANN_JSON_FROM(metabolic_rate);
  NLOHMANN_JSON_FROM(reproduction_prob);
  NLOHMANN_JSON_FROM(reproduction_prob_mean);
  NLOHMANN_JSON_FROM(reproduction_prob_var);
  NLOHMANN_JSON_FROM(spor_metabolic_rate);
  NLOHMANN_JSON_FROM(spor_metabolic_rate_mean);
  NLOHMANN_JSON_FROM(spor_metabolic_rate_var);
  NLOHMANN_JSON_FROM(transformation_time);
  NLOHMANN_JSON_FROM(transformation_time_mean);
  NLOHMANN_JSON_FROM(transformation_range);
  NLOHMANN_JSON_FROM(metab_secretion_rate);
}


// define to_json / from_json pair for sim_param
// members are private here - we use accessors
void to_json(nlohmann::json& j, const sim_param& t) 
{
  j["sim_param"] = nlohmann::json{
    {"env_param", t.get_env_param()},
    {"ind_param", t.get_ind_param()},
    {"meta_param", t.get_meta_param()},
    {"pop_param", t.get_pop_param()}
  };
}


// at least we have an public constructor...
void from_json(const nlohmann::json& j, sim_param& t) 
{
  auto jsp = j["sim_param"];
  auto ep = jsp.at("env_param").get<env_param>();
  auto ip = jsp.at("ind_param").get<ind_param>();
  auto mp = jsp.at("meta_param").get<meta_param>();
  auto pp = jsp.at("pop_param").get<pop_param>();
  t = sim_param(ep, ip, mp, pp);
}

