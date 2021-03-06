﻿#include "graphics\\test_helpers.h"
#include "fake_fs\\fake_fs.h"
#include "gtest\\gtest.h"
#include <mutex>

using namespace graphics;

#define RANGE(x) (x), (x) + (sizeof((x))/sizeof((x)[0])) - 1

static std::mutex force_sequential;
TEST(graphics_tests, texture_rendering) {
	std::lock_guard l(force_sequential);

	EXPECT_TRUE(test_rendering("D:\\VS2007Projects\\open_v2_test_data\\texture", 0, 0, 80, 80, [](open_gl_wrapper& ogl) {
		texture test_tex("D:\\VS2007Projects\\open_v2_test_data\\test_tx.bmp");

		ogl.render_textured_rect(graphics::color_modification::none, 0.0f, 0.0f, 80.0f, 40.0f, test_tex);
		ogl.render_textured_rect(graphics::color_modification::disabled, 0.0f, 40.0f, 80.0f, 40.0f, test_tex);
	}));
}

TEST(graphics_tests, dds_texture_rendering) {
	std::lock_guard l(force_sequential);

	EXPECT_TRUE(test_rendering("D:\\VS2007Projects\\open_v2_test_data\\dds", 0, 0, 48, 48, [](open_gl_wrapper& ogl) {
		texture test_tex("D:\\VS2007Projects\\open_v2_test_data\\army_icon_2.dds");

		ogl.render_textured_rect(graphics::color_modification::none, 0.0f, 0.0f, 48.0f, 24.0f, test_tex);
		ogl.render_textured_rect(graphics::color_modification::disabled, 0.0f, 24.0f, 48.0f, 24.0f, test_tex);
	}));
}

TEST(graphics_tests, text_rendering) {
	std::lock_guard l(force_sequential);

	EXPECT_TRUE(test_rendering("D:\\VS2007Projects\\open_v2_test_data\\text", 0, 0, 128, 210, [](open_gl_wrapper& ogl) {
		
		font test_fallback("D:\\VS2007Projects\\open_v2_test_data\\unifont-9.0.02.ttf");
		font test_font("D:\\VS2007Projects\\open_v2_test_data\\CreteRound-Regular.otf", test_fallback);

		test_fallback.load_font(ogl);
		test_font.load_font(ogl);
		
		ogl.render_outlined_text(u"\u660ETest", 5, graphics::color_modification::none, 0.0f, 0.0f,                                 16.0f, color{ 0.0f,0.0f,0.0f }, test_font);
		ogl.render_text(         u"\u660ETest", 5, graphics::color_modification::none, 0.0f, test_font.line_height(16.0f),         16.0f, color{ 1.0f,1.0f,1.0f }, test_font);
		ogl.render_outlined_text(u"\u660ETest", 5, graphics::color_modification::disabled, 0.0f, 2.0f * test_font.line_height(16.0f), 16.0f, color{ 0.0f,0.0f,0.0f }, test_font);
		ogl.render_text(         u"\u660ETest", 5, graphics::color_modification::disabled, 0.0f, 3.0f * test_font.line_height(16.0f), 16.0f, color{ 1.0f,1.0f,1.0f }, test_font);
		ogl.render_text(u"\u660ETest", 5, graphics::color_modification::none, 0.0f, 4.0f * test_font.line_height(16.0f), 16.0f, color{ 1.0f,0.0f,0.0f }, test_font);
		ogl.render_text(         u"Test",   4, graphics::color_modification::none, 0.0f, 4.0f * test_font.line_height(16.0f), 128.0f, color{ 0.0f,1.0f,1.0f }, test_font);
	}));
}

TEST(graphics_tests, progress_bar) {
	std::lock_guard l(force_sequential);

	EXPECT_TRUE(test_rendering("D:\\VS2007Projects\\open_v2_test_data\\t_progress", 0, 0, 80, 20, [](open_gl_wrapper& ogl) {
		texture prog1("D:\\VS2007Projects\\open_v2_test_data\\progress1.tga");
		texture prog2("D:\\VS2007Projects\\open_v2_test_data\\progress2.tga");

		ogl.render_progress_bar(graphics::color_modification::none, 0.33f, 0.5f, 0.5f, 80.0f, 20.0f, prog1, prog2);
	}));
}

TEST(graphics_tests, piechart) {
	std::lock_guard l(force_sequential);

	EXPECT_TRUE(test_rendering("D:\\VS2007Projects\\open_v2_test_data\\t_piechart", 0, 0, 50, 50, [](open_gl_wrapper& ogl) {

		data_texture dt(100, 3);
		const auto ptr = dt.data();
		for (int i = 0; i < 100; ++i) {
			if (i < 25) {
				ptr[i * 3] = 255; ptr[i * 3 + 1] = 0; ptr[i * 3 + 2] = 0;
			} else if (i < 50) {
				ptr[i * 3] = 0; ptr[i * 3 + 1] = 255; ptr[i * 3 + 2] = 0;
			} else {
				ptr[i * 3] = 0; ptr[i * 3 + 1] = 0; ptr[i * 3 + 2] = 255;
			}
		}
		dt.data_ready();

		ogl.render_piechart(graphics::color_modification::none, 0.0f, 0.0f, 50.0f, dt);
	}));
}

