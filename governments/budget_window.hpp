#pragma once
#include "common\\common.h"
#include "budget_window.h"
#include "gui\\gui.hpp"
#include "governments_functions.h"
#include "economy\\economy_functions.h"
#include "world_state\\world_state.h"

namespace governments {
	class budget_window_base : public ui::draggable_region {
	public:
		template<typename window_type>
		void on_create(window_type& win, world_state& ws);
	};

	class bw_close_button {
	public:
		void button_function(ui::simple_button<bw_close_button>&, world_state& ws);
	};

	class tarrif_percent {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};

	class poor_tax_collected {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class middle_tax_collected {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class rich_tax_collected {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};

	class gold_income {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};

	class total_income {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};

	class total_funds {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};

	class debt_total {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class debt_label {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};

	class fixed_incomes_label {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class variable_expenses_label {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class fixed_incomes_funds {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class variable_expenses_funds {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class hidden_button {
	public:
		void on_create(ui::simple_button<hidden_button>& b, world_state& ws);
	};
	class hidden_text {
	public:
		void on_create(ui::fixed_text<hidden_text>& b, world_state& ws);
	};
	class stockpile_cost {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class industrial_subsidies_cost {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class education_pay {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class administrative_pay {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class military_pay {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class social_spending_cost {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class national_administrative_efficiency {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class expenses_total {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class debt_and_saving_label {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class debt_and_saving_amount {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class tarrif_amount {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};
	class budget_total_amount {
	public:
		template<typename W>
		void windowed_update(W& w, ui::tagged_gui_object box, ui::text_box_line_manager& lm, ui::text_format& fmt, world_state& ws);
	};

	class poor_tax_scrollbar {
	public:
		void on_position(world_state& ws, ui::scrollbar<poor_tax_scrollbar>& sb, int32_t pos);
		void update(ui::scrollbar<poor_tax_scrollbar>& sb, world_state& ws);
	};
	class middle_tax_scrollbar {
	public:
		void on_position(world_state& ws, ui::scrollbar<middle_tax_scrollbar>& sb, int32_t pos);
		void update(ui::scrollbar<middle_tax_scrollbar>& sb, world_state& ws);
	};
	class rich_tax_scrollbar {
	public:
		void on_position(world_state& ws, ui::scrollbar<rich_tax_scrollbar>& sb, int32_t pos);
		void update(ui::scrollbar<rich_tax_scrollbar>& sb, world_state& ws);
	};

	class land_spending_scrollbar {
	public:
		void on_position(world_state& ws, ui::scrollbar<land_spending_scrollbar>& sb, int32_t pos);
		void update(ui::scrollbar<land_spending_scrollbar>& sb, world_state& ws);
	};
	class naval_spending_scrollbar {
	public:
		void on_position(world_state& ws, ui::scrollbar<naval_spending_scrollbar>& sb, int32_t pos);
		void update(ui::scrollbar<naval_spending_scrollbar>& sb, world_state& ws);
	};
	class projects_spending_scrollbar {
	public:
		void on_position(world_state& ws, ui::scrollbar<projects_spending_scrollbar>& sb, int32_t pos);
		void update(ui::scrollbar<projects_spending_scrollbar>& sb, world_state& ws);
	};

	class administrative_pay_scrollbar {
	public:
		void on_position(world_state& ws, ui::scrollbar<administrative_pay_scrollbar>& sb, int32_t pos);
		void update(ui::scrollbar<administrative_pay_scrollbar>& sb, world_state& ws);
	};
	class education_pay_scrollbar {
	public:
		void on_position(world_state& ws, ui::scrollbar<education_pay_scrollbar>& sb, int32_t pos);
		void update(ui::scrollbar<education_pay_scrollbar>& sb, world_state& ws);
	};
	class military_pay_scrollbar {
	public:
		void on_position(world_state& ws, ui::scrollbar<military_pay_scrollbar>& sb, int32_t pos);
		void update(ui::scrollbar<military_pay_scrollbar>& sb, world_state& ws);
	};
	class social_spending_scrollbar {
	public:
		void on_position(world_state& ws, ui::scrollbar<social_spending_scrollbar>& sb, int32_t pos);
		void update(ui::scrollbar<social_spending_scrollbar>& sb, world_state& ws);
	};

