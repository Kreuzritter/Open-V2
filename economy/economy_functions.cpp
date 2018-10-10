#include "common\\common.h"
#include "economy_functions.h"
#include "world_state\\world_state.h"
#include "provinces\\province_functions.hpp"
#include "nations\\nations_functions.hpp"
#include "population\\population_function.h"
#include "nations\\nations_functions.hpp"
#include "provinces\\province_functions.hpp"
#include <random>

namespace economy {
	void init_economic_scenario(world_state& ws) {
		//ws.w.economy_s.current_prices.resize(ws.s.economy_m.aligned_32_goods_count);
		//for(auto& g : ws.s.economy_m.goods) {
		//	ws.w.economy_s.current_prices[g.id] = g.base_price;
		//}
		//if(ws.s.economy_m.aligned_32_goods_count != 0)
		//	ws.w.economy_s.current_prices[economy::money_good] = economy::money_qnty_type(1);
	}

	void reset_state(economic_state& ) {
	}

	production_modifiers rgo_production_modifiers(
		world_state const& ws,
		worked_instance const& instance,
		workers_information const& workers_info,
		nations::nation const& in_nation,
		provinces::province_state const& in_province,
		int32_t rgo_base_size,
		float production_scale,
		economy::goods_tag production,
		float mobilization_effect) {

		production_modifiers result;

		result.input_modifier = 1.0f;
		result.output_modifier = 1.0f;
		result.throughput_modifier = 0.0f;

		bool is_mined = (ws.s.economy_m.goods[production].flags & economy::good_definition::mined) != 0;

		float total_workforce = float(workers_info.workforce * rgo_base_size) * production_scale *
			((is_mined ? 
				in_province.modifier_values[modifiers::provincial_offsets::mine_rgo_size] + in_nation.modifier_values[modifiers::national_offsets::mine_rgo_size] :
				in_province.modifier_values[modifiers::provincial_offsets::farm_rgo_size] + in_nation.modifier_values[modifiers::national_offsets::farm_rgo_size]) +
				ws.w.nation_s.production_adjustments.get(in_nation.id, technologies::economy_tag_to_production_adjustment<technologies::production_adjustment::rgo_size>(production)) +
				1.0f);

		if(total_workforce == 0.0f)
			return result;

		for(uint32_t i = 0; i < max_worker_types; ++i) {
			if(workers_info.workers[i].contribution == contribution_type::output) {
				result.output_modifier += workers_info.workers[i].effect_multiplier * float(instance.worker_populations[i]) / total_workforce;
			} else if(workers_info.workers[i].contribution == contribution_type::throughput) {
				result.throughput_modifier += workers_info.workers[i].effect_multiplier * float(instance.worker_populations[i]) / total_workforce;
			}
		}

		float total_pop = float(ws.w.nation_s.state_demographics.get(in_province.state_instance->id, population::total_population_tag));
		float owner_effect = total_pop != 0.0f ? workers_info.owner.effect_multiplier * (ws.w.nation_s.state_demographics.get(in_province.state_instance->id, population::to_demo_tag(ws, workers_info.owner.type))) / total_pop : 0.0f;

		result.output_modifier *=  std::max(0.0f, (
			in_province.modifier_values[modifiers::provincial_offsets::local_rgo_output] +
			in_nation.modifier_values[modifiers::national_offsets::rgo_output] +
			(is_mined ? in_province.modifier_values[modifiers::provincial_offsets::farm_rgo_eff] : in_province.modifier_values[modifiers::provincial_offsets::mine_rgo_eff]) +
			(workers_info.owner.contribution == contribution_type::output ? owner_effect : 0.0f) +
			ws.w.nation_s.production_adjustments.get(in_nation.id, technologies::economy_tag_to_production_adjustment<technologies::production_adjustment::rgo_goods_output>(production)) +
			1.0f));
		result.throughput_modifier *= mobilization_effect * std::max(0.0f, (
			in_province.modifier_values[modifiers::provincial_offsets::local_rgo_throughput] +
			in_nation.modifier_values[modifiers::national_offsets::rgo_throughput] +
			(workers_info.owner.contribution == contribution_type::throughput ? owner_effect : 0.0f) +
			ws.w.nation_s.production_adjustments.get(in_nation.id, technologies::economy_tag_to_production_adjustment<technologies::production_adjustment::rgo_goods_throughput>(production)) +
			1.0f));

		return result;
	}

	production_modifiers factory_production_modifiers(
		world_state const& ws,
		worked_instance const& instance,
		bonus* bonuses,
		workers_information const& workers_info,
		nations::nation const& in_nation,
		provinces::province_state const& in_province,
		int32_t base_size,
		float production_scale,
		economy::goods_tag production,
		float mobilization_effect) {

		production_modifiers result;

		result.input_modifier = 1.0f;
		result.output_modifier = 1.0f;
		result.throughput_modifier = 0.0f;

		float total_workforce = float(workers_info.workforce * base_size) * production_scale;

		for(uint32_t i = 0; i < max_worker_types; ++i) {
			if(workers_info.workers[i].contribution == contribution_type::input) {
				result.input_modifier += workers_info.workers[i].effect_multiplier * float(instance.worker_populations[i]) / total_workforce;
			} else if(workers_info.workers[i].contribution == contribution_type::output) {
				result.output_modifier += workers_info.workers[i].effect_multiplier * float(instance.worker_populations[i]) / total_workforce;
			} else if(workers_info.workers[i].contribution == contribution_type::throughput) {
				result.throughput_modifier += workers_info.workers[i].effect_multiplier * float(instance.worker_populations[i]) / total_workforce;
			}
		}

		float total_pop = float(ws.w.nation_s.state_demographics.get(in_province.state_instance->id, population::total_population_tag));
		float owner_effect = total_pop != 0.0f ? workers_info.owner.effect_multiplier * (ws.w.nation_s.state_demographics.get(in_province.state_instance->id, population::to_demo_tag(ws, workers_info.owner.type))) / total_pop : 0.0f;

		result.input_modifier *= std::max(0.0f, (
			in_province.modifier_values[modifiers::provincial_offsets::local_factory_input] +
			in_nation.modifier_values[modifiers::national_offsets::factory_input] +
			(workers_info.owner.contribution == contribution_type::input ? owner_effect : 0.0f) +
			ws.w.nation_s.production_adjustments.get(in_nation.id, technologies::economy_tag_to_production_adjustment<technologies::production_adjustment::factory_goods_input>(production)) +
			1.0f));
		result.output_modifier *= std::max(0.0f, (
			in_province.modifier_values[modifiers::provincial_offsets::local_factory_output] +
			in_nation.modifier_values[modifiers::national_offsets::factory_output] +
			(workers_info.owner.contribution == contribution_type::output ? owner_effect : 0.0f) +
			ws.w.nation_s.production_adjustments.get(in_nation.id, technologies::economy_tag_to_production_adjustment<technologies::production_adjustment::factory_goods_output>(production)) +
			1.0f));
		result.throughput_modifier *= mobilization_effect * std::max(0.0f, (
			in_province.modifier_values[modifiers::provincial_offsets::local_factory_throughput] +
			in_nation.modifier_values[modifiers::national_offsets::factory_throughput] +
			(workers_info.owner.contribution == contribution_type::throughput ? owner_effect : 0.0f) +
			ws.w.nation_s.production_adjustments.get(in_nation.id, technologies::economy_tag_to_production_adjustment<technologies::production_adjustment::factory_goods_throughput>(production)) +
			1.0f));

		float bonus_sum = 1.0f;
		for(uint32_t i = 0; i < std::extent_v<decltype(std::declval<factory_type>().bonuses)>; ++i) {
			if(is_valid_index(bonuses[i].condition) && triggers::test_trigger(ws.s.trigger_m.trigger_data.data() + to_index(bonuses[i].condition), ws, in_province.state_instance, in_province.state_instance, nullptr, nullptr)) {
				bonus_sum += bonuses[i].value;
			}
		}

		result.throughput_modifier *= bonus_sum;

		return result;
	}


