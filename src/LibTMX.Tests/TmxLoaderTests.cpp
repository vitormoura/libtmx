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

	ASSERT_EQ(ts->image.format, "");
	ASSERT_EQ(ts->image.width, 256);
	ASSERT_EQ(ts->image.height, 7199);
	ASSERT_STREQ(ts->image.transparency_color.c_str(), "FF00FF");
	ASSERT_EQ(ts->image.source, "lab_tilemap_001.png");

	ASSERT_TRUE(ts->tiles.size() == 3);

	//TODO: implementar demais verificações
}