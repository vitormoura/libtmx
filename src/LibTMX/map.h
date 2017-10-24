#ifndef __map__
#define __map__

#include <string>
#include <algorithm>
#include <sstream>
#include <map>
#include <memory>
#include "enums.h"
#include "tileset.h"
#include "tile_layer.h"
#include "image_layer.h"
#include "custom_property.h"
#include "object_group.h"
#include "object.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_iterators.hpp"

namespace tmxparser {

	using namespace std;
	using namespace rapidxml;

	//map representa um mapa do tiled editor, carregado a partir de arquivos .tmx
	template<class Ch = char>
	class map
	{

	public:

		// The TMX format version. Was "1.0" so far, and will be incremented to match minor Tiled releases
		string version;

		//The Tiled version used to save the file (since Tiled 1.0.1). May be a date (for snapshot builds)
		string tiled_version;

		//Map orientation. Tiled supports "orthogonal", "isometric", "staggered" (since 0.9) and "hexagonal" (since 0.11)
		orientations orientation;

		//The order in which tiles on tile layers are rendered. Valid values are right-down (the default), right-up, left-down and left-up. In all cases, the map is drawn row-by-row
		renderorders render_order;

		//The map width in tiles
		size_t width;

		//The map height in tiles
		size_t height;

		//The width of a tile
		size_t tile_width;

		//The height of a tile
		size_t tile_height;

		//hexsidelength
		//staggeraxis
		//staggerindex
		//nextobjectid

		// The background color of the map. 
		color background_color;

		//file_path contains the file directory and file name
		std::pair<string, string> file_path;

	public:

		//Última mensagem de erro levantada durante rotina de carga do arquivo TMX
		string error;

	public:

		vector<shared_ptr<tileset>>					tilesets;
		vector<shared_ptr<layer>>					layers;

		std::map<string, shared_ptr<tile_layer>>	tile_layers;
		std::map<string, shared_ptr<object_group>>	object_groups;
		std::map<string, shared_ptr<image_layer>>	image_layers;

	public:
		map() {}
		~map() {
			unload();
		}

	public:

		//load carrega o arquivo .tmx localizado no caminho informado, alimentando as propriedades desse objeto
		bool load(const char* filepath) {

			using namespace std;

			if (filepath == nullptr || strlen(filepath) == 0) {
				error = "nenhum caminho de arquivo fornecido";
				return false;
			}

			//Limpa dados carregados anteriormente
			unload();

			//Identificando diretório e arquivo carregado
			fill_filepath(filepath);

			xml_document<> doc;
			unique_ptr<file<>> tmxFile(nullptr);

			try {

				tmxFile.reset(new file<>(filepath));
				doc.parse<0>(tmxFile->data());
				//doc.parse<parse_declaration_node | parse_no_data_nodes>(tmxFile->data());
			}
			catch (std::exception ex) {
				error = ex.what();
				return false;
			}

			auto mapNode = doc.first_node("map");

			if (mapNode != nullptr) {

				//Preenchimento de atributos da tab <map>
				this->fill_map_attributes(mapNode);

				auto mapChildNode = mapNode->first_node();

				while (mapChildNode != nullptr)
				{
					//<layer>
					if (is_name_equals(mapChildNode, "layer")) {
						auto layer = make_shared<tile_layer>();

						this->fill_layer(mapChildNode, layer);

						this->tile_layers[layer->name] = layer;
						this->layers.push_back(layer);
					}
					//<objectgroup>
					else if (is_name_equals(mapChildNode, "objectgroup")) {
						auto objGrp = make_shared<object_group>();

						this->fill_objectgroup(mapChildNode, objGrp);

						this->object_groups[objGrp->name] = objGrp;
						this->layers.push_back(objGrp);
					}
					//<imagelayer>
					else if (is_name_equals(mapChildNode, "imagelayer")) {
						auto imgLayer = make_shared<image_layer>();

						this->fill_imagelayer(mapChildNode, imgLayer);

						this->image_layers[imgLayer->name] = imgLayer;
						this->layers.push_back(imgLayer);
					}
					//<tileset>
					else if (is_name_equals(mapChildNode, "tileset")) {
						auto t = make_shared<tileset>();

						//Vamos verificar se o tileset está presente em um arquivo externo
						auto sourceAttr = mapChildNode->first_attribute("source");

						if (sourceAttr == nullptr) {
							this->fill_tileset(mapChildNode, t);
						}
						else {
										
							read_to(sourceAttr, t->source);

							//Arquivo com detalhes sobre o tileset é externo
							if (t->source.size() > 0) {
								
								//o firstgid ainda é regido pelo arquivo TMX
								read_to(mapChildNode->first_attribute("firstgid"), &t->firstgid);
								
								//Aplicando prefixo do diretório onde o TMX está armazenado
								t->source.insert(0, "/");
								t->source.insert(0, this->file_path.first.c_str());
																								
								try {

									xml_document<> docTSX;

									unique_ptr<file<>> tsxFile(new file<>(t->source.c_str()));
									docTSX.parse<0>(tsxFile->data());
																		
									auto tsNode = docTSX.first_node("tileset");

									if (tsNode != nullptr) {
										this->fill_tileset(tsNode, t);
									}
								}
								catch (std::exception ex) {
									error = ex.what();
									return false;
								}
																
							}
						}

						this->tilesets.push_back(t);
					}


					mapChildNode = mapChildNode->next_sibling();
				}




				//Validação simples
				return version.size() > 0 && width > 0 && height > 0;
			}
			else {
				error = "arquivo xml nao parece ser um arquivo tmx valido";
				return false;
			}
		}

