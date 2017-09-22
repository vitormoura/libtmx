#include <iostream>
#include "map.h"


int main() {
	
	using namespace tmxparser;
	
	tmxparser::map<> m;

	if (m.load("./Assets/lab_map_001.tmx")) {


		
		cout << "version: " << m.version << endl;
		cout << "tiled version: " << m.tiled_version << endl;
		cout << "orientation: " << m.orientation << endl;
		cout << "render order: " << m.render_order << endl;
		cout << "width: " << m.width << endl;
		cout << "height: " << m.height << endl;
		cout << "tile_width: " << m.tile_width << endl;
		cout << "tile_height: " << m.tile_height << endl;
		cout << "layers:" << m.tile_layers.size() << endl;
		cout << "tilesets:" << m.tilesets.size() << endl;
		cout << "object_groups:" << m.object_groups.size() << endl;
		cout << "image layers:" << m.image_layers.size() << endl;
		
	}

	std::cin.get();
}