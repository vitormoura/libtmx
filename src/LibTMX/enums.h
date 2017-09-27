#pragma once

namespace tmxparser {
			
	enum layer_types {
		tiles_t,
		object_t,
		image_t
	};

	enum orientations {
		undefined,
		orthogonal,
		isometric,
		staggered,
		hexagonal
	};

	enum renderorders {
		right_down,
		right_up,
		left_down,
		left_up
	};

	enum prop_types {
		string_t,
		int_t,
		float_t,
		bool_t,
		color_t,
		file_t
	};
}
