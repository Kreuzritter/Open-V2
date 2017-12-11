#include "pch.h"
#include "graphics_objects\\graphics_objects.cpp"

#define RANGE(x) (x), (x) + ((sizeof(x)) / sizeof((x)[0])) - 1

auto fake_texture_lookup() {
	return[i = 0ui16](const char*, const char*) mutable { return ++i; };
}

TEST(empty_file_with_types, graphics_objects_tests) {
	graphics::name_maps nmaps;
	graphics::object_definitions defs;
	std::vector<std::pair<std::string, graphics::errors>> errors_generated;
	auto th = fake_texture_lookup();

	parsing_environment e(nmaps, defs, errors_generated, th);
	e.file = "fake_file";

	gfx_file container(e);

	std::vector<token_group> parse_tree;
	parse_pdx_file(parse_tree, RANGE(
		"spritetypes = {}\n"
		"bitmapfonts = {}\n"
		"fonts = {}\n"
		"lighttypes = {}\n"
		"objecttypes = {}\n"
	));

	if (parse_tree.size() > 0)
		parse_object<gfx_file, gfx_file_domain>(&parse_tree[0], &parse_tree[0] + parse_tree.size(), e);

	EXPECT_EQ(0, defs.definitions.size());
	EXPECT_EQ(0, errors_generated.size());
}

TEST(unknown_filetype_error, graphics_objects_tests) {
	graphics::name_maps nmaps;
	graphics::object_definitions defs;
	std::vector<std::pair<std::string, graphics::errors>> errors_generated;
	auto th = fake_texture_lookup();

	parsing_environment e(nmaps, defs, errors_generated, th);
	e.file = "fake_file";

	gfx_file container(e);

	std::vector<token_group> parse_tree;
	parse_pdx_file(parse_tree, RANGE(
		"spritetypes = {}\n"
		"bitmapfonts = {}\n"
		"fonts = {}\n"
		"lighttypes = {}\n"
		"objecttypes = {}\n"
		"badtype = {}"
	));

	if (parse_tree.size() > 0)
		parse_object<gfx_file, gfx_file_domain>(&parse_tree[0], &parse_tree[0] + parse_tree.size(), e);

	EXPECT_EQ(0, defs.definitions.size());
	EXPECT_EQ(1, errors_generated.size());
	EXPECT_EQ(std::make_pair(std::string("fake_file"), graphics::errors::unknown_file_level_type), errors_generated[0]);
}

TEST(unknown_spritetype_error, graphics_objects_tests) {
	graphics::name_maps nmaps;
	graphics::object_definitions defs;
	std::vector<std::pair<std::string, graphics::errors>> errors_generated;
	auto th = fake_texture_lookup();

	parsing_environment e(nmaps, defs, errors_generated, th);
	e.file = "fake_file";

	gfx_file container(e);

	std::vector<token_group> parse_tree;
	parse_pdx_file(parse_tree, RANGE(
		"spritetypes = {\n"
		"badspritetype = {}\n"
		"}\n"
		"bitmapfonts = {}\n"
		"fonts = {}\n"
		"lighttypes = {}\n"
		"objecttypes = {}\n"
	));

	if (parse_tree.size() > 0)
		parse_object<gfx_file, gfx_file_domain>(&parse_tree[0], &parse_tree[0] + parse_tree.size(), e);

	EXPECT_EQ(0, defs.definitions.size());
	EXPECT_EQ(1, errors_generated.size());
	EXPECT_EQ(std::make_pair(std::string("fake_file"), graphics::errors::unknown_sprite_type), errors_generated[0]);
}


TEST(filter_unmentioned_items, graphics_objects_tests) {
	graphics::name_maps nmaps;
	graphics::object_definitions defs;
	std::vector<std::pair<std::string, graphics::errors>> errors_generated;
	auto th = fake_texture_lookup();

	parsing_environment e(nmaps, defs, errors_generated, th);
	e.file = "fake_file";

	gfx_file container(e);

	std::vector<token_group> parse_tree;
	parse_pdx_file(parse_tree, RANGE(
		"spritetypes = {\n"
		"spritetype = {}\n"
		"spritetype = {name = unknown}\n"
		"}\n"
		"bitmapfonts = {}\n"
		"fonts = {}\n"
		"lighttypes = {}\n"
		"objecttypes = {}\n"
	));

	if (parse_tree.size() > 0)
		parse_object<gfx_file, gfx_file_domain>(&parse_tree[0], &parse_tree[0] + parse_tree.size(), e);

	EXPECT_EQ(0, defs.definitions.size());
	EXPECT_EQ(0, errors_generated.size());
}