	production_modifiers artisan_production_modifiers(
		world_state const& ws,
		nations::nation const& in_nation,
		provinces::province_state const& in_province,
		economy::goods_tag production,
		float mobilization_effect) {

		production_modifiers result;

		result.input_modifier = 1.0f;
		result.output_modifier = 1.0f;
		result.throughput_modifier = 1.0f;

		
		result.input_modifier *= std::max(0.0f, (
			in_province.modifier_values[modifiers::provincial_offsets::local_artisan_input] +
			in_nation.modifier_values[modifiers::national_offsets::artisan_input] +
			ws.w.nation_s.production_adjustments.get(in_nation.id, technologies::economy_tag_to_production_adjustment<technologies::production_adjustment::artisan_goods_input>(production)) +
			1.0f));
		result.output_modifier *= std::max(0.0f, (
			in_province.modifier_values[modifiers::provincial_offsets::local_artisan_output] +
			in_nation.modifier_values[modifiers::national_offsets::artisan_output] +
			ws.w.nation_s.production_adjustments.get(in_nation.id, technologies::economy_tag_to_production_adjustment<technologies::production_adjustment::artisan_goods_output>(production)) +
			1.0f));
		result.throughput_modifier *= mobilization_effect * std::max(0.0f, (
			in_province.modifier_values[modifiers::provincial_offsets::local_artisan_throughput] +
			in_nation.modifier_values[modifiers::national_offsets::artisan_throughput] +
			ws.w.nation_s.production_adjustments.get(in_nation.id, technologies::economy_tag_to_production_adjustment<technologies::production_adjustment::artisan_goods_throughput>(production)) +
			1.0f));

		return result;
	}

	int32_t rgo_max_employment(world_state const& ws, provinces::province_state& ps) {
		auto production = ps.rgo_production;
		if(!is_valid_index(production)) return 1;

		auto owner = ps.owner;
		if(!owner) return 0;

		auto owner_id = owner->id;
		if(!ws.w.nation_s.nations.is_valid_index(owner_id)) return 0;

		bool is_mined = (ws.s.economy_m.goods[production].flags & economy::good_definition::mined) != 0;
		float total_workforce = float((is_mined ? ws.s.economy_m.rgo_mine.workforce : ws.s.economy_m.rgo_farm.workforce) * int32_t(ps.rgo_size)) *
			((is_mined ?
				ps.modifier_values[modifiers::provincial_offsets::mine_rgo_size] + owner->modifier_values[modifiers::national_offsets::mine_rgo_size] :
				ps.modifier_values[modifiers::provincial_offsets::farm_rgo_size] + owner->modifier_values[modifiers::national_offsets::farm_rgo_size]) +
				ws.w.nation_s.production_adjustments.get(owner_id, technologies::economy_tag_to_production_adjustment<technologies::production_adjustment::rgo_size>(production)) +
				1.0f);
		return std::max(0, int32_t(total_workforce));
	}

	void match_rgo_worker_type(world_state& ws, provinces::province_state& ps) {
		bool is_mined = (ws.s.economy_m.goods[ps.rgo_production].flags & economy::good_definition::mined) != 0;
		
		if(is_mined) {
			provinces::for_each_pop(ws, ps, [&ws](population::pop& p) {
				for(uint32_t i = 0; i < std::extent_v<decltype(ws.s.economy_m.rgo_farm.workers)>; ++i) {
					if(p.type == ws.s.economy_m.rgo_farm.workers[i].type && is_valid_index(ws.s.economy_m.rgo_mine.workers[i].type) && p.type != ws.s.economy_m.rgo_mine.workers[i].type)
						population::change_pop_type(ws, p, ws.s.economy_m.rgo_mine.workers[i].type);
				}
			});
		} else {
			provinces::for_each_pop(ws, ps, [&ws](population::pop& p) {
				for(uint32_t i = 0; i < std::extent_v<decltype(ws.s.economy_m.rgo_farm.workers)>; ++i) {
					if(p.type == ws.s.economy_m.rgo_mine.workers[i].type && is_valid_index(ws.s.economy_m.rgo_farm.workers[i].type) && p.type != ws.s.economy_m.rgo_farm.workers[i].type)
						population::change_pop_type(ws, p, ws.s.economy_m.rgo_farm.workers[i].type);
				}
			});
		}
	}

	void update_rgo_employment(world_state& ws, provinces::province_state& ps) {
		bool is_mined = (ws.s.economy_m.goods[ps.rgo_production].flags & economy::good_definition::mined) != 0;

		if(!ps.owner) {
			for(uint32_t i = 0; i < std::extent_v<decltype(ps.rgo_worker_data.worker_populations)>; ++i)
				ps.rgo_worker_data.worker_populations[i] = 0;

			if(is_mined) {
				provinces::for_each_pop(ws, ps, [&ws](population::pop& p) {
					for(uint32_t i = 0; i < std::extent_v<decltype(ws.s.economy_m.rgo_mine.workers)>; ++i) {
						if(p.type == ws.s.economy_m.rgo_mine.workers[i].type)
							ws.w.population_s.pop_demographics.get(p.id, population::total_employment_tag) = 0;
					}
				});
			} else {
				provinces::for_each_pop(ws, ps, [&ws](population::pop& p) {
					for(uint32_t i = 0; i < std::extent_v<decltype(ws.s.economy_m.rgo_farm.workers)>; ++i) {
						if(p.type == ws.s.economy_m.rgo_farm.workers[i].type)
							ws.w.population_s.pop_demographics.get(p.id, population::total_employment_tag) = 0;
					}
				});
			}
			return;
		}

		float total_workforce = float((is_mined ? ws.s.economy_m.rgo_mine.workforce : ws.s.economy_m.rgo_farm.workforce) * int32_t(ps.rgo_size)) *
			((is_mined ?
				ps.modifier_values[modifiers::provincial_offsets::mine_rgo_size] + ps.owner->modifier_values[modifiers::national_offsets::mine_rgo_size] :
				ps.modifier_values[modifiers::provincial_offsets::farm_rgo_size] + ps.owner->modifier_values[modifiers::national_offsets::farm_rgo_size]) +
				ws.w.nation_s.production_adjustments.get(ps.owner->id, technologies::economy_tag_to_production_adjustment<technologies::production_adjustment::rgo_size>(ps.rgo_production)) +
				1.0f);

		if(is_mined) {
			boost::container::small_vector<float, 8> percentage_by_type;
			for(uint32_t i = 0; i < std::extent_v<decltype(ws.s.economy_m.rgo_mine.workers)>; ++i) {
				auto pop_of_type = ws.w.province_s.province_demographics.get(ps.id, population::to_demo_tag(ws, ws.s.economy_m.rgo_mine.workers[i].type));
				if(pop_of_type != 0)
					percentage_by_type.push_back(std::min(1.0f, (total_workforce * ws.s.economy_m.rgo_mine.workers[i].amount) / float(pop_of_type)));
				else
					percentage_by_type.push_back(0.0f);
				ps.rgo_worker_data.worker_populations[i] = std::min(pop_of_type, int32_t(total_workforce * ws.s.economy_m.rgo_mine.workers[i].amount));
			}

			provinces::for_each_pop(ws, ps, [&ws, &percentage_by_type](population::pop& p) {
				for(uint32_t i = 0; i < std::extent_v<decltype(ws.s.economy_m.rgo_mine.workers)>; ++i) {
					if(p.type == ws.s.economy_m.rgo_mine.workers[i].type)
						ws.w.population_s.pop_demographics.get(p.id, population::total_employment_tag) = int32_t(percentage_by_type[i] * ws.w.population_s.pop_demographics.get(p.id, population::total_population_tag));
				}
			});
		} else {
			boost::container::small_vector<float, 8> percentage_by_type;
			for(uint32_t i = 0; i < std::extent_v<decltype(ws.s.economy_m.rgo_farm.workers)>; ++i) {
				auto pop_of_type = ws.w.province_s.province_demographics.get(ps.id, population::to_demo_tag(ws, ws.s.economy_m.rgo_farm.workers[i].type));
				if(pop_of_type != 0)
					percentage_by_type.push_back(std::min(1.0f, (total_workforce * ws.s.economy_m.rgo_farm.workers[i].amount) / float(pop_of_type)));
				else
					percentage_by_type.push_back(0.0f);
				ps.rgo_worker_data.worker_populations[i] = std::min(pop_of_type, int32_t(total_workforce * ws.s.economy_m.rgo_farm.workers[i].amount));
			}

			provinces::for_each_pop(ws, ps, [&ws, &percentage_by_type](population::pop& p) {
				for(uint32_t i = 0; i < std::extent_v<decltype(ws.s.economy_m.rgo_farm.workers)>; ++i) {
					if(p.type == ws.s.economy_m.rgo_farm.workers[i].type)
						ws.w.population_s.pop_demographics.get(p.id, population::total_employment_tag) = int32_t(percentage_by_type[i] * ws.w.population_s.pop_demographics.get(p.id, population::total_population_tag));
				}
			});
		}
	}

