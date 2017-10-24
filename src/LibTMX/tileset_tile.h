#ifndef __tileset_tile__
#define __tileset_tile__

#include <vector>
#include <string>
#include "custom_property.h"
#include "object_group.h"
#include "animation_frame.h"

namespace tmxparser {

	class tileset_tile {
	public:

		//The local tile ID within its tileset.
		int id;

		//
		point position;

		//The type of the tile. Refers to an object type and is used by tile objects
		string type;

		std::map<string, shared_ptr<custom_property>> properties;
		
		shared_ptr<object_group> object_group;

		shared_ptr<tileset_image> image;
		
		//A percentage indicating the probability that this tile is chosen when it competes with others while editing with the terrain tool
		float probability;

		//terrain
		
		//frames de anima��o
		vector<animation_frame> animation;

		tileset_tile() 
			: probability(1.0f) {

		}
	};
}

#endif // !__tileset_tile__

