#ifndef __tileset_terrain__
#define __tileset_terrain__

#include <string>

namespace tmxparser {

	using namespace std;

	class tileset_terrain {
		
		// The name of the terrain type
		string name;

		//The local tile-id of the tile that represents the terrain visually.
		int tile;
	};
}

#endif