	void update_factories_employment(world_state& ws, nations::state_instance& si) {
		int32_t* allocation_by_type = (int32_t*)_alloca(sizeof(int32_t) * std::extent_v<decltype(si.factories)> * ws.s.population_m.count_poptypes);
		std::fill_n(allocation_by_type, std::extent_v<decltype(si.factories)> * ws.s.population_m.count_poptypes, 0);

		int32_t* state_population_by_type = ws.w.nation_s.state_demographics.get_row(si.id) + to_index(population::to_demo_tag(ws, population::pop_type_tag(0)));
		int32_t* unallocated_workers = (int32_t*)_alloca(sizeof(int32_t) * ws.s.population_m.count_poptypes);

		std::copy(state_population_by_type, state_population_by_type + ws.s.population_m.count_poptypes, unallocated_workers);
		std::pair<uint32_t, float> factories_by_profitability[std::extent_v<decltype(si.factories)>];

		for(uint32_t i = 0; i < std::extent_v<decltype(si.factories)>; ++i)
			factories_by_profitability[i] = std::pair<uint32_t, float>(i, si.factories[i].type ? si.factories[i].factory_operational_scale : 0.0f);
		
		std::sort(std::begin(factories_by_profitability), std::end(factories_by_profitability), [](std::pair<uint32_t, float> const& a, std::pair<uint32_t, float> const& b) { return a.second > b.second; });

		for(uint32_t i = 0; i < std::extent_v<decltype(si.factories)>; ++i) {
			auto factory_index = factories_by_profitability[i].first;
			auto type = si.factories[factory_index].type;
			if(type) {
				auto factory_workers = si.factories[factory_index].level * type->factory_workers.workforce * si.factories[factory_index].factory_operational_scale;
				for(uint32_t j = 0; j < std::extent_v<decltype(type->factory_workers.workers)>; ++j) {
					if(is_valid_index(type->factory_workers.workers[j].type)) {
						int32_t amount_from_type = int32_t(factory_workers * type->factory_workers.workers[j].amount);
						uint32_t allocation_index = to_index(type->factory_workers.workers[j].type) *  std::extent_v<decltype(si.factories)> + factory_index;
						allocation_by_type[allocation_index] = std::min(amount_from_type, unallocated_workers[to_index(type->factory_workers.workers[j].type)]);
						unallocated_workers[to_index(type->factory_workers.workers[j].type)] -= allocation_by_type[allocation_index];

						//average with current
						allocation_by_type[allocation_index] =
							(allocation_by_type[allocation_index] + 15) / 16 +
							(si.factories[factory_index].worker_data.worker_populations[j] * 15) / 16;
					}
				}
			}
		}

		for(uint32_t i = 0; i < ws.s.population_m.count_poptypes; ++i) 
			normalize_integer_vector(allocation_by_type + i * std::extent_v<decltype(si.factories)>, std::extent_v<decltype(si.factories)>, state_population_by_type[i] - unallocated_workers[i]);
		
		for(uint32_t i = 0; i < std::extent_v<decltype(si.factories)>; ++i) {
			auto factory_index = factories_by_profitability[i].first;
			auto type = si.factories[factory_index].type;
			if(type) {
				for(uint32_t j = 0; j < std::extent_v<decltype(type->factory_workers.workers)>; ++j) {
					if(is_valid_index(type->factory_workers.workers[j].type)) {
						uint32_t allocation_index = to_index(type->factory_workers.workers[j].type) *  std::extent_v<decltype(si.factories)> +factory_index;
						si.factories[factory_index].worker_data.worker_populations[j] = allocation_by_type[allocation_index];
					}
				}
			}
		}

		nations::for_each_pop(ws, si, [&ws, unallocated_workers, state_population_by_type](population::pop& p) {
			if((ws.s.population_m.pop_types[p.type].flags & population::pop_type::factory_worker) != 0) {
				ws.w.population_s.pop_demographics.get(p.id, population::total_employment_tag) =
					int32_t(float(ws.w.population_s.pop_demographics.get(p.id, population::total_population_tag)) * float(state_population_by_type[to_index(p.type)] - unallocated_workers[to_index(p.type)]) / float(state_population_by_type[to_index(p.type)]));
			}
		});
	}

	money_qnty_type get_factory_project_cost(world_state const& ws, factory_type_tag ftype, factory_project_type ptype, money_qnty_type const* prices) {
		if(ptype == factory_project_type::open || ptype == factory_project_type::expand) {
			Eigen::Map<const Eigen::Matrix<economy::money_qnty_type, -1, 1>, Eigen::Aligned32> prices_v(prices, ws.s.economy_m.aligned_32_goods_count);
			Eigen::Map<const Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::Aligned32> costs(ws.s.economy_m.building_costs.get_row(ftype), ws.s.economy_m.aligned_32_goods_count);

			return prices_v.dot(costs);
		} else {
			// TODO

			Eigen::Map<const Eigen::Matrix<economy::money_qnty_type, -1, 1>, Eigen::Aligned32> prices_v(prices, ws.s.economy_m.aligned_32_goods_count);
			Eigen::Map<const Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::Aligned32> costs(ws.s.economy_m.factory_input_goods.get_row(ftype), ws.s.economy_m.aligned_32_goods_count);

			return prices_v.dot(costs) * money_qnty_type(10);
		}
	}
	money_qnty_type get_railroad_cost(world_state const& ws, money_qnty_type const* prices) {
		Eigen::Map<const Eigen::Matrix<economy::money_qnty_type, -1, 1>, Eigen::Aligned32> prices_v(prices, ws.s.economy_m.aligned_32_goods_count);
		Eigen::Map<const Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::Aligned32> costs(ws.s.economy_m.building_costs.get_row(ws.s.economy_m.railroad.cost_tag), ws.s.economy_m.aligned_32_goods_count);
		
		return prices_v.dot(costs);
	}

	float project_completion(world_state const& ws, nations::state_instance const& si, money_qnty_type const* prices) {
		if(si.project.type == pop_project_type::railroad) {
			auto cost = get_railroad_cost(ws, prices);
			return cost != 0 ? float(si.project.funds / cost) : 0.0f;
		} else if(si.project.type == pop_project_type::factory) {
			if(auto ftype = si.project.factory_type; is_valid_index(ftype)) {
				auto cost = get_factory_project_cost(ws, ftype, get_factory_project_type(si, ftype), prices);
				return cost != 0 ? float(si.project.funds / cost) : 0.0f;
			}
		}
		return 0.0f;
	}

	float get_per_worker_profit(world_state const& ws, nations::state_instance const& si, factory_instance const& f) {
		if(!f.type)
			return std::numeric_limits<float>::min();

		Eigen::Map<const Eigen::Matrix<economy::money_qnty_type, -1, 1>, Eigen::Aligned32> prices(state_current_prices(ws, si), ws.s.economy_m.aligned_32_goods_count);
		Eigen::Map<const Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::Aligned32> inputs(ws.s.economy_m.factory_input_goods.get_row(f.type->id), ws.s.economy_m.aligned_32_goods_count);

		auto inputs_cost = prices.dot(inputs);

		return float(prices[to_index(f.type->output_good)] * f.type->output_amount - inputs_cost);
	}