		//unload limpa todas as propriedades carregadas anteriormente
		void unload() {

			width = height = tile_width = tile_height = 0;

			version.clear();
			tiled_version.clear();
			background_color;
			error.clear();
			file_path.first.clear();
			file_path.second.clear();

			tilesets.clear();
			tile_layers.clear();
			object_groups.clear();
			image_layers.clear();
			layers.clear();
		}

	private:

		bool fill_filepath(const char* filepath) {

			this->file_path.first.clear();
			this->file_path.second.assign(filepath);

			size_t found = this->file_path.second.find_last_of("/\\");

			if (found != string::npos) {
				this->file_path.first = this->file_path.second.substr(0, found);
				this->file_path.second.erase(0, found + 1);

				//Se o diretório for equivalente ao '.' vamos removê-lo
				if (this->file_path.first.size() == 1 && this->file_path.first[0] == '.') {
					this->file_path.first.clear();
				}
			}

			return this->file_path.second.size() > 0;
		}

		bool fill_map_attributes(xml_node<Ch>* mapNode) {

			for (xml_attribute<>* attr = mapNode->first_attribute(); attr; attr = attr->next_attribute()) {

				if (is_name_equals(attr, "version")) {
					read_to(attr, version);
				}
				else if (is_name_equals(attr, "tiledversion")) {
					read_to(attr, tiled_version);
				}
				else if (is_name_equals(attr, "orientation")) {

					if (is_value_equals(attr, "orthogonal")) {
						this->orientation = orientations::orthogonal;
					}
					else if (is_value_equals(attr, "isometric")) {
						this->orientation = orientations::isometric;
					}
					else if (is_value_equals(attr, "hexagonal")) {
						this->orientation = orientations::hexagonal;
					}
					else if (is_value_equals(attr, "staggered")) {
						this->orientation = orientations::staggered;
					}
				}
				else if (is_name_equals(attr, "renderorder")) {

					if (is_value_equals(attr, "left-up")) {
						this->render_order = renderorders::left_up;
					}
					else if (is_value_equals(attr, "left-down")) {
						this->render_order = renderorders::left_down;
					}
					else if (is_value_equals(attr, "right-down")) {
						this->render_order = renderorders::right_down;
					}
					else if (is_value_equals(attr, "right-up")) {
						this->render_order = renderorders::right_up;
					}
				}
				else if (is_name_equals(attr, "width")) {
					read_to(attr, &width);
				}
				else if (is_name_equals(attr, "height")) {
					read_to(attr, &height);
				}
				else if (is_name_equals(attr, "tilewidth")) {
					read_to(attr, &tile_width);
				}
				else if (is_name_equals(attr, "tileheight")) {
					read_to(attr, &tile_height);
				}
				else if (is_name_equals(attr, "backgroundcolor")) {
					read_to(attr, background_color);
				}
			}

			return true;
		}

