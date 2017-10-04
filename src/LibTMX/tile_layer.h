#ifndef __tile_layer__
#define __tile_layer__

#include <string>
#include <vector>
#include "point.h"
#include "layer.h"
#include "custom_property.h"

namespace tmxparser {

	class tile_layer 
		: public layer 
	{

	public:
		
		//Whatever format you choose for your layer data, you will always end up with so called "global tile IDs" (gids). They are global, since they may refer to a tile from any of the tilesets used by the map
		shared_ptr<vector<shared_ptr<tileset_tile>>> data;
					
	public:
		tile_layer() : layer::layer(layer_types::tiles_t) {}

	public:

		//get_tile recupera tile na linha e coluna informadas
		inline shared_ptr<tileset_tile> get_tile(size_t col, size_t line) {
			assert(line * width + col < data->size());

			return data->at(line * width + col);
		}

		//get_tile recupera tile com índice sequecial informado
		inline shared_ptr<tileset_tile> get_tile(size_t index) {
			assert(index < data->size());
			return data->at(index);
		}

	};
}


#endif