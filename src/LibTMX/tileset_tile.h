#ifndef __tileset_tile__
#define __tileset_tile__

#include <vector>
#include <string>
#include "custom_property.h"
#include "object_group.h"

namespace tmxparser {

	class tileset_tile {
	public:

		//The local tile ID within its tileset.
		int id;

		//The type of the tile. Refers to an object type and is used by tile objects
		string type;

		vector<custom_property*>* properties;
		
		object_group* object_group;

		tileset_image* image;
		
		//A percentage indicating the probability that this tile is chosen when it competes with others while editing with the terrain tool
		float probability;

		//terrain
		//animation
	};
}

#endif // !__tileset_tile__

