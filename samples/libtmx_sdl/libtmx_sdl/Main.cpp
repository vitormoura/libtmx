// libtmx_sdl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>


SDL_Texture* load_texture(SDL_Renderer* renderer, const std::string local, bool setColorKey, SDL_Color color)
{
	assert(renderer != nullptr);
	assert(local.size() > 0);

	//Caso ele não exista ainda, podemos iniciar a carga desse sprite
	auto tempSurface = IMG_Load(local.c_str());

	if (tempSurface == nullptr)
		return nullptr;
	
	if (setColorKey) {
		if (!SDL_SetColorKey(tempSurface, SDL_TRUE, SDL_MapRGB(tempSurface->format, color.r, color.g, color.b))) {
			cout << SDL_GetError() << endl;
		}
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
		m.width * m.tile_width,            // width, in pixels
		m.height * m.tile_height,          // height, in pixels
		SDL_WINDOW_OPENGL                  // flags - see below
		);

	// Check that the window was successfully created
	if (window == NULL) {
		// In the case that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	if (!m.background_color.is_empty()) {
		SDL_SetRenderDrawColor(renderer, m.background_color.red, m.background_color.green, m.background_color.blue, m.background_color.alpha);
	}

	//Carregando apenas o primeiro tileset e sua textura associada
	auto tileset = m.tilesets.at(0);
	auto texture = load_texture(renderer, tileset->image->source, !tileset->image->transparency_color.is_empty(), SDL_Color{ (Uint8)tileset->image->transparency_color.red, (Uint8)tileset->image->transparency_color.green, (Uint8)tileset->image->transparency_color.blue, 255 });
	
	// The window is open: could enter program loop here (see SDL_PollEvent())
		
	while (1) {

		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			break;
		}
				
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
			//Camada de objetos (Desenhando representação de objetos apenas para demonstração)
			else if (layer->type == tmxparser::layer_types::object_t && layer->visible) {
				auto objs_layer = dynamic_cast<tmxparser::object_group*>(layer.get());
				
				for (auto& obj : *objs_layer->objects) {
					
					if (!objs_layer->color.is_empty()) {
						SDL_SetRenderDrawColor(renderer, objs_layer->color.red, objs_layer->color.green, objs_layer->color.blue, objs_layer->color.alpha);
					}

					if (obj->visible) {

						//Temos um polygon ou polyline?
						if (obj->shape == tmxparser::object_shapes::polygon_t || obj->shape == tmxparser::object_shapes::polyline_t) {
							
							auto offset = layer->offset;

							//SDL vai precisar de um vetor de pontos para desenhar o poligono/polyline
							vector<SDL_Point> points(obj->points->size());

							//A função 'transform' vai aplicar uma transformação para gerar pontos do tipo SDL_Point
							std::transform(
								obj->points->begin(), 
								obj->points->end(), 
								points.begin(), 
								[&offset](tmxparser::point p) { return SDL_Point{ (int)(offset.x + p.x), (int)(offset.y +p.y) }; }
							);
																					
							SDL_RenderDrawLines(renderer, &points.at(0), points.size());
						}
						else if (obj->shape == tmxparser::object_shapes::ellipse_t) {

							ellipseRGBA(renderer,
								layer->offset.x + obj->position.x + (obj->width / 2),
								layer->offset.x + obj->position.y + (obj->height / 2),
								obj->width / 2, 
								obj->height / 2,
								255, 0,0, 255 );
						}
						//Por padrão, um retângulo
						else {

							SDL_Rect rect{};
							
							rect.w = obj->width;
							rect.h = obj->height;
							rect.x = layer->offset.x + obj->position.x;
							rect.y = layer->offset.y + obj->position.y;

							SDL_RenderDrawRect(renderer, &rect);
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