	class tarrif_setting_scrollbar {
	public:
		void on_position(world_state& ws, ui::scrollbar<tarrif_setting_scrollbar>& sb, int32_t pos);
		void update(ui::scrollbar<tarrif_setting_scrollbar>& sb, world_state& ws);
	};

	class poor_needs_pie_chart {
	public:
		void update(ui::piechart<poor_needs_pie_chart>& pie, world_state& ws);
	};

	class middle_needs_pie_chart {
	public:
		void update(ui::piechart<middle_needs_pie_chart>& pie, world_state& ws);
	};

	class rich_needs_pie_chart {
	public:
		void update(ui::piechart<rich_needs_pie_chart>& pie, world_state& ws);
	};

	class pop_button {
	public:
		population::pop_type_tag type;

		pop_button(population::pop_type_tag t) : type(t) {}
		void update(ui::simple_button<pop_button>& ico, world_state& ws);
		bool has_tooltip(world_state&) { return true; }
		void create_tooltip(world_state& ws, ui::tagged_gui_object tw);
	};

	class pop_item_base : public ui::visible_region {
	public:
		pop_item_base(population::pop_type_tag) {}

		template<typename window_type>
		void on_create(window_type& win, world_state& ws);
	};

	using pop_item_t = ui::gui_window<
		CT_STRING("pop"), ui::simple_button<pop_button>,
		pop_item_base
	>;

	class poor_pops_lb {
	public:
		template<typename lb_type>
		void populate_list(lb_type& lb, world_state& ws);
		ui::window_tag element_tag(ui::gui_static& m);
	};

	class middle_pops_lb {
	public:
		template<typename lb_type>
		void populate_list(lb_type& lb, world_state& ws);
		ui::window_tag element_tag(ui::gui_static& m);
	};

	class rich_pops_lb {
	public:
		template<typename lb_type>
		void populate_list(lb_type& lb, world_state& ws);
		ui::window_tag element_tag(ui::gui_static& m);
	};

	class education_pops_lb {
	public:
		template<typename lb_type>
		void populate_list(lb_type& lb, world_state& ws);
		ui::window_tag element_tag(ui::gui_static& m);
	};

	class administation_pops_lb {
	public:
		template<typename lb_type>
		void populate_list(lb_type& lb, world_state& ws);
		ui::window_tag element_tag(ui::gui_static& m);
	};

	class military_pops_lb {
	public:
		template<typename lb_type>
		void populate_list(lb_type& lb, world_state& ws);
		ui::window_tag element_tag(ui::gui_static& m);
	};