		bool fill_layer(xml_node<Ch>* layerNode, shared_ptr<tile_layer> lay) {

			lay->data = nullptr;
			lay->visible = true;
			lay->opacity = 1.0f;

			for (xml_attribute<Ch>* attr = layerNode->first_attribute(); attr; attr = attr->next_attribute()) {

				if (is_name_equals(attr, "name")) {
					read_to(attr, lay->name);
				}
				else if (is_name_equals(attr, "width")) {
					read_to(attr, &lay->width);
				}
				else if (is_name_equals(attr, "height")) {
					read_to(attr, &lay->height);
				}
				else if (is_name_equals(attr, "opacity")) {
					read_to(attr, &lay->opacity);
				}
				else if (is_name_equals(attr, "visible")) {
					read_to(attr, &lay->visible);
				}
				else if (is_name_equals(attr, "offsetx")) {
					read_to(attr, &lay->offset.x);
				}
				else if (is_name_equals(attr, "offsety")) {
					read_to(attr, &lay->offset.y);
				}
			}

			auto layerNodeChild = layerNode->first_node();

			do
			{
				if (is_name_equals(layerNodeChild, "data")) {

					auto encodingAttr = layerNodeChild->first_attribute("encoding");

					//Se enconding foi informado, podemos esperar dados na forma de lista de identificadores de tiles
					if (encodingAttr != nullptr)
					{
						auto compression = layerNodeChild->first_attribute("compression");
						char* stringContent = nullptr;

						//Se temos uma compressão diferente, devemos extrair a lista de identificadores  adequadamente
						if (compression != nullptr)
						{
							//TODO
						}
						//Do contrário só lemos o conteúdo na forma de uma string simples
						else
							stringContent = layerNodeChild->value();

						//Temos conteúdo para processar?
						if (stringContent != nullptr) {

							if (is_value_equals(encodingAttr, "csv"))
							{
								//Pré-alocando uma lista suficiente para guardar todas as referências
								lay->data = make_shared<vector<shared_ptr<tileset_tile>>>(lay->width * lay->height);

								int pos = 0;
								char separator;
								stringstream iss(stringContent);

								while (!iss.eof()) {
									int id;
									iss >> id; //Tile Global ID

									if (!iss.fail()) {
																				
										//Localizando tileset com maior firstgid igual ao ID global
										shared_ptr<tileset> ownerTs = nullptr;

										for (auto ts : this->tilesets) {
											if (id >= ts->firstgid) {
												ownerTs = ts;
											}
										}

										//Caso localizado, 
										if (ownerTs != nullptr) {
											lay->data->at(pos++) = ownerTs->tiles[id];
										}
										else {
											lay->data->at(pos++) = nullptr;
										}

										iss >> separator;

									}
								}
							}
						}
					}
					//No outro caso o conteúdo é uma lista de tiles
					else
					{

					}

				}
				else if (is_name_equals(layerNodeChild, "properties")) {
					this->fill_properties(layerNodeChild, lay->properties);
				}

				layerNodeChild = layerNodeChild->next_sibling();

			} while (layerNodeChild);

			return true;
		}

		bool fill_objectgroup(xml_node<Ch>* node, shared_ptr<object_group> grp) {

			grp->opacity = 1.0f;
			grp->visible = true;

			for (xml_attribute<Ch>* attr = node->first_attribute(); attr; attr = attr->next_attribute()) {

				if (is_name_equals(attr, "name")) {
					read_to(attr, grp->name);
				}
				else if (is_name_equals(attr, "x")) {
					read_to(attr, &grp->x);
				}
				else if (is_name_equals(attr, "y")) {
					read_to(attr, &grp->y);
				}
				else if (is_name_equals(attr, "width")) {
					read_to(attr, &grp->width);
				}
				else if (is_name_equals(attr, "height")) {
					read_to(attr, &grp->height);
				}
				else if (is_name_equals(attr, "opacity")) {
					read_to(attr, &grp->opacity);
				}
				else if (is_name_equals(attr, "visible")) {
					read_to(attr, &grp->visible);
				}
				else if (is_name_equals(attr, "offsetx")) {
					read_to(attr, &grp->offset.x);
				}
				else if (is_name_equals(attr, "offsety")) {
					read_to(attr, &grp->offset.y);
				}
				else if (is_name_equals(attr, "draworder")) {
					grp->draw_order = is_value_equals(attr, "index") ? object_group::draworder::index : object_group::draworder::topdown;
				}
				else if (is_name_equals(attr, "color")) {
					read_to(attr, grp->color);
				}
			}

			xml_node<Ch>* currentNode = node->first_node();

			do
			{
				if (is_name_equals(currentNode, "properties"))
				{
					this->fill_properties(currentNode, grp->properties);
				}
				else if (is_name_equals(currentNode, "object")) {

					if (grp->objects == nullptr)
						grp->objects = make_shared<vector<shared_ptr<object>>>();

					auto obj = make_shared<object>();

					this->fill_object(currentNode, obj);

					grp->objects->push_back(obj);
				}

				currentNode = currentNode->next_sibling();

			} while (currentNode);


			return false;
		}

