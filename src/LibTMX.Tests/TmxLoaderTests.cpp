#include <gtest\gtest.h>
#include "../LibTMX/map.h"

using namespace tmxparser;

TEST(Tests_CargaArquivo, CargaArquivoNaoExistenteRetornaFalse) {

	tmxparser::map<> map;
	
	EXPECT_FALSE(map.load("./Assets/lab_map_002.tmx"));
	EXPECT_TRUE(map.error.size() > 0);
}

TEST(Tests_CargaArquivo, CargaCaminhoArquivoVazioRetornaFalse) {

	tmxparser::map<> map;

	EXPECT_FALSE(map.load(""));
	ASSERT_STRCASEEQ(map.error.c_str(), "nenhum caminho de arquivo fornecido");
}

TEST(Tests_CargaArquivo, CargaCaminhoArquivoXmlInvalidoRetornaFalse) {

	tmxparser::map<> map;

	EXPECT_FALSE(map.load("./Assets/lab_map_001_invalid.tmx"));
	ASSERT_STRCASEEQ(map.error.c_str(), "arquivo xml nao parece ser um arquivo tmx valido");
}

TEST(Tests_CargaArquivo, CargaArquivoValidoRetornaTrue) {

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
	ASSERT_STREQ(map.file_path.first.c_str(), "./Assets");
	ASSERT_STREQ(map.file_path.second.c_str(), "lab_map_001.tmx");

	//Tileset
	ASSERT_EQ(map.tilesets.size(), 1);
		
	auto ts = map.tilesets[0];

	ASSERT_EQ(ts->firstgid, 1);
	ASSERT_STRCASEEQ(ts->name.c_str(), "routes_003");
	ASSERT_EQ(ts->tile_width, 32);
	ASSERT_EQ(ts->tile_height, 32);
	ASSERT_EQ(ts->tile_count, 1792);
	ASSERT_EQ(ts->columns, 8);
	ASSERT_EQ(ts->tile_offset.x, 0);
	ASSERT_EQ(ts->tile_offset.y, 0);
	
	
	//Tileset/image
	
	ASSERT_EQ(ts->image->format, "");
	ASSERT_EQ(ts->image->width, 256);
	ASSERT_EQ(ts->image->height, 7199);
	
	ASSERT_FALSE(ts->image->transparency_color.is_empty());
	ASSERT_STREQ(ts->image->transparency_color.hex_string.c_str(), "FF00FF");
	ASSERT_EQ(ts->image->transparency_color.red,255);
	ASSERT_EQ(ts->image->transparency_color.green, 0);
	ASSERT_EQ(ts->image->transparency_color.blue, 255);

	ASSERT_EQ(ts->image->source, "./Assets/lab_tilemap_001.png");
	ASSERT_TRUE(ts->image->data == nullptr);

	//Tileset/tiles
		
	ASSERT_EQ(ts->tiles.size(), 1792);

	auto ts_0 = ts->tiles[45];

	ASSERT_EQ(ts_0->id, 45);
	ASSERT_EQ(ts_0->properties.size(), 1);
	ASSERT_EQ(ts_0->properties["Type"]->value, "teste");
			
	ASSERT_TRUE(ts_0->object_group != nullptr);
	ASSERT_EQ(ts_0->object_group->draw_order, object_group::draworder::index );
	ASSERT_EQ(ts_0->object_group->name, "");
		
	ASSERT_EQ(ts_0->object_group->properties.size(), 2);
	ASSERT_EQ(ts_0->object_group->properties["Tag"]->value, "Pared");
	
	ASSERT_TRUE(ts_0->object_group->objects != nullptr);
	ASSERT_EQ(ts_0->object_group->objects->size(), 1);
	
	ASSERT_EQ(ts_0->object_group->objects->at(0)->id, 5);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->position.x, 0);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->position.y, 0);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->width, 32);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->height, 32);

	auto ts_2 = ts->tiles[177];

	ASSERT_EQ(ts_2->object_group->objects->size(), 1);
	ASSERT_EQ(ts_2->object_group->objects->at(0)->id, 1);
	ASSERT_EQ(ts_2->object_group->objects->at(0)->position.x, -3);
	ASSERT_EQ(ts_2->object_group->objects->at(0)->position.y, -2);
	ASSERT_EQ(ts_2->object_group->objects->at(0)->width, 40);
	ASSERT_EQ(ts_2->object_group->objects->at(0)->height, 37);

	ASSERT_EQ(map.tile_layers["Terrain"]->data->size(), 576);

	auto tl_tile_0 =  map.tile_layers["Terrain"]->data->at(0);

	ASSERT_EQ( tl_tile_0->id, 1116);

	auto tl_tile_23 = map.tile_layers["Terrain"]->data->at(23);

	ASSERT_EQ(tl_tile_23->id, 918);


	//Layers
	ASSERT_EQ(map.layers.size(), 5);
	ASSERT_EQ(map.layers[0]->type, tmxparser::layer_types::tiles_t);
	ASSERT_EQ(map.layers[1]->type, tmxparser::layer_types::image_t);
	ASSERT_EQ(map.layers[2]->type, tmxparser::layer_types::tiles_t);
	ASSERT_EQ(map.layers[3]->type, tmxparser::layer_types::object_t);
	ASSERT_EQ(map.layers[4]->type, tmxparser::layer_types::object_t);

	//Object layers
	ASSERT_EQ(map.object_groups.size(), 2);
	
	auto group_Regions = map.object_groups["Regions"];
	auto obj_com_polygon = group_Regions->objects->at(3);

	ASSERT_TRUE(obj_com_polygon->shape == object_shapes::polygon_t);
	ASSERT_TRUE(obj_com_polygon->points != nullptr);
	ASSERT_EQ(obj_com_polygon->points->size(), 7);
	
	/*
	ASSERT_EQ(obj_com_polygon->polygon->at(0).x, 0);
	ASSERT_EQ(obj_com_polygon->polygon->at(0).y, 10);
	ASSERT_EQ(obj_com_polygon->polygon->at(1).x, 94.6667f);
	ASSERT_EQ(obj_com_polygon->polygon->at(1).y, 11);
	ASSERT_EQ(obj_com_polygon->polygon->at(2).x, 95.6667f);
	ASSERT_EQ(obj_com_polygon->polygon->at(2).y, -85.3333f);
	*/

	ASSERT_EQ(map.layers[1]->visible, false);

	//TODO: implementar demais verifica��es

	map.unload();

	ASSERT_TRUE(map.tilesets.size() == 0);
	
}