TEST(graphics_tests, barchart) {
	std::lock_guard l(force_sequential);

	EXPECT_TRUE(test_rendering("D:\\VS2007Projects\\open_v2_test_data\\t_barchart", 0, 0, 100, 75, [](open_gl_wrapper& ogl) {
		data_texture dt(100, 4);
		const auto ptr = dt.data();
		for (int i = 0; i < 100; ++i) {
			if (i < 25) {
				ptr[i * 4] = 255; ptr[i * 4 + 1] = 0; ptr[i * 4 + 2] = 0; ptr[i * 4 + 3] = 210;
			} else if (i < 50) {
				ptr[i * 4] = 0; ptr[i * 4 + 1] = 255; ptr[i * 4 + 2] = 0; ptr[i * 4 + 3] = 128;
			} else {
				ptr[i * 4] = 0; ptr[i * 4 + 1] = 0; ptr[i * 4 + 2] = 255; ptr[i * 4 + 3] = 64;
			}
		}
		dt.data_ready();

		ogl.render_barchart(graphics::color_modification::none, 0.0f, 0.0f, 100.0f, 75.0f, dt);
	}));
}

TEST(graphics_tests, linegraph) {
	std::lock_guard l(force_sequential);

	EXPECT_TRUE(test_rendering("D:\\VS2007Projects\\open_v2_test_data\\t_linegraph", 0, 0, 100, 75, [](open_gl_wrapper& ogl) {
		lines graph(10);
		float yval[] = { 0.3f, 0.6f, 0.5f, 1.0f, 0.4f, 0.5f, 0.0f, 0.3f, 0.2f, 0.6f };
		graph.set_y(yval);
		ogl.render_linegraph(graphics::color_modification::none, 0.0f, 0.0f, 100.0f, 75.0f, graph);
	}));
}

TEST(graphics_tests, rotated_mask) {
	std::lock_guard l(force_sequential);

	EXPECT_TRUE(test_rendering("D:\\VS2007Projects\\open_v2_test_data\\t_rotated_mask", 0, 0, 40, 80, [](open_gl_wrapper& ogl) {
		texture test_tex("D:\\VS2007Projects\\open_v2_test_data\\test_tx.bmp");
		texture mask_tex("D:\\VS2007Projects\\open_v2_test_data\\mask.tga");

		ogl.render_masked_rect(graphics::color_modification::none, 0.0f, 0.0f, 40.0f, 80.0f, test_tex, mask_tex, rotation::right);
	}));
}

TEST(graphics_tests, sprite_fragment) {
	std::lock_guard l(force_sequential);

	EXPECT_TRUE(test_rendering("D:\\VS2007Projects\\open_v2_test_data\\t_sprite_fragment", 0, 0, 36, 36, [](open_gl_wrapper& ogl) {
		texture strip_tex("D:\\VS2007Projects\\open_v2_test_data\\strip10.dds");

		ogl.render_subsprite(graphics::color_modification::none, 7, 10, 0.0, 0.0, 36.0, 36.0, strip_tex);
	}));
}

TEST(graphics_tests, border_stretch) {
	std::lock_guard l(force_sequential);

	EXPECT_TRUE(test_rendering("D:\\VS2007Projects\\open_v2_test_data\\t_border", 0, 0, 60, 50, [](open_gl_wrapper& ogl) {
		texture bord("D:\\VS2007Projects\\open_v2_test_data\\border.dds");

		ogl.render_bordered_rect(graphics::color_modification::none, 32.0f, 0.0f, 0.0f, 350.0f, 150.0f, bord);
	}));
}

TEST(graphics_tests, sprite_tinting) {
	std::lock_guard l(force_sequential);

	EXPECT_TRUE(test_rendering("D:\\VS2007Projects\\open_v2_test_data\\t_tint", 0, 0, 60, 50, [](open_gl_wrapper& ogl) {
		texture ico("D:\\VS2007Projects\\open_v2_test_data\\legend_icon.dds");

		ogl.render_tinted_textured_rect(0.0f, 0.0f, 24.0f, 24.0f, 1.0f, 0.5f, 0.0f, ico);
		ogl.render_tinted_textured_rect(24.0f, 0.0f, 24.0f, 24.0f, 0.2f, 0.2f, 1.0f, ico);
		ogl.render_tinted_textured_rect(0.0f, 24.0f, 24.0f, 24.0f, 1.0f, 1.0f, 1.0f, ico);
	}));
}

class test_file_structure {
public:
	directory_representation f_root = directory_representation(u"F:");
	directory_representation gfx_root = directory_representation(u"gfx", f_root);
	directory_representation mod = directory_representation(u"mod", f_root);
	directory_representation mod_gfx = directory_representation(u"gfx", mod);

	file_representation alt_a_file_b = file_representation(u"file_a.tga", gfx_root, "");
	file_representation a_file_a = file_representation(u"file_b.dds", gfx_root, "");