		bool fill_imagelayer(xml_node<Ch>* node, shared_ptr<image_layer> imgLayer) {

			imgLayer->opacity = 1.0;
			imgLayer->visible = true;

			for (xml_attribute<Ch>* attr = node->first_attribute(); attr; attr = attr->next_attribute()) {

				if (is_name_equals(attr, "name")) {
					read_to(attr, imgLayer->name);
				}
				else if (is_name_equals(attr, "x")) {
					read_to(attr, &imgLayer->x);
				}
				else if (is_name_equals(attr, "y")) {
					read_to(attr, &imgLayer->y);
				}
				else if (is_name_equals(attr, "opacity")) {
					read_to(attr, &imgLayer->opacity);
				}
				else if (is_name_equals(attr, "visible")) {
					read_to(attr, &imgLayer->visible);
				}
				else if (is_name_equals(attr, "offsetx")) {
					read_to(attr, &imgLayer->offset.x);
				}
				else if (is_name_equals(attr, "offsety")) {
					read_to(attr, &imgLayer->offset.y);
				}
			}

			xml_node<Ch>* currentNode = node->first_node();

			while (currentNode) {

				if (is_name_equals(currentNode, "properties"))
				{
					this->fill_properties(currentNode, imgLayer->properties);
				}
				else if (is_name_equals(currentNode, "image")) {
					imgLayer->image = make_shared<tileset_image>();
					this->fill_image(currentNode, imgLayer->image);
				}

				currentNode = currentNode->next_sibling();
			}

			return false;
		}

		bool fill_tileset(xml_node<Ch>* node, shared_ptr<tileset> t) {
				
			xml_attribute<Ch>* attr = node->first_attribute();

			while (attr) {

				if (is_name_equals(attr, "firstgid")) {
					read_to(attr, &t->firstgid);
				}
				else if (is_name_equals(attr, "source")) {
					read_to(attr, t->source);
				}
				else if (is_name_equals(attr, "name")) {
					read_to(attr, t->name);
				}
				else if (is_name_equals(attr, "tilewidth")) {
					read_to(attr, &t->tile_width);
				}
				else if (is_name_equals(attr, "tileheight")) {
					read_to(attr, &t->tile_height);
				}
				else if (is_name_equals(attr, "spacing")) {
					read_to(attr, &t->spacing);
				}
				else if (is_name_equals(attr, "margin")) {
					read_to(attr, &t->margin);
				}
				else if (is_name_equals(attr, "tilecount")) {
					read_to(attr, &t->tile_count);
				}
				else if (is_name_equals(attr, "columns")) {
					read_to(attr, &t->columns);
				}
				else if (is_name_equals(attr, "backgroundcolor")) {
					read_to(attr, t->background_color);
				}

				attr = attr->next_attribute();
			}
			
			//Criando o conjunto padrão de tiles deste tileset
			for (size_t id = t->firstgid, i = 0; i < t->tile_count; id++, i++)
			{
				t->tiles[id] = make_shared<tileset_tile>();
				t->tiles[id]->id = id; //global ID

				//Nem sempre o tileset descreve a quantidade de colunas, geralmente isso ocorre quando esse tileset é formado por imagens individuais
				if (t->columns > 0) {

					auto line = (i / t->columns);

					t->tiles[id]->position.x = t->spacing + (t->tile_width * i) - (line * t->tile_width * t->columns);
					t->tiles[id]->position.y = t->spacing + (line * t->tile_height);
				}
				else {
					t->tiles[id]->position.x = t->spacing;
					t->tiles[id]->position.y = t->spacing;
				}
			}
						

			xml_node<Ch>* currentNode = node->first_node();

			do
			{
				if (is_name_equals(currentNode, "tileoffset"))
				{
					read_to(currentNode->first_attribute("x"), &t->tile_offset.x);
					read_to(currentNode->first_attribute("y"), &t->tile_offset.y);
				}
				else if (is_name_equals(currentNode, "image"))
				{
					t->image = make_shared<tileset_image>();
					this->fill_image(currentNode, t->image);
				}
				else if (is_name_equals(currentNode, "terraintypes"))
				{

				}
				else if (is_name_equals(currentNode, "tile"))
				{
					auto tempTile = make_shared<tileset_tile>();

					this->fill_tile(currentNode, tempTile);

					//O identificador do tile alimentado é um identificador local, devemos complementar seu ID com o firstgid do tileset
					tempTile->id = tempTile->id + t->firstgid;
					tempTile->position = t->tiles[tempTile->id]->position;

					//Será preciso atualizar também o identificador de tile do frame de animação levando em consideração o firstgid do tileset
					for (auto& af : tempTile->animation) {
						af.tileid = t->firstgid + af.tileid;
					}

					t->tiles[tempTile->id] = tempTile;
				}

				currentNode = currentNode->next_sibling();

			} while (currentNode);

			return false;
		}