	money_qnty_type get_factory_profit(world_state const& ws, provinces::province_state const& in_province, factory_instance const& f, money_qnty_type const* prices) {
		auto powner = in_province.owner;
		auto f_type = f.type;
		if(bool(powner) && bool(f_type)) {
			auto modifiers = factory_production_modifiers(ws, f.worker_data, f_type->bonuses,
				f_type->factory_workers, *powner, in_province, f.level, f.factory_operational_scale,
				f_type->output_good,
				((powner->flags & nations::nation::is_mobilized) == 0) ? 1.0f : std::max(0.0f, 1.0f - powner->modifier_values[modifiers::national_offsets::mobilisation_size] * powner->modifier_values[modifiers::national_offsets::mobilisation_economy_impact]));

			//TODO: use modifiers properly

			Eigen::Map<const Eigen::Matrix<economy::money_qnty_type, -1, 1>, Eigen::Aligned32> prices_v(prices, ws.s.economy_m.aligned_32_goods_count);
			Eigen::Map<const Eigen::Matrix<economy::money_qnty_type, -1, 1>, Eigen::Aligned32> inputs(ws.s.economy_m.factory_input_goods.get_row(f_type->id), ws.s.economy_m.aligned_32_goods_count);

			auto inputs_cost = prices_v.dot(inputs * modifiers.input_modifier);

			return (prices[to_index(f_type->output_good)] * f_type->output_amount * modifiers.output_modifier - inputs_cost) * f.factory_operational_scale * f.level * modifiers.throughput_modifier;
		}
		return money_qnty_type(0);
	}

	float factory_employment_fraction(world_state const& ws, factory_instance const& fi) {
		if(auto f_type = fi.type; f_type) {
			float total_workforce = float(f_type->factory_workers.workforce * fi.level);
			float total_employed = float(std::accumulate(std::begin(fi.worker_data.worker_populations), std::end(fi.worker_data.worker_populations), 0));
			return total_workforce != 0.0f ? total_employed / total_workforce : 0.0f;
		} else {
			return 0.0f;
		}
	}

	void init_factory_employment(world_state& ws) {
		ws.w.nation_s.states.for_each([&ws](nations::state_instance& si) {
			if(si.owner) {
				for(uint32_t i = 0; i < std::extent_v<decltype(si.factories)>; ++i) {
					if(si.factories[i].type && si.factories[i].level != 0) {
						for(uint32_t j = 0; j < std::extent_v<decltype(si.factories[i].type->factory_workers.workers)>; ++j) {
							if(is_valid_index(si.factories[i].type->factory_workers.workers[j].type))
								si.factories[i].worker_data.worker_populations[j] = 100;
						}
					}
				}

				update_factories_employment(ws, si);
			}
		});
	}

	bool possible_to_invest_in(world_state const& ws, nations::nation const& investor, nations::nation const& target) {
		return !nations::is_great_power(ws, target) && nations::is_great_power(ws, investor) &&
			((investor.current_rules.rules_value & (issues::rules::open_factory_invest | issues::rules::expand_factory_invest | issues::rules::build_factory_invest)) != 0) &&
			((target.current_rules.rules_value & issues::rules::allow_foreign_investment) != 0) && 
			(investor.id != target.id);
	}

	int32_t count_factories_in_state(nations::state_instance const& si) {
		return
			std::accumulate(
				std::begin(si.factories),
				std::end(si.factories),
				0i32,
				[](int32_t v, economy::factory_instance const& f) { return v + int32_t(f.level != 0); });
	}

	int32_t count_factories_in_nation(world_state const& ws, nations::nation const& n) {
		auto states = get_range(ws.w.nation_s.state_arrays, n.member_states);
		int32_t total = 0;
		for(auto s = states.first; s != states.second; ++s) {
			if(auto si = s->state; si) {
				total += count_factories_in_state(*si);
			}
		}
		return total;
	}

	float average_railroad_level(world_state const& ws, nations::state_instance const& si) {
		float total_provinces = 0.0f;
		float total_levels = 0.0f;
		nations::for_each_province(ws, si, [&total_provinces, &total_levels](provinces::province_state const& ps) {
			total_levels += float(ps.railroad_level);
			++total_provinces;
		});
		return total_provinces != 0.0f ? total_levels / total_provinces : 0.0f;
	}

	bool factory_is_open(factory_instance const& fi) {
		return bool(fi.type) && fi.level != 0 && fi.factory_operational_scale > 0.0f;
	}
	bool factory_is_closed(factory_instance const& fi) {
		return bool(fi.type) && fi.level != 0 && fi.factory_operational_scale <= 0.0f;
	}
	bool factory_is_under_construction(factory_instance const& fi) {
		return bool(fi.type) && fi.level == 0 && fi.factory_progress > 0.0f;
	}
	bool factory_is_upgrading(factory_instance const& fi) {
		return bool(fi.type) && fi.level != 0 && fi.factory_progress > 0.0f;
	}
	factory_project_type get_factory_project_type(nations::state_instance const& location, factory_type_tag ftype) {
		for(auto& fi : location.factories) {
			if(auto ft = fi.type; bool(ft) && ft->id == ftype) {
				if(factory_is_closed(fi))
					return factory_project_type::reopen;
				else
					return factory_project_type::expand;
			}
		}
		return factory_project_type::open;
	}

	uint32_t storage_space_for_n_neighbors(world_state const& ws, uint32_t neighbor_count) {
		const uint32_t gc = ws.s.economy_m.aligned_32_goods_count;
		return gc * 4ui32 + //local demand & production quantities
			neighbor_count * gc * 2ui32 // current and previous imports from neighbors
			;
	}

	goods_qnty_type* imports_for_nth_neighbor(world_state const& ws, goods_qnty_type* data, uint32_t neighbor_count) {
		const uint32_t gc = ws.s.economy_m.aligned_32_goods_count;
		return data + gc * 4ui32 + neighbor_count * gc * 2ui32;
	}

	void allocate_new_state_production(world_state& ws, nations::state_instance& si) {
		resize(ws.w.nation_s.state_goods_arrays, si.production_imports_arrays, storage_space_for_n_neighbors(ws, 0));
		auto g_range = get_range(ws.w.nation_s.state_goods_arrays, si.production_imports_arrays);
		std::fill_n(g_range.first, storage_space_for_n_neighbors(ws, 0), economy::goods_qnty_type(1));
	}

	constexpr economy::goods_qnty_type fixed_supply_demand_adjustment = economy::goods_qnty_type(0.001);
	constexpr float slowdown_factor = 0.5f;

