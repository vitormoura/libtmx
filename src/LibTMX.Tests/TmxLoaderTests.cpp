#include <gtest\gtest.h>
#include "../LibTMX/map.h"

using namespace tmxparser;

TEST(TmxLoaderTests, CargaArquivoNaoExistenteRetornaFalse) {

	tmxparser::map<> map;
	
	EXPECT_FALSE(map.load("./Assets/lab_map_002.tmx"));
	EXPECT_TRUE(map.error.size() > 0);
}

TEST(TmxLoaderTests, CargaCaminhoArquivoVazioRetornaFalse) {

	tmxparser::map<> map;

	EXPECT_FALSE(map.load(""));
	ASSERT_STRCASEEQ(map.error.c_str(), "nenhum caminho de arquivo fornecido");
}

TEST(TmxLoaderTests, CargaCaminhoArquivoXmlInvalidoRetornaFalse) {

	tmxparser::map<> map;

	EXPECT_FALSE(map.load("./Assets/lab_map_001_invalid.tmx"));
	ASSERT_STRCASEEQ(map.error.c_str(), "arquivo xml nao parece ser um arquivo tmx valido");
}

TEST(TmxLoaderTests, CargaArquivoValidoRetornaTrue) {

	tmxparser::map<> map;

	EXPECT_TRUE(map.load("./Assets/lab_map_001.tmx")); 

	//Map attributes
	ASSERT_STRCASEEQ(map.version.c_str(), "1.0");
	ASSERT_EQ(map.orientation, tmxparser::orientations::orthogonal);
	ASSERT_EQ(map.render_order, tmxparser::renderorders::left_down);
	ASSERT_EQ(map.width, 24);
	ASSERT_EQ(map.height, 24);
	ASSERT_EQ(map.tile_width, 32);
	ASSERT_EQ(map.tile_height, 32);

	//Tileset
	ASSERT_TRUE(map.tilesets.size() == 1);
		
	auto ts = map.tilesets[0];

	ASSERT_EQ(ts->firstgid, 1);
	ASSERT_STRCASEEQ(ts->name.c_str(), "routes_003");
	ASSERT_EQ(ts->tile_width, 32);
	ASSERT_EQ(ts->tile_height, 32);
	ASSERT_EQ(ts->tile_count, 1792);
	ASSERT_EQ(ts->columns, 8);
	ASSERT_EQ(ts->tile_offset.x, 15);
	ASSERT_EQ(ts->tile_offset.y, 15);
	
	//Tileset/image
	
	ASSERT_EQ(ts->image->format, "");
	ASSERT_EQ(ts->image->width, 256);
	ASSERT_EQ(ts->image->height, 7199);
	ASSERT_STREQ(ts->image->transparency_color.c_str(), "FF00FF");
	ASSERT_EQ(ts->image->source, "lab_tilemap_001.png");
	ASSERT_TRUE(ts->image->data == nullptr);

	//Tileset/tiles

	ASSERT_TRUE(ts->tiles != nullptr);
	ASSERT_TRUE(ts->tiles->size() == 3);

	auto ts_0 = ts->tiles->at(0);

	ASSERT_EQ(ts_0->id, 44);
	ASSERT_EQ(ts_0->properties->size(), 1);
	ASSERT_EQ(ts_0->properties->at(0)->name, "Type");
	ASSERT_EQ(ts_0->properties->at(0)->value, "teste");

	ASSERT_TRUE(ts_0->object_group != nullptr);
	ASSERT_EQ(ts_0->object_group->draw_order, object_group::draworder::index );
	ASSERT_EQ(ts_0->object_group->name, "");
	
	ASSERT_TRUE(ts_0->object_group->properties != nullptr);
	ASSERT_EQ(ts_0->object_group->properties->size(), 2);
	ASSERT_EQ(ts_0->object_group->properties->at(0)->name, "Tag");
	ASSERT_EQ(ts_0->object_group->properties->at(0)->value, "Pared");

	ASSERT_TRUE(ts_0->object_group->objects != nullptr);
	ASSERT_EQ(ts_0->object_group->objects->size(), 1);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->id, 5);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->x, 0);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->y, 0);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->width, 32);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->height, 32);

	auto ts_2 = ts->tiles->at(2);

	ASSERT_EQ(ts_2->object_group->objects->size(), 1);
	ASSERT_EQ(ts_2->object_group->objects->at(0)->id, 1);
	ASSERT_EQ(ts_2->object_group->objects->at(0)->x, -3);
	ASSERT_EQ(ts_2->object_group->objects->at(0)->y, -2);
	ASSERT_EQ(ts_2->object_group->objects->at(0)->width, 40);
	ASSERT_EQ(ts_2->object_group->objects->at(0)->height, 37);

	//TODO: implementar demais verificações

	map.unload();

	ASSERT_TRUE(map.tilesets.size() == 0);
	
}