TEST(accept_mentioned_items, graphics_objects_tests) {
	graphics::name_maps nmaps;
	graphics::object_definitions defs;
	std::vector<std::pair<std::string, graphics::errors>> errors_generated;
	auto th = fake_texture_lookup();

	nmaps.names.emplace("dummy", 1);
	defs.definitions.resize(1);

	parsing_environment e(nmaps, defs, errors_generated, th);
	e.file = "fake_file";

	gfx_file container(e);

	std::vector<token_group> parse_tree;
	parse_pdx_file(parse_tree, RANGE(
		"spritetypes = {\n"
		"spritetype = {name = unknown}\n"
		"spritetype = {name = dummy}\n"
		"}\n"
		"bitmapfonts = {}\n"
		"fonts = {}\n"
		"lighttypes = {}\n"
		"objecttypes = {}\n"
	));

	if (parse_tree.size() > 0)
		parse_object<gfx_file, gfx_file_domain>(&parse_tree[0], &parse_tree[0] + parse_tree.size(), e);

	EXPECT_EQ(1, defs.definitions.size());
	EXPECT_EQ(0, errors_generated.size());
	EXPECT_EQ(0, defs.definitions[0].flags & graphics::object::always_transparent);
	EXPECT_EQ(0, defs.definitions[0].flags & graphics::object::flip_v);
	EXPECT_EQ(0, defs.definitions[0].flags & graphics::object::has_click_sound);
	EXPECT_EQ(0, defs.definitions[0].flags & graphics::object::do_transparency_check);
	EXPECT_EQ(0, defs.definitions[0].size.x);
	EXPECT_EQ(0, defs.definitions[0].size.y);
	EXPECT_EQ(0, defs.definitions[0].primary_texture_handle);
	EXPECT_EQ(0, defs.definitions[0].type_dependant);
	EXPECT_EQ(1, defs.definitions[0].number_of_frames);
	EXPECT_EQ((uint8_t)graphics::object_type::generic_sprite, defs.definitions[0].flags & graphics::object::type_mask);
}

TEST(non_default_properties, graphics_objects_tests) {
	graphics::name_maps nmaps;
	graphics::object_definitions defs;
	std::vector<std::pair<std::string, graphics::errors>> errors_generated;
	auto th = fake_texture_lookup();

	nmaps.names.emplace("dummy", 1);
	defs.definitions.resize(1);

	parsing_environment e(nmaps, defs, errors_generated, th);
	e.file = "fake_file";

	gfx_file container(e);

	std::vector<token_group> parse_tree;
	parse_pdx_file(parse_tree, RANGE(
		"spritetypes = {\n"
		"spritetype = {name = unknown}\n"
		"barcharttype = {\n"
		"name = dummy\n"
		"allwaystransparent = yes\n"
		"flipv = yes\n"
		"clicksound = click\n"
		"transparencecheck = yes\n"
		"textureFile = blah\n"
		"borderSize = {x=4 y= 4}\n"
		"size = {x = 20 y = 40}\n"
		"noOfFrames = 6\n"
		"}\n"
		"}\n"
		"bitmapfonts = {}\n"
		"fonts = {}\n"
		"lighttypes = {}\n"
		"objecttypes = {}\n"
	));

	if (parse_tree.size() > 0)
		parse_object<gfx_file, gfx_file_domain>(&parse_tree[0], &parse_tree[0] + parse_tree.size(), e);

	EXPECT_EQ(1, defs.definitions.size());
	EXPECT_EQ(0, errors_generated.size());
	EXPECT_EQ(graphics::object::always_transparent, defs.definitions[0].flags & graphics::object::always_transparent);
	EXPECT_EQ(graphics::object::flip_v, defs.definitions[0].flags & graphics::object::flip_v);
	EXPECT_EQ(graphics::object::has_click_sound, defs.definitions[0].flags & graphics::object::has_click_sound);
	EXPECT_EQ(graphics::object::do_transparency_check, defs.definitions[0].flags & graphics::object::do_transparency_check);
	EXPECT_EQ(20, defs.definitions[0].size.x);
	EXPECT_EQ(40, defs.definitions[0].size.y);
	EXPECT_EQ(1, defs.definitions[0].primary_texture_handle);
	EXPECT_EQ(4, defs.definitions[0].type_dependant);
	EXPECT_EQ(6, defs.definitions[0].number_of_frames);
	EXPECT_EQ((uint8_t)graphics::object_type::barchart, defs.definitions[0].flags & graphics::object::type_mask);
}