	inline float production_rescale(
		production_modifiers const& mod,
		float current_production_scale,
		float fixed_scale_value,
		Eigen::Map<Eigen::Matrix<economy::money_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> const& prices,
		Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> const& old_supply,
		Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> const& old_demand,
		Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> const& inputs,
		goods_tag output_type,
		economy::goods_qnty_type output_qnty) {

		Eigen::Map <Eigen::Array<economy::goods_qnty_type, -1, 1>> demand_supply_sum((economy::goods_qnty_type*)_alloca(sizeof(economy::goods_qnty_type) * old_demand.rows()), old_demand.rows());
		demand_supply_sum = old_supply.array() + old_demand.array();

		Eigen::Map <Eigen::Array<economy::goods_qnty_type, -1, 1>> with_nan_array((economy::goods_qnty_type*)_alloca(sizeof(economy::goods_qnty_type) * old_demand.rows()), old_demand.rows());
		
		with_nan_array = economy::goods_qnty_type(2) * fixed_scale_value * (economy::goods_qnty_type(1) - (old_supply.array() * (demand_supply_sum - inputs.array() * current_production_scale * fixed_scale_value)) / (demand_supply_sum).square());
		auto input_first_derivative = economy::money_qnty_type(-1) * prices.dot(with_nan_array.isNaN().select(economy::goods_qnty_type(0), with_nan_array).matrix());

		with_nan_array = economy::goods_qnty_type(4) * fixed_scale_value * fixed_scale_value * old_supply.array() *
			(demand_supply_sum - inputs.array() * current_production_scale * fixed_scale_value).array() / demand_supply_sum.cube();
		auto input_second_derivative = economy::money_qnty_type(-1) * prices.dot(with_nan_array.isNaN().select(economy::goods_qnty_type(0), with_nan_array).matrix());
		
		auto output_first_derivative = economy::goods_qnty_type(2) * fixed_scale_value * old_demand[to_index(output_type)] * prices[to_index(output_type)] * (demand_supply_sum[to_index(output_type)] - current_production_scale * fixed_scale_value * output_qnty) / (demand_supply_sum[to_index(output_type)] * demand_supply_sum[to_index(output_type)]);
		auto output_second_derivative = economy::goods_qnty_type(-4) * fixed_scale_value * fixed_scale_value* old_demand[to_index(output_type)] * prices[to_index(output_type)] * (demand_supply_sum[to_index(output_type)] - current_production_scale * fixed_scale_value * output_qnty) / (demand_supply_sum[to_index(output_type)] * demand_supply_sum[to_index(output_type)] * demand_supply_sum[to_index(output_type)]);
		
		auto second_derivative = input_second_derivative + output_second_derivative;
		if(second_derivative != 0)
			return std::clamp(current_production_scale + slowdown_factor * (input_first_derivative + output_first_derivative) / std::abs(second_derivative), 0.0f, 1.0f);
		else
			return current_production_scale;
		/*
		auto base_inputs_qnty = (inputs * fixed_scale_value * mod.input_modifier * mod.throughput_modifier).array();
		auto old_demand_minus_inputs = (old_demand.array() - base_inputs_qnty * current_production_scale).array();
		auto input_derivative_array = base_inputs_qnty * (old_demand_minus_inputs + base_inputs_qnty * goods_qnty_type(2) * current_production_scale) / (old_supply.array() + Eigen::ArrayXf::Constant(fixed_supply_demand_adjustment, old_demand.rows()));
		
		auto input_derivative = economy::money_qnty_type(-1) * prices.dot(input_derivative_array.matrix());
		auto input_second_derivative = economy::money_qnty_type(-1) * prices.dot((base_inputs_qnty * base_inputs_qnty * goods_qnty_type(2) / old_supply.array()).matrix());

		auto base_output_qnty = output_qnty * fixed_scale_value * mod.output_modifier * mod.throughput_modifier;
		auto old_supply_minus_output = old_supply[to_index(output_type)] - base_output_qnty * current_production_scale + fixed_supply_demand_adjustment;
		
		auto output_derivate_denom = old_supply_minus_output + base_output_qnty * current_production_scale;
		auto output_derivative = old_demand[to_index(output_type)] * old_supply_minus_output * base_output_qnty * prices[to_index(output_type)] / (output_derivate_denom * output_derivate_denom);
		auto output_second_derivative = economy::money_qnty_type(-2) * old_demand[to_index(output_type)] * old_supply_minus_output * base_output_qnty * base_output_qnty * prices[to_index(output_type)] / (output_derivate_denom * output_derivate_denom * output_derivate_denom);

		auto second_derivative = input_second_derivative + output_second_derivative;
		if(second_derivative != 0)
			return std::clamp(current_production_scale + slowdown_factor * std::copysign((input_derivative + output_derivative) / (second_derivative), input_derivative + output_derivative), 0.0f, 1.0f);
		else
			return current_production_scale;
		*/
	}

	constexpr float imports_slowdown_factor = 0.5f;
	constexpr float pop_needs_divisor = 200'000.0f;

	inline void import_rescale(
		Eigen::Map<Eigen::Matrix<economy::money_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> const& local_prices,
		Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> const& old_local_supply,
		Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> const& old_local_demand,
		Eigen::Map<Eigen::Matrix<economy::money_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> const& remote_prices,
		Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> const& old_remote_supply,
		Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> const& old_remote_demand,
		Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> const& current_imports,
		Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32>& new_imports,
		goods_tag coal,
		economy::goods_qnty_type coal_cost_multiplier
		) {

		Eigen::Map <Eigen::Array<economy::goods_qnty_type, -1, 1>> local_demand_supply_sum((economy::goods_qnty_type*)_alloca(sizeof(economy::goods_qnty_type) * local_prices.rows()), local_prices.rows());
		local_demand_supply_sum = old_local_supply.array() + old_local_demand.array();

		Eigen::Map <Eigen::Array<economy::goods_qnty_type, -1, 1>> remote_demand_supply_sum((economy::goods_qnty_type*)_alloca(sizeof(economy::goods_qnty_type) * local_prices.rows()), local_prices.rows());
		remote_demand_supply_sum = old_remote_supply.array() + old_remote_demand.array();

		auto profits_first_derivative = economy::money_qnty_type(2) * local_prices.array() * old_local_demand.array() * (local_demand_supply_sum - current_imports.array()) / local_demand_supply_sum.square();
		auto profits_second_derivative = economy::money_qnty_type(-4) * local_prices.array() * old_local_demand.array() *(local_demand_supply_sum - current_imports.array()) / local_demand_supply_sum.cube();

		auto costs_first_derivative = economy::money_qnty_type(-2) * remote_prices.array() * (Eigen::ArrayXf::Ones(remote_prices.rows()) - old_remote_supply.array() * (remote_demand_supply_sum - current_imports.array()) / remote_demand_supply_sum.square());
		auto costs_second_derivative = economy::money_qnty_type(-4) * remote_prices.array() * old_remote_supply.array() * (remote_demand_supply_sum - current_imports.array()) / remote_demand_supply_sum.cube();

		auto local_coal_demand_p_supply_value = old_local_supply[to_index(coal)] + old_local_demand[to_index(coal)];

		auto coal_first_derivative = economy::money_qnty_type(-2 * coal_cost_multiplier * local_prices[to_index(coal)]) * (economy::goods_qnty_type(1) - old_local_supply[to_index(coal)] * (local_coal_demand_p_supply_value - current_imports.array() * coal_cost_multiplier) / (local_coal_demand_p_supply_value * local_coal_demand_p_supply_value));
		auto coal_second_derivative = economy::money_qnty_type(-4 * coal_cost_multiplier * local_prices[to_index(coal)] * old_local_supply[to_index(coal)] / (local_coal_demand_p_supply_value * local_coal_demand_p_supply_value * local_coal_demand_p_supply_value)) * (local_coal_demand_p_supply_value - current_imports.array() * coal_cost_multiplier);

		Eigen::Map <Eigen::Array<economy::goods_qnty_type, -1, 1>> with_nan_array((economy::goods_qnty_type*)_alloca(sizeof(economy::goods_qnty_type) * local_prices.rows()), local_prices.rows());
		with_nan_array = imports_slowdown_factor * (profits_first_derivative + costs_first_derivative + coal_first_derivative) / (profits_second_derivative + costs_second_derivative + coal_second_derivative).abs();
		new_imports = (current_imports.array() + with_nan_array.isNaN().select(economy::goods_qnty_type(0), with_nan_array)).max(economy::goods_qnty_type(0)).matrix();
	}

