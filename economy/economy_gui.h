#pragma once
#include "common\\common.h"
#include "economy.h"

class world_state;

namespace ui {
	class gui_manager;
}

namespace economy {
	class production_window_t;

	enum class country_sub_filter : uint8_t {
		all,
		continent_asia,
		continent_africa,
		continent_oceania,
		continent_north_america,
		continent_south_america,
		continent_europe,
		neighbor,
		ally,
		enemy,
		sphere
	};

	enum class country_sort : uint8_t {
		none,
		name,
		gp_self,
		gp_one,
		gp_two,
		gp_three,
		gp_four,
		gp_five,
		gp_six,
		gp_seven,
		gp_eight,
		sphere_leader,
		prestige_rank,
		economic_rank,
		military_rank,
		overall_rank,
		opinion,
		relation,
		number_of_factories
	};

	enum class project_sort : uint8_t {
		none,
		state,
		project_type,
		completion,
		investors
	};

	class production_window {
	public:
		std::unique_ptr<production_window_t> win;

		nations::country_tag foreign_investment_nation;

		country_sub_filter filter = country_sub_filter::all;
		country_sort sort_type = country_sort::none;
		project_sort project_sort_type = project_sort::none;

		production_window();
		~production_window();

		void hide(ui::gui_manager& gui_m);
		void init(world_state& ws);
		void update(ui::gui_manager& gui_m);
		void show(ui::gui_manager& gui_m);

		void show_factories(ui::gui_manager& gui_m);
		void show_projects(ui::gui_manager& gui_m);
		void show_production(ui::gui_manager& gui_m);
		void show_foreign_investment(ui::gui_manager& gui_m);
		void show_particular_foreign_investment(ui::gui_manager& gui_m, nations::country_tag target);

	};
}