	file_representation b_file_a = file_representation(u"file_a.tga", mod_gfx, "");

	test_file_structure() {
		set_default_root(f_root);
	}
};

TEST(graphics_tests, texture_manager_test) {
	test_file_structure real_fs;
	file_system f;

	f.set_root(u"F:");

	const char v1[] = "gfx\\file_a.tga";
	const char v2[] = "gfx\\\\file_b.tga";

	texture_manager tm_a;
	const auto handle_a = tm_a.retrieve_by_name(f.get_root(), RANGE(v1));
	EXPECT_EQ(texture_tag(0), handle_a);
	const auto handle_b = tm_a.retrieve_by_name(f.get_root(), RANGE(v2));
	EXPECT_EQ(texture_tag(1), handle_b);

	EXPECT_EQ(std::string("F:\\gfx\\file_a.tga"), tm_a.retrieve_by_key(handle_a).filename);
	EXPECT_EQ(std::string("F:\\gfx\\file_b.dds"), tm_a.retrieve_by_key(handle_b).filename);

	f.add_root(u"F:\\mod");

	texture_manager tm_b;

	const char v3[] = "gfx\\\\file_a.tga";
	const char v4[] = "gfx\\file_b.tga";
	const auto handle_c = tm_b.retrieve_by_name(f.get_root(), RANGE(v3));
	EXPECT_EQ(texture_tag(0), handle_c);
	const auto handle_d = tm_b.retrieve_by_name(f.get_root(), RANGE(v4));
	EXPECT_EQ(texture_tag(1), handle_d);

	EXPECT_EQ(std::string("F:\\mod\\gfx\\file_a.tga"), tm_b.retrieve_by_key(handle_c).filename);
	EXPECT_EQ(std::string("F:\\gfx\\file_b.dds"), tm_b.retrieve_by_key(handle_d).filename);
}

TEST(graphics_tests, texture_manager_unique) {
	test_file_structure real_fs;
	file_system f;

	f.set_root(u"F:");

	const char v1[] = "gfx\\file_a.tga";

	texture_manager tm_a;
	const auto handle_a = tm_a.retrieve_by_name(f.get_root(), RANGE(v1));
	EXPECT_EQ(texture_tag(0), handle_a);
	const auto handle_b = tm_a.retrieve_by_name(f.get_root(), RANGE(v1));
	EXPECT_EQ(texture_tag(0), handle_b);
}

TEST(graphics_tests, font_manager_test) {
	font_manager fm;

	const char v1[] = "Arial_17_black";
	const char v2[] = "ToolTip_Font";
	const char v3[] = "unknownfont";
	const char v4[] = "vic_32";
	const char v5[] = "FPS_Font";

	const auto f1 = fm.find_font(RANGE(v1));
	const auto f2 = fm.find_font(RANGE(v2));
	const auto f3 = fm.find_font(RANGE(v3));
	const auto f4 = fm.find_font(RANGE(v4));

	EXPECT_EQ(f1, font_tag(1));
	EXPECT_EQ(f2, font_tag(1));
	EXPECT_EQ(f3, font_tag(0));
	EXPECT_EQ(f4, font_tag(2));

	EXPECT_EQ(18ui32, fm.find_font_size(RANGE(v1)));
	EXPECT_EQ(14ui32, fm.find_font_size(RANGE(v5)));
	EXPECT_EQ(32ui32, fm.find_font_size(RANGE(v4)));
	EXPECT_EQ(14ui32, fm.find_font_size(RANGE(v3)));
}

TEST(graphics_tests, clipping) {
	std::lock_guard l(force_sequential);

	EXPECT_TRUE(test_rendering("D:\\VS2007Projects\\open_v2_test_data\\clipping_a", 0, 0, 80, 80, [](open_gl_wrapper& ogl) {
		texture test_tex("D:\\VS2007Projects\\open_v2_test_data\\test_tx.bmp");

		scissor_rect r1(ogl, 10, 10, 60, 60);
		ogl.render_textured_rect(graphics::color_modification::none, 0.0f, 0.0f, 80.0f, 40.0f, test_tex);
		ogl.render_textured_rect(graphics::color_modification::disabled, 0.0f, 40.0f, 80.0f, 40.0f, test_tex);
	}));

	EXPECT_TRUE(test_rendering("D:\\VS2007Projects\\open_v2_test_data\\clipping_b", 0, 0, 80, 80, [](open_gl_wrapper& ogl) {
		texture test_tex("D:\\VS2007Projects\\open_v2_test_data\\test_tx.bmp");

		scissor_rect r1(ogl, 10, 10, 60, 60);
		{
			scissor_rect r2(ogl, 0, 00, 40, 80);
			ogl.render_textured_rect(graphics::color_modification::none, 0.0f, 0.0f, 80.0f, 40.0f, test_tex);
		}
		ogl.render_textured_rect(graphics::color_modification::disabled, 0.0f, 40.0f, 80.0f, 40.0f, test_tex);
	}));
}
