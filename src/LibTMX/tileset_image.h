#ifndef __image__
#define __image__

#include <string>
#include <vector>

using namespace std;

namespace tmxparser {

	class tileset_image {

	public:

		//Used for embedded images, in combination with a data child element
		string format;

		//The reference to the tileset image file (Tiled supports most common image formats)
		string source;

		//Defines a specific color that is treated as transparent (example value: "#FF00FF" for magenta)
		string transparency_color;

		//The image width in pixels (optional, used for tile index correction when the image changes)
		size_t width;

		//The image height in pixels (optional)
		size_t height;

		//
		vector<int>* data;
	};
}

#endif // !__image__

