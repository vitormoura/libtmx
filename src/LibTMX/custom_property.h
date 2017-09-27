#ifndef __custom_property__
#define __custom_property__

#include <string>
#include "enums.h"

namespace tmxparser {
		
	using namespace std;

	class custom_property {

	public:
		
		// The name of the property.
		string name;

		//The type of the property. Can be string (default), int, float, bool, color or file (since 0.16, with color and file added in 0.17).
		prop_types type;

		//The value of the property.
		string value;
	};
}

#endif // !__custom_property__

