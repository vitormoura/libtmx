#ifndef __object__
#define __object__

#include <string>
#include <vector>
#include "point.h"
#include "custom_property.h"

namespace tmxparser {
	
	class object {
	public:

		//Unique ID of the object. Each object that is placed on a map gets a unique id. Even if an object was deleted, no object gets the same ID
		int id;

		//The name of the object. An arbitrary string
		string name;

		//The type of the object. An arbitrary string
		string type;

		//The x coordinate of the object in pixels
		int x;

		//The y coordinate of the object in pixels
		int y;

		// The width of the object in pixels (defaults to 0)
		size_t width;

		//The height of the object in pixels (defaults to 0)
		size_t height;

		//The rotation of the object in degrees clockwise (defaults to 0). (since 0.10)
		int rotation;

		//An reference to a tile (optional)
		int tile_gid;

		//Whether the object is shown (1) or hidden (0). Defaults to 1. (since 0.9)
		bool visible;

		//
		std::map<string, shared_ptr<custom_property> > properties;

		//ellipse
		//polygon
		//polyline
		//text
	};
}


#endif // !__object__
