#ifndef __object_group__
#define __object_group__

#include <string>
#include <vector>
#include "enums.h"
#include "layer.h"
#include "custom_property.h"
#include "point.h"
#include "object.h"
#include "color.h"

namespace tmxparser {

	class object_group :
		public layer {

	public:

		enum draworder {
			index,
			topdown
		};

	public:
				
		//The color used to display the objects in this group.
		color color;
				
		//The x coordinate of the object group in tiles. Defaults to 0 and can no longer be changed in Tiled Qt
		int x;

		//y: The y coordinate of the object group in tiles. Defaults to 0 and can no longer be changed in Tiled Qt
		int y;

		//Whether the objects are drawn according to the order of appearance ("index") or sorted by their y-coordinate ("topdown"). Defaults to "topdown".
		draworder draw_order;

		//
		shared_ptr<vector<shared_ptr<object>>> objects;

	public:
		object_group() : layer::layer(layer_types::object_t) {}
	};
}

#endif