		bool fill_properties(xml_node<Ch>* propertiesNode, std::map<string, shared_ptr<custom_property>>& result) {

			auto propChildNode = propertiesNode->first_node();

			while (propChildNode) {

				auto prop = make_shared<custom_property>();
				prop->type = prop_types::string_t;


				for (xml_attribute<Ch>* attr = propChildNode->first_attribute(); attr; attr = attr->next_attribute()) {

					if (is_name_equals(attr, "name")) {
						read_to(attr, prop->name);
					}
					else if (is_name_equals(attr, "value")) {
						read_to(attr, prop->value);
					}
					else if (is_name_equals(attr, "type")) {

						if (is_value_equals(attr, "int")) {
							prop->type = prop_types::int_t;
						}
						else if (is_value_equals(attr, "float")) {
							prop->type = prop_types::float_t;
						}
						else if (is_value_equals(attr, "bool")) {
							prop->type = prop_types::bool_t;
						}
						else if (is_value_equals(attr, "color")) {
							prop->type = prop_types::color_t;
						}
						else if (is_value_equals(attr, "file")) {
							prop->type = prop_types::file_t;
						}
					}
				}

				result[prop->name] = prop;

				propChildNode = propChildNode->next_sibling();
			}

			return true;
		}

		bool fill_object(xml_node<Ch>* objNode, shared_ptr<object> obj) {

			obj->visible = true;
			obj->rotation = 0;
			obj->shape = object_shapes::rect_t;

			for (xml_attribute<Ch>* attr = objNode->first_attribute(); attr; attr = attr->next_attribute()) {

				if (is_name_equals(attr, "id")) {
					read_to(attr, &obj->id);
				}
				else if (is_name_equals(attr, "name")) {
					read_to(attr, obj->name);
				}
				else if (is_name_equals(attr, "type")) {
					read_to(attr, obj->type);
				}
				else if (is_name_equals(attr, "x")) {
					read_to(attr, &obj->position.x);
				}
				else if (is_name_equals(attr, "y")) {
					read_to(attr, &obj->position.y);
				}
				else if (is_name_equals(attr, "width")) {
					read_to(attr, &obj->width);
				}
				else if (is_name_equals(attr, "height")) {
					read_to(attr, &obj->height);
				}
				else if (is_name_equals(attr, "rotation")) {
					read_to(attr, &obj->rotation);
				}
				else if (is_name_equals(attr, "gid")) {
					read_to(attr, &obj->tile_gid);
				}
				else if (is_name_equals(attr, "visible")) {
					read_to(attr, &obj->visible);
				}
			}

			///*
			xml_node<Ch>* currNode = objNode->first_node();

			while (currNode) {

				if (is_name_equals(currNode, "properties")) {
					this->fill_properties(currNode, obj->properties);
				}
				else if (is_name_equals(currNode, "ellipse")) {
					obj->shape = object_shapes::ellipse_t;
				}
				else if (is_name_equals(currNode, "polygon")) {
					obj->shape = object_shapes::polygon_t;
					fill_points(currNode, obj);
				}
				else if (is_name_equals(currNode, "polyline")) {
					obj->shape = object_shapes::polyline_t;
					fill_points(currNode, obj);
				}
				

				currNode = currNode->next_sibling();
			}
			//*/

			return true;
		}

