#ifndef __map__
#define __map__

#include <string>
#include <algorithm>
#include <sstream>
#include <map>
#include <memory>
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
		string background_color;

		//�ltima mensagem de erro
		string error;

	public:

		vector<tileset*> tilesets;

	public:

		std::map<string, tile_layer*>	tile_layers;
		std::map<string, object_group*> object_groups;
		std::map<string, image_layer*>	image_layers;
		
	public:
		map() {}
		~map() {
			
			//TODO: Implementacao logica de limpeza de recursos
			
			for (auto og : object_groups) {

				if (og.second->objects != nullptr) {

					for (auto o : *og.second->objects) {
						
						if (o->properties != nullptr) {

							for (auto prop : *o->properties) {
								delete prop;
							}

							delete o->properties;
						}
					}

					delete og.second->objects;
				}

				delete og.second;
			}

			for (auto tl : tile_layers) {

				if (tl.second->properties != nullptr) {
					for (auto prop : *tl.second->properties) {
						delete prop;
					}

					delete tl.second->properties;
				}

				if (tl.second->data != nullptr) {
					delete tl.second->data;
				}

				delete tl.second;
			}

			/*
			for (auto img : image_layers) {
				delete img.second;
			}

			for (auto ts : vector<tileset*> tilesets) {
				delete ts;
			}
			*/
		}

	public:

		bool load(const char* filepath) {
			
			using namespace std;

			if (filepath == nullptr || strlen(filepath) == 0) {
				error = "nenhum caminho de arquivo fornecido";
				return false;
			}
						
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
						this->fill_layer(mapChildNode);
					}
					//<objectgroup>
					else if (is_name_equals(mapChildNode, "objectgroup")) {
						auto objGrp = new object_group();

						this->fill_objectgroup(mapChildNode, objGrp);

						this->object_groups[objGrp->name] = objGrp;
					}
					//<imagelayer>
					else if (is_name_equals(mapChildNode, "imagelayer")) {
						auto imgLayer = new image_layer();

						this->fill_imagelayer(mapChildNode, imgLayer);

						this->image_layers[imgLayer->name] = imgLayer;
					}
					//<tileset>
					else if (is_name_equals(mapChildNode, "tileset")) {
						auto t = new tileset();

						this->fill_tileset(mapChildNode, t);

						this->tilesets.push_back(t);
					}
				

					mapChildNode = mapChildNode->next_sibling();
				} 
				

				//Valida��o simples
				return version.size() > 0 && width > 0 && height > 0;
			}
			else {
				error = "arquivo xml nao parece ser um arquivo tmx valido";
				return false;
			}
		}

	private:

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
					read_to(attr, background_color, true);
				}
			}

			return true;
		}

		bool fill_layer(xml_node<Ch>* layerNode) {

			auto lay = new tile_layer();

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

						//Se temos uma compress�o diferente, devemos extrair a lista de identificadores  adequadamente
						if (compression != nullptr)
						{
							//TODO
						}
						//Do contr�rio s� lemos o conte�do na forma de uma string simples
						else
							stringContent = layerNodeChild->value();

						//Temos conte�do para processar?
						if (stringContent != nullptr) {

							if (is_value_equals(encodingAttr, "csv"))
							{
								//Pr�-alocando uma lista suficiente para guardar todas as refer�ncias
								lay->data = new vector<int>(lay->width * lay->height);

								int pos = 0;
								char separator;
								stringstream iss(stringContent);

								while (!iss.eof()) {
									int id;
									iss >> id;

									if (!iss.fail()) {
										lay->data->at(pos++) = id;
										iss >> separator;
									}
								}
							}
						}
					}
					//No outro caso o conte�do � uma lista de tiles
					else
					{

					}

				}
				else if (is_name_equals(layerNodeChild, "properties")) {
					lay->properties = this->fill_properties(layerNodeChild);
				}

				layerNodeChild = layerNodeChild->next_sibling();

			} while (layerNodeChild);

			this->tile_layers[lay->name] = lay;

			return true;
		}

		bool fill_objectgroup(xml_node<Ch>* node, object_group* grp) {

			grp->opacity = 1.0f;
			grp->visible = true;

			for (xml_attribute<Ch>* attr = node->first_attribute(); attr; attr = attr->next_attribute()) {

				if (is_name_equals(attr, "name")) {
					read_to(attr, grp->name);
				}
				else if (is_name_equals(attr, "x") ) {
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
				else if (is_name_equals(attr, "draworder") ) {
					grp->draw_order = is_value_equals(attr, "index") ? object_group::draworder::index : object_group::draworder::topdown;
				}
				else if (is_name_equals(attr, "color") ) {
					read_to(attr, grp->color, true);
				}
			}

			xml_node<Ch>* currentNode = node->first_node();

			do
			{
				if (is_name_equals(currentNode, "properties"))
				{
					grp->properties = this->fill_properties(currentNode);
				}
				else if (is_name_equals(currentNode, "object")) {

					if (grp->objects == nullptr)
						grp->objects = new vector<object*>();

					auto obj = new object();

					this->fill_object(currentNode, obj);

					grp->objects->push_back(obj);
				}

				currentNode = currentNode->next_sibling();

			} while (currentNode);


			return false;
		}

		bool fill_imagelayer(xml_node<Ch>* node, image_layer* imgLayer) {
			
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

			while(currentNode) {

				if (is_name_equals(currentNode, "properties"))
				{
					imgLayer->properties = this->fill_properties(currentNode);
				}
				else if (is_name_equals(currentNode, "image")) {
					imgLayer->image = new tileset_image();
					this->fill_image(currentNode, imgLayer->image);
				}

				currentNode = currentNode->next_sibling();
			}

			return false;
		}

		bool fill_tileset(xml_node<Ch>* node, tileset* t) {
						
			for (xml_attribute<Ch>* attr = node->first_attribute(); attr; attr = attr->next_attribute()) {

				if (is_name_equals( attr, "firstgid")) {
					read_to(attr, &t->firstgid);
				}
				else if (is_name_equals(attr,  "source")) {
					read_to(attr, t->source);
				}
				else if (is_name_equals(attr, "name")) {
					read_to(attr, t->name);
				}
				else if (is_name_equals(attr, "tilewidth")) {
					read_to(attr, &t->tile_width);
				}
				else if (is_name_equals(attr, "tileheight") ) {
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
					read_to(attr, t->background_color, true);
				}
			}

			//Por padr�o, o mesmo node recebido � o node para ler elementos extras, mas pode mudar se um arquivo externo for apontado
			xml_node<Ch>* detailsNode = node;

			//Arquivo com detalhes sobre o tileset � externo
			if (t->source.size() > 0) {
				//TODO
			}

			xml_node<Ch>* currentNode = detailsNode->first_node();

			do
			{
				if (is_name_equals(currentNode, "tileoffset"))
				{
					read_to(currentNode->first_attribute("x"), &t->tile_offset.x);
					read_to(currentNode->first_attribute("y"), &t->tile_offset.y);
				}
				else if (is_name_equals(currentNode, "image"))
				{
					this->fill_image(currentNode, &t->image);
				}
				else if (is_name_equals(currentNode, "terraintypes"))
				{

				}
				else if (is_name_equals(currentNode, "tile"))
				{
					auto tile = new tileset_tile();

					this->fill_tile(currentNode, tile);

					t->tiles.push_back(tile);
				}

				currentNode = currentNode->next_sibling();

			} while (currentNode);
			
			return false;
		}

		vector<custom_property*>* fill_properties(xml_node<Ch>* propertiesNode) {

			auto result = new vector<custom_property*>();
			auto propChildNode = propertiesNode->first_node();

			while (propChildNode) {

				auto prop = new custom_property();
				prop->type = custom_property::types::string_type;

				for (xml_attribute<Ch>* attr = propChildNode->first_attribute(); attr; attr = attr->next_attribute()) {

					if (is_name_equals(attr, "name")) {
						read_to(attr, prop->name);
					}
					else if (is_name_equals(attr, "value")) {
						read_to(attr, prop->value);
					}
					else if (is_name_equals(attr, "type")) {
												
						if (is_value_equals(attr, "int")) {
							prop->type = custom_property::types::int_type;
						}
						else if (is_value_equals(attr, "float")) {
							prop->type = custom_property::types::float_type;
						}
						else if (is_value_equals(attr, "bool")) {
							prop->type = custom_property::types::bool_type;
						}
						else if (is_value_equals(attr, "color")) {
							prop->type = custom_property::types::color_type;
						}
						else if (is_value_equals(attr, "file")) {
							prop->type = custom_property::types::file_type;
						}
					}					
				}

				result->push_back(prop);

				propChildNode = propChildNode->next_sibling();
			}

			return result;
		}

		bool fill_object(xml_node<Ch>* objNode, object* obj) {

			obj->visible = true;
			obj->rotation = 0;

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
					read_to(attr, &obj->x);
				}
				else if (is_name_equals(attr, "y")) {
					read_to(attr, &obj->y);
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
					obj->properties = this->fill_properties(currNode);
				}

				currNode = currNode->next_sibling();
			}
			//*/

			return true;
		}

		void fill_image(xml_node<Ch>* imageNode, tileset_image* img) {

			for (xml_attribute<Ch>* attr = imageNode->first_attribute(); attr; attr = attr->next_attribute()) {

				if (is_name_equals(attr, "source")) {
					read_to(attr, img->source);
				}
				else if (is_name_equals(attr, "trans")) {
					read_to(attr, img->transparency_color, true);
				}
				else if (is_name_equals(attr, "width")) {
					read_to(attr, &img->width);
				}
				else if (is_name_equals(attr, "height")) {
					read_to(attr, &img->height);
				}
			}
		}

		void fill_tile(xml_node<Ch>* tileNode, tileset_tile* tile) {

			read_to(tileNode->first_attribute("id"), &tile->id);
						 			
			auto propsNode = tileNode->first_node("properties");

			if (propsNode != nullptr) {
				tile->properties = this->fill_properties(propsNode);
			}

			auto objgrpNode = tileNode->first_node("objectgroup");

			if (objgrpNode != nullptr) {
				tile->object_group = new object_group();
				this->fill_objectgroup(objgrpNode, tile->object_group);
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