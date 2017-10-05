#ifndef __tileset__
#define __tileset__

#include <string>
#include <vector>
#include "rapidxml.hpp"
#include "point.h"
#include "tileset_terrain.h"
#include "tileset_image.h"
#include "tileset_tile.h"
#include "color.h"

using namespace std;
using namespace rapidxml;

namespace tmxparser {

	class tileset {

	public:

		//The first global tile ID of this tileset (this global ID maps to the first tile in this tileset).
		int firstgid;

		//If this tileset is stored in an external TSX (Tile Set XML) file
		string source;

		//The name of this tileset
		string name;

		//The(maximum) width of the tiles in this tileset
		size_t tile_width;

		//The(maximum) height of the tiles in this tileset
		size_t tile_height;

		//The spacing in pixels between the tiles in this tileset (applies to the tileset image)
		size_t spacing;

		//The margin around the tiles in this tileset (applies to the tileset image)
		size_t margin;

		//The number of tiles in this tileset (since 0.13)
		size_t tile_count;

		//The number of tile columns in the tileset. For image collection tilesets it is editable and is used when displaying the tileset. (since 0.15)
		size_t columns;

		//
		color background_color;

		//This element is used to specify an offset in pixels, to be applied when drawing a tile from the related tileset
		point tile_offset;

		//This element defines an array of terrain types, which can be referenced from the terrain attribute of the tile element
		shared_ptr<vector<shared_ptr<tileset_terrain>>> terrain_types;

		//
		std::map<int,shared_ptr<tileset_tile>> tiles;

		//
		shared_ptr<tileset_image> image;

	public:
		tileset() : tile_offset({0,0}), spacing(0), columns(0), margin(0), tile_width(0), tile_height(0), tile_count(0), background_color() {}
		
	};
}
#endif // !__tileset__

