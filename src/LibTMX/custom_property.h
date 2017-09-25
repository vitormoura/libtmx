#ifndef __custom_property__
#define __custom_property__

#include <string>

namespace tmxparser {
		
	using namespace std;

	class custom_property {
	
	public:
		enum types {
			string_type, 
			int_type, 
			float_type, 
			bool_type, 
			color_type,
			file_type
		};

	public:
		
		// The name of the property.
		string name;

		//The type of the property. Can be string (default), int, float, bool, color or file (since 0.16, with color and file added in 0.17).
		types type;

		//The value of the property.
		string value;
	};
}

#endif // !__custom_property__

