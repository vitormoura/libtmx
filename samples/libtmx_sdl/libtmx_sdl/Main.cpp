// libtmx_sdl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>


SDL_Texture* load_texture(SDL_Renderer* renderer, const std::string local, Uint32 colorkey)
{
	assert(renderer != nullptr);
	assert(local.size() > 0);

	//Caso ele não exista ainda, podemos iniciar a carga desse sprite
	auto tempSurface = IMG_Load(local.c_str());

	if (tempSurface == nullptr)
		return nullptr;

	if (!SDL_SetColorKey(tempSurface, SDL_TRUE, colorkey)) {
		cout << SDL_GetError() << endl;
	}

	auto txt = SDL_CreateTextureFromSurface(renderer, tempSurface);

	if (!txt)
		return nullptr;

	SDL_FreeSurface(tempSurface);

	return txt;
}


int main(int argc, char *args[])
{
	using namespace std;

	tmxparser::map<> m;

	if (!m.load("./assets/lab_map_001.tmx")) {
		cout << m.error << endl;
		return EXIT_FAILURE;
	}

	SDL_Window *window;                    // Declare a pointer
	SDL_Renderer *renderer;
	SDL_Event event;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cout << SDL_GetError() << endl;
		return EXIT_FAILURE;
	}

										   // Create an application window with the following settings:
	window = SDL_CreateWindow(
		"An SDL2 window",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		m.width * m.tile_width,                               // width, in pixels
		m.height * m.tile_height,                               // height, in pixels
		SDL_WINDOW_OPENGL                  // flags - see below
		);

	// Check that the window was successfully created
	if (window == NULL) {
		// In the case that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	///*
	string hexString = "FF00FF";
	int hexNumber;
	sscanf(hexString.c_str(), "%x", &hexNumber);
	//*/

	auto tileset = m.tilesets.at(0);
	auto texture = load_texture(renderer, tileset->image->source, hexNumber );

	// The window is open: could enter program loop here (see SDL_PollEvent())
		
	while (1) {

		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			break;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);


		for (auto& layer : m.layers) {
			
			//Camadas de tiles são desenhadas
			if (layer->type == tmxparser::layer_types::tiles_t && layer->visible) {

				auto tile_layer = dynamic_cast<tmxparser::tile_layer*>(layer.get());

				//ajustando alpha da textura que será desenhada
				if (tile_layer->opacity != 1.0) {
					SDL_SetTextureAlphaMod(texture, 255 * tile_layer->opacity);
				}
				else {
					SDL_SetTextureAlphaMod(texture, 255);
				}
				
				//linhas 
				for (size_t l = 0; l < m.width; l++)
				{
					//colunas
					for (size_t c = 0; c < m.height; c++)
					{
						//recuperando tile da linha e coluna
						auto tile = tile_layer->get_tile(c, l);
						
						//nem sempre existe um tile na posição desejada
						if (tile != nullptr) {
							
							//A imagem a ser desenhada é linha da posição X/Y do tile e do tamanho padrão dos tiles nesse tileset
							auto srcRect = SDL_Rect{ (int)tile->position.x, (int)tile->position.y, (int)tileset->tile_width, (int)tileset->tile_height };

							//O rect destino precisa ser calculado levando em consideração a linha, coluna e offsets da camada e do tileset
							auto dstRect = SDL_Rect{
								(int)tileset->tile_offset.x + (int)layer->offset.x + (int)(c * tileset->tile_width),
								(int)tileset->tile_offset.y + (int)layer->offset.y + (int)(l * tileset->tile_height),
								(int)tileset->tile_width,
								(int)tileset->tile_height
							};
														
							SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
						}
					}
				}
			}
		}
				
		SDL_RenderPresent(renderer);
	}
		
	SDL_DestroyTexture(texture);

	// Close and destroy the window
	SDL_DestroyWindow(window);

	// Clean up
	SDL_Quit();
	return 0;
}

