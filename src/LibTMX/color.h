#ifndef __color__
#define __color__

class color {

public:

	std::string hex_string;
	unsigned int red;
	unsigned int green;
	unsigned int blue;
	unsigned int alpha;

public:
	color() { reset();}

public:

	//is_empty verifica se a cor possui uma faixa de valores válida para seus componentes R,G e B
	inline bool is_empty() { return red > 0xFF || green > 0xFF || blue > 0xFF; }

	inline void reset() { red = green = blue = 0x100; alpha = 0xFF; }
};

#endif

