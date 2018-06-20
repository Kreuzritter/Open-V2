#include "scenario_io.h"

namespace scenario {
	void read_scenario(scenario_manager& s, const directory& root) {
		//stage 1
		ui::load_gui_from_directory(root, s.gui_m);

		//stage 2
		auto const country_files = cultures::read_national_tags(s.culutre_m, root);

		cultures::read_religions(s.culutre_m, root, s.gui_m.text_data_sequences);
		cultures::read_cultures(s.culutre_m, s.gui_m.textures, root, s.gui_m.text_data_sequences);

		economy::read_goods(s.economy_m, root, s.gui_m.text_data_sequences);
		auto building_production_map = economy::read_buildings(s.economy_m, root, s.gui_m.text_data_sequences);

		auto const ideology_state = ideologies::pre_parse_ideologies(s.ideologies_m, root, s.gui_m.text_data_sequences);

		auto const issues_state = issues::pre_parse_issues(s.issues_m, root, s.gui_m.text_data_sequences);

		auto const government_names = governments::read_governments(s.governments_m, root, s.gui_m.text_data_sequences, s.ideologies_m);

		military::parsing_state military_state(s.gui_m.text_data_sequences, s.military_m);
		military::pre_parse_unit_types(military_state, root);
		military::pre_parse_cb_types(military_state, root);
		military::read_leader_traits(military_state, root);

		modifiers::parsing_state modifiers_state(s.gui_m.text_data_sequences, s.modifiers_m);
		modifiers::read_defines(s.modifiers_m, root);
		modifiers::pre_parse_crimes(modifiers_state, root);
		modifiers::pre_parse_triggered_modifiers(modifiers_state, root);
		modifiers::read_national_values(modifiers_state, root);
		modifiers::read_static_modifiers(modifiers_state, root);
		modifiers::read_event_modifiers(modifiers_state, root);

		population::parsing_state pop_state(s.gui_m.text_data_sequences, s.population_m);
		population::pre_parse_pop_types(s.population_m, root, s.gui_m.text_data_sequences);
		population::pre_parse_rebel_types(pop_state, root);

		provinces::parsing_state province_state(s.gui_m.text_data_sequences, s.province_m, s.modifiers_m);
		provinces::read_default_map_file(province_state, root);

		auto const color_to_terrain = provinces::pre_parse_terrain(province_state, root);
		provinces::read_states(province_state, root);
		provinces::read_continents(province_state, root);
		provinces::read_climates(province_state, root);

		sound::populate_music(s.sound_m, root);
		sound::read_effects(s.sound_m, s.gui_m.text_data_sequences, root);

		technologies::parsing_state tech_state(s.gui_m.text_data_sequences, root.get_directory(u"\\technologies"), s.technology_m, s.modifiers_m);
		technologies::pre_parse_technologies(tech_state, root);
		technologies::pre_parse_inventions(tech_state, root);

		// stage 3

		events::event_creation_manager ecm;

		governments::ready_party_issues(s.governments_m, s.issues_m);

		cultures::read_country_files(country_files, s, root);
		cultures::read_flag_graphics(s, root);
		cultures::populate_country_names(s, government_names);

		economy::read_production_types(s, building_production_map, root);

		ideologies::read_ideologies(s, ideology_state);

		issues::read_issue_options(issues_state, s, ecm);

		military::read_unit_types(military_state, s.military_m, s.economy_m, s.sound_m, s.gui_m.text_data_sequences);
		military::read_cb_types(military_state, s, ecm);

		modifiers::read_crimes(modifiers_state, s);
		modifiers::read_triggered_modifiers(modifiers_state, s);
		modifiers::read_national_focuses(s, root);

		population::read_main_poptype_file(s, root);
		population::read_poptypes(s, root);
		population::read_rebel_types(pop_state, s, ecm);
		population::populate_demote_to(s.population_m);

		events::read_on_actions_file(s, ecm, root);
		events::read_event_files(s, ecm, root);
		events::read_decision_files(s, ecm, root);

		technologies::prepare_technologies_read(s);
		technologies::read_inventions(tech_state, s);
		technologies::read_technologies(tech_state, s);

		// stage 4

		commit_pending_triggered_events(s, ecm, root);

		//return color_to_terrain;
	}
}