	class budget_window_t : public ui::gui_window<
		CT_STRING("close_button"), ui::simple_button<bw_close_button>,
		CT_STRING("tariffs_percent"), ui::display_text<tarrif_percent>,
		CT_STRING("diplomatic_desc"), ui::display_text<debt_and_saving_label, -10>,
		CT_STRING("tax_0_inc"), ui::display_text<poor_tax_collected>,
		CT_STRING("tax_1_inc"), ui::display_text<middle_tax_collected>,
		CT_STRING("tax_2_inc"), ui::display_text<rich_tax_collected>,
		CT_STRING("gold_inc"), ui::display_text<gold_income>,
		CT_STRING("total_inc"), ui::display_text<total_income>,
		CT_STRING("total_funds_val"), ui::display_text<total_funds, -12>,
		CT_STRING("national_bank_desc"), ui::display_text<debt_label, -12>,
		CT_STRING("national_bank_val"), ui::display_text<debt_total, -12>,
		CT_STRING("debt_desc"), ui::display_text<fixed_incomes_label, -10>,
		CT_STRING("interest_desc"), ui::display_text<variable_expenses_label, -10>,
		CT_STRING("debt_val"), ui::display_text<fixed_incomes_funds, -10>,
		CT_STRING("interest_val"), ui::display_text<variable_expenses_funds, -10>,
		CT_STRING("tab_takenloans"), ui::simple_button<hidden_button>,
		CT_STRING("tab_givenloans"), ui::simple_button<hidden_button>,
		CT_STRING("takenloans_text"), ui::fixed_text<hidden_text>,
		CT_STRING("givenloans_text"), ui::fixed_text<hidden_text>,
		CT_STRING("debt_sort_country"), ui::simple_button<hidden_button>,
		CT_STRING("debt_sort_amount"), ui::simple_button<hidden_button>,
		CT_STRING("nat_stock_est"), ui::display_text<stockpile_cost>,
		CT_STRING("nat_stock_val"), ui::fixed_text<hidden_text>,
		CT_STRING("mil_cost_desc"), ui::fixed_text<hidden_text>,
		CT_STRING("mil_cost_val"), ui::fixed_text<hidden_text>,
		CT_STRING("overseas_desc"), ui::fixed_text<hidden_text>,
		CT_STRING("overseas_cost_val"), ui::fixed_text<hidden_text>,
		CT_STRING("ind_sub_val"), ui::display_text<industrial_subsidies_cost>,
		CT_STRING("admin_efficiency"), ui::display_text<national_administrative_efficiency>,
		CT_STRING("exp_val_0"), ui::display_text<education_pay>,
		CT_STRING("exp_val_1"), ui::display_text<administrative_pay>,
		CT_STRING("exp_val_2"), ui::display_text<social_spending_cost>,
		CT_STRING("exp_val_3"), ui::display_text<military_pay>,
		CT_STRING("total_exp"), ui::display_text<expenses_total>,
		CT_STRING("tariff_val"), ui::display_text<tarrif_amount>,
		CT_STRING("diplomatic_balance"), ui::display_text<debt_and_saving_amount>,
		CT_STRING("balance"), ui::display_text<budget_total_amount>,
		CT_STRING("tax_0_slider"), ui::scrollbar<poor_tax_scrollbar>,
		CT_STRING("tax_1_slider"), ui::scrollbar<middle_tax_scrollbar>,
		CT_STRING("tax_2_slider"), ui::scrollbar<rich_tax_scrollbar>,
		CT_STRING("land_stockpile_slider"), ui::scrollbar<land_spending_scrollbar>,
		CT_STRING("naval_stockpile_slider"), ui::scrollbar<naval_spending_scrollbar>,
		CT_STRING("projects_stockpile_slider"), ui::scrollbar<projects_spending_scrollbar>,
		CT_STRING("exp_0_slider"), ui::scrollbar<education_pay_scrollbar>,
		CT_STRING("exp_1_slider"), ui::scrollbar<administrative_pay_scrollbar>,
		CT_STRING("exp_2_slider"), ui::scrollbar<social_spending_scrollbar>,
		CT_STRING("exp_3_slider"), ui::scrollbar<military_pay_scrollbar>,
		CT_STRING("tariff_slider"), ui::scrollbar<tarrif_setting_scrollbar>,
		CT_STRING("take_loan"), ui::simple_button<hidden_button>,
		CT_STRING("repay_loan"), ui::simple_button<hidden_button>,
		CT_STRING("chart_0"), ui::piechart<poor_needs_pie_chart>,
		CT_STRING("chart_1"), ui::piechart<middle_needs_pie_chart>,
		CT_STRING("chart_2"), ui::piechart<rich_needs_pie_chart>,
		CT_STRING("tax_0_pops"), ui::overlap_box<poor_pops_lb, ui::window_tag, pop_item_t, 32>,
		CT_STRING("tax_1_pops"), ui::overlap_box<middle_pops_lb, ui::window_tag, pop_item_t, 32>,
		CT_STRING("tax_2_pops"), ui::overlap_box<rich_pops_lb, ui::window_tag, pop_item_t, 32>,
		CT_STRING("exp_0_pops"), ui::overlap_box<education_pops_lb, ui::window_tag, pop_item_t, 1>,
		CT_STRING("exp_1_pops"), ui::overlap_box<administation_pops_lb, ui::window_tag, pop_item_t, 1>,
		CT_STRING("exp_3_pops"), ui::overlap_box<military_pops_lb, ui::window_tag, pop_item_t, 1>,
		budget_window_base
	> {};

