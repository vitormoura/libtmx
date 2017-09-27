#ifndef __image_layer__
#define __image_layer__

#include <string>
#include <vector>
#include "point.h"
#include "custom_property.h"
#include "layer.h"
#include "tileset_image.h"

namespace tmxparser {

	using namespace std;

	class image_layer : 
		public layer {

	public:

		// The x position of the image layer in pixels. (deprecated since 0.15)
		int x;

		//The y position of the image layer in pixels. (deprecated since 0.15)
		int y;
				
		//
		std::map<string, shared_ptr<custom_property>> properties;

		//
		shared_ptr<tileset_image> image;
			
	};
}

#endif