	void apply_pop_consumption(world_state & ws, provinces::province_state & p, Eigen::Map<Eigen::VectorXf, Eigen::AlignmentType::Aligned32> &state_consumption_v, Eigen::Map<Eigen::VectorXf, Eigen::AlignmentType::Aligned32> &state_old_production_v, const Eigen::Map<Eigen::VectorXf, Eigen::AlignmentType::Aligned32> &state_old_consumption_v, economy::money_qnty_type * life_needs_cost_by_type, economy::money_qnty_type * everyday_needs_cost_by_type, economy::money_qnty_type * luxury_needs_cost_by_type) {
		provinces::for_each_pop(ws, p, [&ws, &state_consumption_v, &state_old_production_v, &state_old_consumption_v, life_needs_cost_by_type, everyday_needs_cost_by_type, luxury_needs_cost_by_type](population::pop& po) {
			float pop_size_multiplier = float(ws.w.population_s.pop_demographics.get(po.id, population::total_population_tag)) / pop_needs_divisor;

			if(po.money < (pop_size_multiplier * life_needs_cost_by_type[to_index(po.type)])) {
				// case: can only partially satisfy life needs
				auto fraction = po.money / (pop_size_multiplier * life_needs_cost_by_type[to_index(po.type)]);
				po.needs_satisfaction = fraction;
				state_consumption_v += (fraction * pop_size_multiplier) * Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32>(ws.s.population_m.life_needs.get_row(po.type), ws.s.economy_m.aligned_32_goods_count);
			} else {
				// case: can fully satisfy life needs
				state_consumption_v += pop_size_multiplier * Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32>(ws.s.population_m.life_needs.get_row(po.type), ws.s.economy_m.aligned_32_goods_count);
				po.money -= pop_size_multiplier * life_needs_cost_by_type[to_index(po.type)];

				if(po.money < pop_size_multiplier * everyday_needs_cost_by_type[to_index(po.type)]) {
					// case: can partially satisfy everyday needs

					auto fraction = po.money / (pop_size_multiplier * everyday_needs_cost_by_type[to_index(po.type)]);
					po.needs_satisfaction = 1.0f + fraction;
					state_consumption_v += (fraction * pop_size_multiplier) * Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32>(ws.s.population_m.everyday_needs.get_row(po.type), ws.s.economy_m.aligned_32_goods_count);
				} else {
					// case: can fully satisfy everyday needs
					state_consumption_v += pop_size_multiplier * Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32>(ws.s.population_m.everyday_needs.get_row(po.type), ws.s.economy_m.aligned_32_goods_count);
					po.money -= pop_size_multiplier * everyday_needs_cost_by_type[to_index(po.type)];

					// remainder of money spent on luxury needs
					auto last_fraction = po.money / (pop_size_multiplier * luxury_needs_cost_by_type[to_index(po.type)]);
					po.needs_satisfaction = 2.0f + last_fraction;

					state_consumption_v += (pop_size_multiplier * last_fraction) * Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32>(ws.s.population_m.luxury_needs.get_row(po.type), ws.s.economy_m.aligned_32_goods_count);
				}
			}

			po.money = 0.0f;
		});
	}

	constexpr economy::goods_qnty_type coal_consumption_factor = 0.0005f;

	void fill_needs_costs_arrays(world_state & ws, nations::state_instance const& si, provinces::province_state const& state_capital, Eigen::Map<Eigen::VectorXf, Eigen::AlignmentType::Aligned32> const& state_prices_v,
		economy::money_qnty_type * life_needs_cost_by_type, economy::money_qnty_type * everyday_needs_cost_by_type, economy::money_qnty_type * luxury_needs_cost_by_type) {
		for(population::pop_type_tag::value_base_t i = 0; i < ws.s.population_m.count_poptypes; ++i) {
			population::pop_type_tag this_type(i);
			auto this_strata = ws.s.population_m.pop_types[this_type].flags & population::pop_type::strata_mask;

			economy::money_qnty_type ln_factor;
			economy::money_qnty_type ev_factor;
			economy::money_qnty_type lx_factor;

			if(this_strata == population::pop_type::strata_poor) {
				ln_factor = economy::money_qnty_type(1) + si.owner->modifier_values[modifiers::national_offsets::poor_life_needs] + state_capital.modifier_values[modifiers::provincial_offsets::poor_life_needs];
				ev_factor = economy::money_qnty_type(1) + si.owner->modifier_values[modifiers::national_offsets::poor_everyday_needs] + state_capital.modifier_values[modifiers::provincial_offsets::poor_everyday_needs];
				lx_factor = economy::money_qnty_type(1) + si.owner->modifier_values[modifiers::national_offsets::poor_luxury_needs] + state_capital.modifier_values[modifiers::provincial_offsets::poor_luxury_needs];
			} else if(this_strata == population::pop_type::strata_middle) {
				ln_factor = economy::money_qnty_type(1) + si.owner->modifier_values[modifiers::national_offsets::middle_life_needs] + state_capital.modifier_values[modifiers::provincial_offsets::middle_life_needs];
				ev_factor = economy::money_qnty_type(1) + si.owner->modifier_values[modifiers::national_offsets::middle_everyday_needs] + state_capital.modifier_values[modifiers::provincial_offsets::middle_everyday_needs];
				lx_factor = economy::money_qnty_type(1) + si.owner->modifier_values[modifiers::national_offsets::middle_luxury_needs] + state_capital.modifier_values[modifiers::provincial_offsets::middle_luxury_needs];
			} else { //if(this_strata == population::pop_type::strata_rich) {
				ln_factor = economy::money_qnty_type(1) + si.owner->modifier_values[modifiers::national_offsets::rich_life_needs] + state_capital.modifier_values[modifiers::provincial_offsets::rich_life_needs];
				ev_factor = economy::money_qnty_type(1) + si.owner->modifier_values[modifiers::national_offsets::rich_everyday_needs] + state_capital.modifier_values[modifiers::provincial_offsets::rich_everyday_needs];
				lx_factor = economy::money_qnty_type(1) + si.owner->modifier_values[modifiers::national_offsets::rich_luxury_needs] + state_capital.modifier_values[modifiers::provincial_offsets::rich_luxury_needs];
			}

			life_needs_cost_by_type[i] = ln_factor * state_prices_v.dot(Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32>(ws.s.population_m.life_needs.get_row(this_type), ws.s.economy_m.aligned_32_goods_count));
			everyday_needs_cost_by_type[i] = ev_factor * state_prices_v.dot(Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32>(ws.s.population_m.everyday_needs.get_row(this_type), ws.s.economy_m.aligned_32_goods_count));
			luxury_needs_cost_by_type[i] = lx_factor * state_prices_v.dot(Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32>(ws.s.population_m.luxury_needs.get_row(this_type), ws.s.economy_m.aligned_32_goods_count));
		}
	}

	void pay_workers(world_state & ws, economy::workers_information const& workers_info, economy::worked_instance const& workers,
		economy::money_qnty_type profit, economy::money_qnty_type* pay_by_type, economy::money_qnty_type* life_needs_cost_by_type, bool owners_present) {
		if(profit <= economy::money_qnty_type(0))
			return;

		economy::money_qnty_type* temp_pay_by_type = (economy::money_qnty_type*)_alloca(sizeof(economy::money_qnty_type) * ws.s.population_m.count_poptypes);
		std::fill_n(temp_pay_by_type, ws.s.population_m.count_poptypes, economy::money_qnty_type(0));

		economy::money_qnty_type total_min_wage = economy::money_qnty_type(0);

		for(uint32_t i = 0; i < std::extent_v<decltype(workers_info.workers)>; ++i) {
			if(is_valid_index(workers_info.workers[i].type)) {
				// = minimum wage
				temp_pay_by_type[to_index(workers_info.workers[i].type)] = life_needs_cost_by_type[to_index(workers_info.workers[i].type)] * float(workers.worker_populations[i]) / pop_needs_divisor;
				total_min_wage += temp_pay_by_type[to_index(workers_info.workers[i].type)];
			}
		}

		if(owners_present) {
			if(profit > total_min_wage * 2.0f) {
				for(uint32_t i = 0; i < std::extent_v<decltype(workers_info.workers)>; ++i) {
					if(is_valid_index(workers_info.workers[i].type))
						pay_by_type[to_index(workers_info.workers[i].type)] += (temp_pay_by_type[to_index(workers_info.workers[i].type)] / total_min_wage) * (total_min_wage / 2.0f + profit / 4.0f);
				}
			} else {
				for(uint32_t i = 0; i < std::extent_v<decltype(workers_info.workers)>; ++i) {
					if(is_valid_index(workers_info.workers[i].type))
						pay_by_type[to_index(workers_info.workers[i].type)] += temp_pay_by_type[to_index(workers_info.workers[i].type)] * profit / (total_min_wage * 2.0f);
				}
			}
			pay_by_type[to_index(workers_info.owner.type)] = profit <= total_min_wage * 2.0f ? (profit / 2.0f) : (profit * 3.0f / 4.0f - total_min_wage / 2.0f);
		} else {
			for(uint32_t i = 0; i < std::extent_v<decltype(workers_info.workers)>; ++i) {
				if(is_valid_index(workers_info.workers[i].type))
					pay_by_type[to_index(workers_info.workers[i].type)] += (temp_pay_by_type[to_index(workers_info.workers[i].type)] * profit) / total_min_wage;
			}
		}
	}