	template<typename window_type>
	void budget_window_base::on_create(window_type & win, world_state & ws) {
		associated_object->size = ui::xy_pair{ 1017i16, 636i16 };
		ui::for_each_child(ws.w.gui_m, ui::tagged_gui_object{ *associated_object, ui::gui_object_tag() }, [](ui::tagged_gui_object obj) {
			obj.object.position += ui::xy_pair{ -3i16, 3i16 };
		});
	}
	template<typename W>
	void tarrif_percent::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		if(auto player = ws.w.local_player_nation; player) {
			char16_t local_buffer[16];
			put_value_in_buffer(local_buffer, display_type::percent, float(player->tarrifs) / 100.0f);
			ui::text_chunk_to_instances(ws.s.gui_m, ws.w.gui_m, vector_backed_string<char16_t>(local_buffer), box, ui::xy_pair{ 0,0 }, fmt, lm);
			lm.finish_current_line();
		}
	}
	template<typename W>
	void poor_tax_collected::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		if(auto player = ws.w.local_player_nation; player) {
			char16_t local_buffer[16];
			put_value_in_buffer(local_buffer, display_type::currency, ws.w.local_player_data.collected_poor_tax * float(player->poor_tax) / 100.0f);
			ui::text_chunk_to_instances(ws.s.gui_m, ws.w.gui_m, vector_backed_string<char16_t>(local_buffer), box, ui::xy_pair{ 0,0 }, fmt, lm);
			lm.finish_current_line();
		}
	}
	template<typename W>
	void middle_tax_collected::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		if(auto player = ws.w.local_player_nation; player) {
			char16_t local_buffer[16];
			put_value_in_buffer(local_buffer, display_type::currency, ws.w.local_player_data.collected_middle_tax * float(player->middle_tax) / 100.0f);
			ui::text_chunk_to_instances(ws.s.gui_m, ws.w.gui_m, vector_backed_string<char16_t>(local_buffer), box, ui::xy_pair{ 0,0 }, fmt, lm);
			lm.finish_current_line();
		}
	}
	template<typename W>
	void rich_tax_collected::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		if(auto player = ws.w.local_player_nation; player) {
			char16_t local_buffer[16];
			put_value_in_buffer(local_buffer, display_type::currency, ws.w.local_player_data.collected_rich_tax * float(player->rich_tax) / 100.0f);
			ui::text_chunk_to_instances(ws.s.gui_m, ws.w.gui_m, vector_backed_string<char16_t>(local_buffer), box, ui::xy_pair{ 0,0 }, fmt, lm);
			lm.finish_current_line();
		}
	}
	template<typename W>
	void gold_income::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {}

	template<typename W>
	void total_income::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		if(auto player = ws.w.local_player_nation; player) {
			if(auto pid = player->id; ws.w.nation_s.nations.is_valid_index(pid)) {
				auto tax_income = ws.w.local_player_data.collected_poor_tax * float(player->poor_tax) / 100.0f +
					ws.w.local_player_data.collected_middle_tax * float(player->middle_tax) / 100.0f +
					ws.w.local_player_data.collected_rich_tax * float(player->rich_tax) / 100.0f;
				economy::money_qnty_type tarrif_income = 0;
				for(uint32_t i = 1; i < ws.s.economy_m.goods_count; ++i) {
					tarrif_income += ws.w.nation_s.collected_tarrifs.get(pid, economy::goods_tag(economy::goods_tag::value_base_t(i)));
				}

				char16_t local_buffer[16];
				put_value_in_buffer(local_buffer, display_type::currency, tax_income + tarrif_income);
				ui::text_chunk_to_instances(ws.s.gui_m, ws.w.gui_m, vector_backed_string<char16_t>(local_buffer), box, ui::xy_pair{ 0,0 }, fmt, lm);
				lm.finish_current_line();
			}
		}
	}
	template<typename W>
	void total_funds::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		if(auto player = ws.w.local_player_nation; player) {
			if(auto pid = player->id; ws.w.nation_s.nations.is_valid_index(pid)) {
				char16_t local_buffer[16];
				put_value_in_buffer(local_buffer, display_type::currency, ws.w.nation_s.national_stockpiles.get(pid, economy::money_good));
				ui::text_chunk_to_instances(ws.s.gui_m, ws.w.gui_m, vector_backed_string<char16_t>(local_buffer), box, ui::xy_pair{ 0,0 }, fmt, lm);
				lm.finish_current_line();
			}
		}
	}
	template<typename W>
	void debt_total::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		if(auto player = ws.w.local_player_nation; player) {
			char16_t local_buffer[16];
			put_value_in_buffer(local_buffer, display_type::currency, player->national_debt);
			ui::text_chunk_to_instances(ws.s.gui_m, ws.w.gui_m, vector_backed_string<char16_t>(local_buffer), box, ui::xy_pair{ 0,0 }, fmt, lm);
			lm.finish_current_line();
		}
	}
	template<typename W>
	void debt_label::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		ui::add_linear_text(ui::xy_pair{ 0,0 }, ws.s.fixed_ui_text[scenario::fixed_ui::total_debt], fmt, ws.s.gui_m, ws.w.gui_m, box, lm);
		lm.finish_current_line();
	}
	template<typename W>
	void fixed_incomes_label::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		ui::add_linear_text(ui::xy_pair{ 0,0 }, ws.s.fixed_ui_text[scenario::fixed_ui::fixed_income], fmt, ws.s.gui_m, ws.w.gui_m, box, lm);
		lm.finish_current_line();
	}
	template<typename W>
	void variable_expenses_label::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		ui::add_linear_text(ui::xy_pair{ 0,0 }, ws.s.fixed_ui_text[scenario::fixed_ui::variable_expenses], fmt, ws.s.gui_m, ws.w.gui_m, box, lm);
		lm.finish_current_line();
	}
	template<typename W>
	void fixed_incomes_funds::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {}
	
