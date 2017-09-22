#ifndef __layer__
#define __layer__

#include <string>
#include <vector>
#include "point.h"
#include "custom_property.h"

namespace tmxparser {
	
	class layer {

	public:

		//The name of the layer
		string name;

		//The opacity of the layer as a value from 0 to 1. Defaults to 1
		float opacity;

		//Whether the layer is shown (1) or hidden (0). Defaults to 1
		bool visible;

		//Rendering offset for this layer in pixels. Defaults to 0. (since 0.14)
		point offset;

		//The width of the layer in tiles. Traditionally required, but as of Tiled Qt always the same as the map width
		size_t width;

		//The height of the layer in tiles. Traditionally required, but as of Tiled Qt always the same as the map height
		size_t height;

		//
		vector<custom_property*>* properties;
	};
}


#endif // !__layer__
#pragma once