TEST(Tests_CargaArquivo, CargaArquivoTsxExternoValidoRetornaTrue) {

	tmxparser::map<> map;

	EXPECT_TRUE(map.load("./Assets/snf_tilemap.tmx"));

	ASSERT_EQ(map.tilesets.size(), 1);

	auto ts = map.tilesets[0];

	ASSERT_EQ(ts->firstgid, 1);
	ASSERT_STRCASEEQ(ts->name.c_str(), "snf_tileset");
	ASSERT_EQ(ts->tile_width, 64);
	ASSERT_EQ(ts->tile_height, 81);
	ASSERT_EQ(ts->tile_count, 6);
	ASSERT_EQ(ts->columns, 0);
	ASSERT_EQ(ts->tile_offset.x, 0);
	ASSERT_EQ(ts->tile_offset.y, 0);

	ASSERT_TRUE(ts->tiles.size() == 6);

	auto ts_0 = ts->tiles[1];

	ASSERT_TRUE(ts_0 != nullptr);

	ASSERT_EQ(ts_0->id, 1);
	
	ASSERT_TRUE(ts_0->image != nullptr);
	ASSERT_EQ(ts_0->image->source, "./Assets/ranma_male/stand_0.png");
	ASSERT_EQ(ts_0->image->width, 64);
	ASSERT_EQ(ts_0->image->height, 79);

	ASSERT_TRUE(ts_0->animation.size() == 3);

	ASSERT_TRUE(ts_0->object_group != nullptr);
	ASSERT_EQ(ts_0->object_group->draw_order, object_group::draworder::index);
	ASSERT_EQ(ts_0->object_group->name, "");
		
	ASSERT_TRUE(ts_0->object_group->objects != nullptr);
	ASSERT_EQ(ts_0->object_group->objects->size(), 1);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->id, 1);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->position.x, 28);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->position.y, 9);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->width, 18);
	ASSERT_EQ(ts_0->object_group->objects->at(0)->height, 42);
}
