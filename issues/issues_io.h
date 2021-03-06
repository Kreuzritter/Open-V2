#pragma once
#include "common\\common.h"
#include "issues.h"
#include "simple_serialize\\simple_serialize.hpp"
#include "Parsers\\parsers.hpp"
#include "text_data\\text_data.h"
#include <ppl.h>

template<>
class serialization::serializer<issues::issue_option> : public serialization::memcpy_serializer<issues::issue_option> {};
template<>
class serialization::serializer<issues::issue> : public serialization::memcpy_serializer<issues::issue> {};

template<>
class serialization::serializer<issues::issues_manager> {
public:
	static constexpr bool has_static_size = false;
	static constexpr bool has_simple_serialize = false;

	static void rebuild_indexes(issues::issues_manager& obj) {
		for(auto const& i_issue : obj.issues_container) {
			obj.named_issue_index.emplace(i_issue.name, i_issue.id);
			if(i_issue.type == issues::issue_group::party)
				obj.party_issues.push_back(i_issue.id);
			else if(i_issue.type == issues::issue_group::economic)
				obj.economic_issues.push_back(i_issue.id);
			else if(i_issue.type == issues::issue_group::military)
				obj.military_issues.push_back(i_issue.id);
			else if(i_issue.type == issues::issue_group::political)
				obj.political_issues.push_back(i_issue.id);
			else if(i_issue.type == issues::issue_group::social)
				obj.social_issues.push_back(i_issue.id);
		}

		obj.party_issues_options_count = 0ui32;
		obj.political_issues_options_count = 0ui32;
		obj.social_issues_options_count = 0ui32;

		for(auto& i_option : obj.options) {
			obj.named_option_index.emplace(i_option.name, i_option.id);
			if(obj.issues_container[i_option.parent_issue].type == issues::issue_group::party)
				++obj.party_issues_options_count;
			else if(obj.issues_container[i_option.parent_issue].type == issues::issue_group::political)
				++obj.political_issues_options_count;
			else if(obj.issues_container[i_option.parent_issue].type == issues::issue_group::social)
				++obj.social_issues_options_count;

			i_option.type = obj.issues_container[i_option.parent_issue].type;
		}

		obj.tracked_options_count = obj.party_issues_options_count + obj.political_issues_options_count + obj.social_issues_options_count;
		obj.options_count = uint32_t(obj.options.size());
	}

	static void serialize_object(std::byte* &output, issues::issues_manager const& obj) {
		serialize(output, obj.issues_container);
		serialize(output, obj.options);
		serialize(output, obj.jingoism);
	}
	static void deserialize_object(std::byte const* &input, issues::issues_manager& obj) {
		deserialize(input, obj.issues_container);
		deserialize(input, obj.options);
		deserialize(input, obj.jingoism);

		rebuild_indexes(obj);
	}
	static void deserialize_object(std::byte const* &input, issues::issues_manager& obj, concurrency::task_group& tg) {
		deserialize(input, obj.issues_container);
		deserialize(input, obj.options);
		deserialize(input, obj.jingoism);

		tg.run([&obj]() { rebuild_indexes(obj); });
	}
	static size_t size(issues::issues_manager const& obj) {
		return serialize_size(obj.issues_container) +
			serialize_size(obj.options) +
			serialize_size(obj.jingoism);
	}
};

namespace issues {
	struct parsing_environment;

	class parsing_state {
	public:
		std::unique_ptr<parsing_environment> impl;

		parsing_state(text_data::text_sequences& tl, issues_manager& m);
		parsing_state(parsing_state&&) = default;
		~parsing_state();
	};

	parsing_state pre_parse_issues(
		issues_manager& manager,
		const directory& source_directory,
		text_data::text_sequences& text_function);

	rules_set read_rules(const token_group* start, const token_group* end);
	void read_issue_options(
		const parsing_state& ps,
		scenario::scenario_manager& s,
		events::event_creation_manager& ecm);
}