TEST(non_default_properties_b, graphics_objects_tests) {
	graphics::name_maps nmaps;
	graphics::object_definitions defs;
	std::vector<std::pair<std::string, graphics::errors>> errors_generated;
	auto th = fake_texture_lookup();

	nmaps.names.emplace("dummy", 1);
	defs.definitions.resize(1);

	parsing_environment e(nmaps, defs, errors_generated, th);
	e.file = "fake_file";

	gfx_file container(e);

	std::vector<token_group> parse_tree;
	parse_pdx_file(parse_tree, RANGE(
		"spritetypes = {\n"
		"spritetype = {name = unknown}\n"
		"progressbartype = {\n"
		"name = dummy\n"
		"allwaystransparent = yes\n"
		"flipv = yes\n"
		"clicksound = click\n"
		"transparencecheck = yes\n"
		"textureFile1 = blah\n"
		"textureFile2 = blah\n"
		"size = 30\n"
		"noOfFrames = 6\n"
		"horizontal = no\n"
		"}\n"
		"}\n"
		"bitmapfonts = {}\n"
		"fonts = {}\n"
		"lighttypes = {}\n"
		"objecttypes = {}\n"
	));

	if (parse_tree.size() > 0)
		parse_object<gfx_file, gfx_file_domain>(&parse_tree[0], &parse_tree[0] + parse_tree.size(), e);

	EXPECT_EQ(1, defs.definitions.size());
	EXPECT_EQ(0, errors_generated.size());
	EXPECT_EQ(graphics::object::always_transparent, defs.definitions[0].flags & graphics::object::always_transparent);
	EXPECT_EQ(graphics::object::flip_v, defs.definitions[0].flags & graphics::object::flip_v);
	EXPECT_EQ(graphics::object::has_click_sound, defs.definitions[0].flags & graphics::object::has_click_sound);
	EXPECT_EQ(graphics::object::do_transparency_check, defs.definitions[0].flags & graphics::object::do_transparency_check);
	EXPECT_EQ(30, defs.definitions[0].size.x);
	EXPECT_EQ(30, defs.definitions[0].size.y);
	EXPECT_EQ(1, defs.definitions[0].primary_texture_handle);
	EXPECT_EQ(2, defs.definitions[0].type_dependant);
	EXPECT_EQ(6, defs.definitions[0].number_of_frames);
	EXPECT_EQ((uint8_t)graphics::object_type::vertical_progress_bar, defs.definitions[0].flags & graphics::object::type_mask);
}

TEST(errors, graphics_objects_tests) {
	graphics::name_maps nmaps;
	graphics::object_definitions defs;
	std::vector<std::pair<std::string, graphics::errors>> errors_generated;
	auto th = fake_texture_lookup();

	nmaps.names.emplace("dummy", 1);
	defs.definitions.resize(1);

	parsing_environment e(nmaps, defs, errors_generated, th);
	e.file = "fake_file";

	gfx_file container(e);

	std::vector<token_group> parse_tree;
	parse_pdx_file(parse_tree, RANGE(
		"spritetypes = {\n"
		"spritetype = {name = unknown}\n"
		"progressbartype = {\n"
		"name = dummy\n"
		"allwaystransparent = yes\n"
		"flipv = yes\n"
		"clicksound = bad\n"
		"transparencecheck = yes\n"
		"textureFile1 = blah\n"
		"bordersize = {x= 10 y = 6}\n"
		"size = 30\n"
		"noOfFrames = 600\n"
		"horizontal = no\n"
		"badattribute\n"
		"}\n"
		"}\n"
		"bitmapfonts = {}\n"
		"fonts = {}\n"
		"lighttypes = {}\n"
		"objecttypes = {}\n"
	));

	if (parse_tree.size() > 0)
		parse_object<gfx_file, gfx_file_domain>(&parse_tree[0], &parse_tree[0] + parse_tree.size(), e);

	EXPECT_EQ(1, defs.definitions.size());
	EXPECT_EQ(4, errors_generated.size());

	EXPECT_EQ(std::make_pair(std::string("fake_file"), graphics::errors::invalid_click_sound), errors_generated[0]);
	EXPECT_EQ(std::make_pair(std::string("fake_file"), graphics::errors::non_square_border_size), errors_generated[1]);
	EXPECT_EQ(std::make_pair(std::string("fake_file"), graphics::errors::invalid_number_of_frames), errors_generated[2]);
	EXPECT_EQ(std::make_pair(std::string("fake_file"), graphics::errors::unknown_attribute), errors_generated[3]);

	EXPECT_EQ(graphics::object::always_transparent, defs.definitions[0].flags & graphics::object::always_transparent);
	EXPECT_EQ(graphics::object::flip_v, defs.definitions[0].flags & graphics::object::flip_v);
	EXPECT_EQ(0, defs.definitions[0].flags & graphics::object::has_click_sound);
	EXPECT_EQ(graphics::object::do_transparency_check, defs.definitions[0].flags & graphics::object::do_transparency_check);
	EXPECT_EQ(30, defs.definitions[0].size.x);
	EXPECT_EQ(30, defs.definitions[0].size.y);
	EXPECT_EQ(1, defs.definitions[0].primary_texture_handle);
	EXPECT_EQ(0, defs.definitions[0].type_dependant);
	EXPECT_EQ(1, defs.definitions[0].number_of_frames);
	EXPECT_EQ((uint8_t)graphics::object_type::vertical_progress_bar, defs.definitions[0].flags & graphics::object::type_mask);
}