		void fill_image(xml_node<Ch>* imageNode, shared_ptr<tileset_image> img) {

			for (xml_attribute<Ch>* attr = imageNode->first_attribute(); attr; attr = attr->next_attribute()) {

				if (is_name_equals(attr, "source")) {
					read_to(attr, img->source);

					if (img->source.size() > 0) {
						img->source.insert(0, "/");
						img->source.insert(0, this->file_path.first.c_str());
					}
				}
				else if (is_name_equals(attr, "trans")) {
					read_to(attr, img->transparency_color);
				}
				else if (is_name_equals(attr, "width")) {
					read_to(attr, &img->width);
				}
				else if (is_name_equals(attr, "height")) {
					read_to(attr, &img->height);
				}
			}
		}

		void fill_tile(xml_node<Ch>* tileNode, shared_ptr<tileset_tile> tile) {

			read_to(tileNode->first_attribute("id"), &tile->id);

			tile->probability = 1.0f;

			xml_node<Ch>* currNode = tileNode->first_node();

			while (currNode) {

				if (is_name_equals(currNode, "properties")) {
					this->fill_properties(currNode, tile->properties);
				}
				else if (is_name_equals(currNode, "objectgroup")) {
					tile->object_group = make_shared<object_group>();
					this->fill_objectgroup(currNode, tile->object_group);
				}
				else if (is_name_equals(currNode, "image")) {
					tile->image = make_shared<tileset_image>();
					this->fill_image(currNode, tile->image);
				}
				else if (is_name_equals(currNode, "animation")) {

					xml_node<Ch>* frameNode = currNode->first_node();

					while (frameNode != nullptr) {

						animation_frame af{};

						read_to(frameNode->first_attribute("tileid"), &af.tileid);
						read_to(frameNode->first_attribute("duration"), &af.duration);

						tile->animation.push_back(af);

						frameNode = frameNode->next_sibling();
					}
				}

				
				currNode = currNode->next_sibling();
			}
		}

		void fill_points(xml_node<Ch>* polyNode, shared_ptr<object> obj) {

			string points;

			read_to(polyNode->first_attribute("points"), points);

			obj->points = make_shared<vector<point>>();

			istringstream iss(points);
			string vpair;

			while (std::getline(iss, vpair, ' ')) {
				point p{};

				p.x = obj->position.x + stof(vpair.substr(0, vpair.find_first_of(',')));
				p.y = obj->position.y + stof(vpair.substr(vpair.find_first_of(',') + 1));

				obj->points->push_back(p);
			}

			//A lista de pontos exige a finalização do poligono com um último ponto, exatamente
			//o ponto de origem
			if (obj->points->size() > 1) {
				obj->points->push_back(obj->points->at(0));
			}
		}

	private:

		inline void read_to(xml_attribute<Ch>* attr, string& s, bool toUpper = false)
		{
			s.assign(attr->value(), attr->value_size());

			if (toUpper)
				transform(s.begin(), s.end(), s.begin(), toupper);
		}

		inline void read_to(xml_attribute<Ch>* attr, int* i)
		{
			(*i) = atoi(attr->value());
		}

		inline void read_to(xml_attribute<Ch>* attr, size_t* i)
		{
			(*i) = (size_t)atoi(attr->value());
		}

		inline void read_to(xml_attribute<Ch>* attr, bool* b)
		{
			(*b) = atoi(attr->value()) == 1;
		}

		inline void read_to(xml_attribute<Ch>* attr, float* v)
		{
			(*v) = atof(attr->value());
		}

		inline void read_to(xml_attribute<Ch>* attr, color& c) {
			
			c.hex_string.assign(attr->value());
			
			int hexNumber;

			sscanf_s(c.hex_string.c_str(), "%x", &hexNumber);

			c.red = (int)(hexNumber % 0x1000000 / 0x10000);
			c.green = (int)(hexNumber % 0x10000 / 0x100);
			c.blue = (int)(hexNumber % 0x100);
		}

		inline bool is_name_equals(xml_base<Ch>* attr, const char* name)
		{
			return strcmp(attr->name(), name) == 0;
		}

		inline bool is_value_equals(xml_base<Ch>* attr, const char* name)
		{
			return strcmp(attr->value(), name) == 0;
		}
	};
}

#endif // !__map__