#pragma once
#include "common\\common.h"
#include "issues.h"

class world_state;

namespace issues {
	template<typename value_type>
	float calculate_political_interest(world_state const& ws, value_type* demographics_array);
	template<typename value_type>
	float calculate_social_interest(world_state const& ws, value_type* demographics_array);

	void reset_active_issues(world_state& ws, nations::country_tag nation_for);
}