	template<typename W>
	void variable_expenses_funds::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {}
	
	template<typename W>
	void stockpile_cost::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {}
	template<typename W>
	void industrial_subsidies_cost::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {}
	template<typename W>
	void education_pay::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {}
	template<typename W>
	void administrative_pay::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {}
	template<typename W>
	void military_pay::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {}
	template<typename W>
	void social_spending_cost::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {}
	template<typename W>
	void national_administrative_efficiency::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {}
	template<typename W>
	void expenses_total::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {}
	
	template<typename W>
	void debt_and_saving_label::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		ui::add_linear_text(ui::xy_pair{ 0,0 }, ws.s.fixed_ui_text[scenario::fixed_ui::borrowing_and_saving], fmt, ws.s.gui_m, ws.w.gui_m, box, lm);
		lm.finish_current_line();
	}
	template<typename W>
	void debt_and_saving_amount::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {}
	template<typename W>
	void tarrif_amount::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		if(auto player = ws.w.local_player_nation; player) {
			if(auto pid = player->id; ws.w.nation_s.nations.is_valid_index(pid)) {
				economy::money_qnty_type tarrif_income = 0;
				for(uint32_t i = 1; i < ws.s.economy_m.goods_count; ++i) {
					tarrif_income += ws.w.nation_s.collected_tarrifs.get(pid, economy::goods_tag(economy::goods_tag::value_base_t(i)));
				}

				char16_t local_buffer[16];
				put_value_in_buffer(local_buffer, display_type::currency, tarrif_income);
				ui::text_chunk_to_instances(ws.s.gui_m, ws.w.gui_m, vector_backed_string<char16_t>(local_buffer), box, ui::xy_pair{ 0,0 }, fmt, lm);
				lm.finish_current_line();
			}
		}
	}
	template<typename W>
	void budget_total_amount::windowed_update(W & w, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {}
	
	template<typename lb_type>
	void poor_pops_lb::populate_list(lb_type & lb, world_state & ws) {
		for(auto& pt : ws.s.population_m.pop_types) {
			if((pt.flags & population::pop_type::strata_mask) == population::pop_type::strata_poor) {
				lb.add_item(ws, pt.id);
			}
		}
	}

	template<typename lb_type>
	void middle_pops_lb::populate_list(lb_type & lb, world_state & ws) {
		for(auto& pt : ws.s.population_m.pop_types) {
			if((pt.flags & population::pop_type::strata_mask) == population::pop_type::strata_middle) {
				lb.add_item(ws, pt.id);
			}
		}
	}

	template<typename lb_type>
	void rich_pops_lb::populate_list(lb_type & lb, world_state & ws) {
		for(auto& pt : ws.s.population_m.pop_types) {
			if((pt.flags & population::pop_type::strata_mask) == population::pop_type::strata_rich) {
				lb.add_item(ws, pt.id);
			}
		}
	}
	template<typename window_type>
	void pop_item_base::on_create(window_type & win, world_state & ws) {
		associated_object->size.y = 64i16;
	}
	template<typename lb_type>
	void education_pops_lb::populate_list(lb_type & lb, world_state & ws) {
		lb.add_item(ws, ws.s.population_m.clergy);
	}
	template<typename lb_type>
	void administation_pops_lb::populate_list(lb_type & lb, world_state & ws) {
		lb.add_item(ws, ws.s.population_m.bureaucrat);
	}
	template<typename lb_type>
	void military_pops_lb::populate_list(lb_type & lb, world_state & ws) {
		lb.add_item(ws, ws.s.population_m.soldier);
		lb.add_item(ws, ws.s.population_m.officer);
	}
}