	constexpr float price_change_slowdown_factor = 0.5f;
	constexpr economy::money_qnty_type minimum_price = economy::money_qnty_type(0.01);

	void update_state_production_and_consumption(world_state& ws, nations::state_instance& si) {
		if(!(si.owner))
			return;

		const auto state_capital = nations::get_state_capital(ws, si);
		if(!state_capital)
			return;

		const float mobilization_effect = ((si.owner->flags & nations::nation::is_mobilized) == 0) ?
			1.0f : std::max(0.0f, 1.0f - si.owner->modifier_values[modifiers::national_offsets::mobilisation_size] * si.owner->modifier_values[modifiers::national_offsets::mobilisation_economy_impact]);
		auto state_production_data = get_range(ws.w.nation_s.state_goods_arrays, si.production_imports_arrays).first + (ws.s.economy_m.aligned_32_goods_count * (to_index(ws.w.current_date) & 1));
		auto state_consumption_data = state_production_data + ws.s.economy_m.aligned_32_goods_count * 2ui32;

		auto state_old_production_data = get_range(ws.w.nation_s.state_goods_arrays, si.production_imports_arrays).first + (ws.s.economy_m.aligned_32_goods_count * (1ui32 - (to_index(ws.w.current_date) & 1)));
		auto state_old_consumption_data = state_old_production_data + ws.s.economy_m.aligned_32_goods_count * 2ui32;

		Eigen::Map<Eigen::Matrix<float, -1, 1>, Eigen::AlignmentType::Aligned32> state_prices_v(
			ws.w.nation_s.state_prices.get_row(si.id) + (ws.s.economy_m.aligned_32_goods_count * (to_index(ws.w.current_date) & 1)),
			ws.s.economy_m.aligned_32_goods_count);

		Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> state_production_v(state_production_data, ws.s.economy_m.aligned_32_goods_count);
		Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> state_consumption_v(state_consumption_data, ws.s.economy_m.aligned_32_goods_count);
		Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> state_old_production_v(state_old_production_data, ws.s.economy_m.aligned_32_goods_count);
		Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> state_old_consumption_v(state_old_consumption_data, ws.s.economy_m.aligned_32_goods_count);
		state_production_v.setZero();
		state_consumption_v.setZero();

		economy::money_qnty_type* life_needs_cost_by_type = (economy::money_qnty_type*)_alloca(sizeof(economy::money_qnty_type) * ws.s.population_m.count_poptypes);
		economy::money_qnty_type* everyday_needs_cost_by_type = (economy::money_qnty_type*)_alloca(sizeof(economy::money_qnty_type) * ws.s.population_m.count_poptypes);
		economy::money_qnty_type* luxury_needs_cost_by_type = (economy::money_qnty_type*)_alloca(sizeof(economy::money_qnty_type) * ws.s.population_m.count_poptypes);

		fill_needs_costs_arrays(ws, si, *state_capital, state_prices_v, life_needs_cost_by_type, everyday_needs_cost_by_type, luxury_needs_cost_by_type);

		nations::for_each_province(ws, si, [&ws, &state_consumption_v, &state_production_v, &state_old_production_v, &state_old_consumption_v, &state_prices_v, &si, mobilization_effect, life_needs_cost_by_type, everyday_needs_cost_by_type, luxury_needs_cost_by_type](provinces::province_state& p) {
			// pop conumption before any pops have been paid
			apply_pop_consumption(ws, p, state_consumption_v, state_old_production_v, state_old_consumption_v,
				life_needs_cost_by_type, everyday_needs_cost_by_type, luxury_needs_cost_by_type);

			economy::money_qnty_type* province_pay_by_type = (economy::money_qnty_type*)_alloca(sizeof(economy::money_qnty_type) * ws.s.population_m.count_poptypes);
			std::fill_n(province_pay_by_type, ws.s.population_m.count_poptypes, economy::money_qnty_type(0));
			auto province_population_by_type = ws.w.province_s.province_demographics.get_row(p.id) + to_index(population::to_demo_tag(ws, population::pop_type_tag(0)));

			// artisan production, consumption & profit

			if(province_population_by_type[to_index(ws.s.population_m.artisan)] != 0) {
				auto artisan_modifiers = artisan_production_modifiers(ws, *si.owner, p, p.artisan_production, mobilization_effect);
				const auto artisan_id = ws.s.economy_m.goods[p.artisan_production].artisan_id;
				auto& artisan_formula = ws.s.economy_m.artisan_types[artisan_id];
				float artisan_size_multiplier = p.artisan_production_scale * float(province_population_by_type[to_index(ws.s.population_m.artisan)]) / float(artisan_formula.workforce);

				Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> input_v(ws.s.economy_m.artisan_input_goods.get_row(artisan_id), ws.s.economy_m.aligned_32_goods_count);
				auto artisan_inputs_cost = state_prices_v.dot(input_v) * artisan_modifiers.input_modifier;
				province_pay_by_type[to_index(ws.s.population_m.artisan)] =
					(state_prices_v[to_index(artisan_formula.output_good)] * artisan_formula.output_amount * artisan_modifiers.output_modifier - artisan_inputs_cost) * artisan_size_multiplier * artisan_modifiers.throughput_modifier;

				state_consumption_v += (artisan_modifiers.input_modifier * artisan_modifiers.throughput_modifier * artisan_size_multiplier) * input_v;
				state_production_v[to_index(artisan_formula.output_good)] += artisan_formula.output_amount * artisan_modifiers.output_modifier * artisan_modifiers.throughput_modifier * artisan_size_multiplier;

				// handle failure to make profit, scaling production, changing production

				p.artisan_production_scale = production_rescale(artisan_modifiers, p.artisan_production_scale, float(province_population_by_type[to_index(ws.s.population_m.artisan)]) / float(artisan_formula.workforce),
					state_prices_v, state_old_production_v, state_old_consumption_v, input_v, artisan_formula.output_good, artisan_formula.output_amount);
				
				if(p.artisan_production_scale < 0.1f) {
					// change production type
				}
				if(province_pay_by_type[to_index(ws.s.population_m.artisan)] < 0.0f)
					province_pay_by_type[to_index(ws.s.population_m.artisan)] = 0.0f;
			}

			// rgo production & profit
			auto& rgo_type = ((ws.s.economy_m.goods[p.rgo_production].flags & good_definition::mined) != 0) ? ws.s.economy_m.rgo_mine : ws.s.economy_m.rgo_farm;
			float rgo_profit = 0.0f;
			
			{
				auto rgo_modifiers = rgo_production_modifiers(ws, p.rgo_worker_data, rgo_type,
					*si.owner, p, p.rgo_size, 1.0f, p.rgo_production, mobilization_effect);
				auto output_amount = ws.s.economy_m.goods[p.rgo_production].base_rgo_value * p.rgo_size * rgo_modifiers.output_modifier * rgo_modifiers.throughput_modifier;
				state_production_v[to_index(p.rgo_production)] += output_amount;
				rgo_profit = output_amount * state_prices_v[to_index(p.rgo_production)];
			}

			//pay pops
			pay_workers(ws, rgo_type, p.rgo_worker_data,
				rgo_profit, province_pay_by_type, life_needs_cost_by_type,
				province_population_by_type[to_index(rgo_type.owner.type)] != 0);

			provinces::for_each_pop(ws, p, [province_population_by_type, province_pay_by_type](population::pop& po) {
				po.money = province_pay_by_type[to_index(po.type)] / float(province_population_by_type[to_index(po.type)]);
			});
		});

		economy::money_qnty_type* state_pay_by_type = (economy::money_qnty_type*)_alloca(sizeof(economy::money_qnty_type) * ws.s.population_m.count_poptypes);
		std::fill_n(state_pay_by_type, ws.s.population_m.count_poptypes, economy::money_qnty_type(0));
		auto state_population_by_type = ws.w.nation_s.state_demographics.get_row(si.id) + to_index(population::to_demo_tag(ws, population::pop_type_tag(0)));


		// factory production, consumption, & profit
		for(uint32_t i = 0; i < std::extent_v<decltype(si.factories)>; ++i) {
			auto& f = si.factories[i];
			if(factory_is_open(f)) {
				auto factory_modifiers = factory_production_modifiers(ws, f.worker_data, f.type->bonuses,
					f.type->factory_workers, *si.owner, *state_capital, f.level, f.factory_operational_scale,
					f.type->output_good, mobilization_effect);

				Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> input_v(
					ws.s.economy_m.factory_input_goods.get_row(f.type->id), ws.s.economy_m.aligned_32_goods_count);
				
				auto factory_profit = (state_prices_v[to_index(f.type->output_good)] * f.type->output_amount * factory_modifiers.output_modifier - state_prices_v.dot(input_v) * factory_modifiers.input_modifier)
					* factory_modifiers.throughput_modifier * f.level;

				state_consumption_v += (factory_modifiers.input_modifier * factory_modifiers.throughput_modifier * f.level) * input_v;
				state_production_v[to_index(f.type->output_good)] += f.type->output_amount * factory_modifiers.output_modifier * factory_modifiers.throughput_modifier * f.level;

				// calculate pay
				pay_workers(ws, f.type->factory_workers, f.worker_data,
					factory_profit, state_pay_by_type, life_needs_cost_by_type,
					state_population_by_type[to_index(f.type->factory_workers.owner.type)] != 0);
				
				//  scale production

				f.factory_operational_scale = production_rescale(factory_modifiers, f.factory_operational_scale, f.level,
					state_prices_v, state_old_production_v, state_old_consumption_v, input_v, f.type->output_good, f.type->output_amount);

			}
		}

		// import & export costs and profit
		auto neighbor_range = get_range(ws.w.nation_s.state_neighbor_arrays, si.neighbors);
		const auto neighbor_data_base = get_range(ws.w.nation_s.state_goods_arrays, si.production_imports_arrays).first;
		for(auto n = neighbor_range.first; n != neighbor_range.second; ++n) {
			Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> import_amounts(
				imports_for_nth_neighbor(ws, neighbor_data_base, n->neighbor_index) + (ws.s.economy_m.aligned_32_goods_count * (1 - (to_index(ws.w.current_date) & 1))),
				ws.s.economy_m.aligned_32_goods_count);

			const auto neighbor_production_data = get_range(ws.w.nation_s.state_goods_arrays, ws.w.nation_s.states[n->neighbor_tag].production_imports_arrays).first;

			auto rev_neigbor = find(ws.w.nation_s.state_neighbor_arrays, ws.w.nation_s.states[n->neighbor_tag].neighbors, nations::state_neighbor{0.0f, si.id, 0ui16});
			
			Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> export_amounts(
				imports_for_nth_neighbor(ws, neighbor_production_data, rev_neigbor->neighbor_index) + ws.s.economy_m.aligned_32_goods_count * 2ui32 + (ws.s.economy_m.aligned_32_goods_count * (1 - (to_index(ws.w.current_date) & 1))),
				ws.s.economy_m.aligned_32_goods_count);

			Eigen::Map<Eigen::Matrix<float, -1, 1>, Eigen::AlignmentType::Aligned32> neighbor_state_prices_v(
				ws.w.nation_s.state_prices.get_row(n->neighbor_tag) + (ws.s.economy_m.aligned_32_goods_count * (to_index(ws.w.current_date) & 1)),
				ws.s.economy_m.aligned_32_goods_count);

			state_production_v += import_amounts;
			state_consumption_v += export_amounts;

			auto coal_consumed = import_amounts.sum() * coal_consumption_factor * n->distance;
			state_consumption_v[to_index(ws.w.economy_s.coal)] += coal_consumed;

			// profits to capitalists
			state_pay_by_type[to_index(ws.s.population_m.capitalist)] += std::max(0.0f, 
				+ state_prices_v.dot(import_amounts) - neighbor_state_prices_v.dot(import_amounts)
				- coal_consumed * state_prices_v[to_index(ws.w.economy_s.coal)]
				);

			// calculate updated imports
			Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> updated_import_amounts(
				imports_for_nth_neighbor(ws, neighbor_data_base, n->neighbor_index) + (ws.s.economy_m.aligned_32_goods_count * (to_index(ws.w.current_date) & 1)),
				ws.s.economy_m.aligned_32_goods_count);

			auto neigbor_state_old_production_data = neighbor_production_data + (ws.s.economy_m.aligned_32_goods_count * (1ui32 - (to_index(ws.w.current_date) & 1)));
			auto neigbor_state_old_consumption_data = neigbor_state_old_production_data + ws.s.economy_m.aligned_32_goods_count * 2ui32;
			Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> neigbor_old_production_v(neigbor_state_old_production_data, ws.s.economy_m.aligned_32_goods_count);
			Eigen::Map<Eigen::Matrix<economy::goods_qnty_type, -1, 1>, Eigen::AlignmentType::Aligned32> neigbor_old_consumption_v(neigbor_state_old_consumption_data, ws.s.economy_m.aligned_32_goods_count);

			import_rescale(
				state_prices_v, state_old_production_v, state_old_consumption_v,
				neighbor_state_prices_v, neigbor_old_production_v, neigbor_old_consumption_v,
				import_amounts, updated_import_amounts, ws.w.economy_s.coal, coal_consumption_factor * n->distance);
		}

		// pay pops
		nations::for_each_pop(ws, si, [state_population_by_type, state_pay_by_type](population::pop& po) {
			po.money += state_pay_by_type[to_index(po.type)] / float(state_population_by_type[to_index(po.type)]);
		});

		//adjust prices

		Eigen::Map<Eigen::Array<float, -1, 1>, Eigen::AlignmentType::Aligned32> new_state_prices(
			ws.w.nation_s.state_prices.get_row(si.id) + (ws.s.economy_m.aligned_32_goods_count * (1ui32 - (to_index(ws.w.current_date) & 1))),
			ws.s.economy_m.aligned_32_goods_count);
		new_state_prices = (economy::money_qnty_type(1.0) + price_change_slowdown_factor * economy::money_qnty_type(2) * state_consumption_v.array() / (state_consumption_v + state_production_v).array());
		new_state_prices = (state_prices_v.array() * new_state_prices.isNaN().select(economy::money_qnty_type(1), new_state_prices)).max(minimum_price);
	}

	void world_economy_update_tick(world_state& ws) {
		ws.w.nation_s.states.parallel_for_each([&ws](nations::state_instance& si) {
			update_state_production_and_consumption(ws, si);
		});
	}

	money_qnty_type* state_current_prices(world_state const& ws, nations::state_instance const& si) {
		return ws.w.nation_s.state_prices.get_row(si.id) + (ws.s.economy_m.aligned_32_goods_count * (1ui32 - (to_index(ws.w.current_date) & 1)));
	}

	void init_artisan_producation(world_state& ws) {
		auto max_artisan = ws.s.economy_m.artisan_types.size();
		auto& gen = get_local_generator();

		std::uniform_int_distribution<int32_t> d(0, int32_t(max_artisan - 1));
		for(auto& p : ws.w.province_s.province_state_container) {
			if(p.owner)
				p.artisan_production = ws.s.economy_m.artisan_types[artisan_type_tag(artisan_type_tag::value_base_t(d(gen)))].output_good;
		}
	}
}
