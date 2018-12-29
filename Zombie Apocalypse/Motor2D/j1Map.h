#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "SDL\include\SDL.h"

// ----------------------------------------------------


struct Properties
{
	p2List <p2SString*> name;

	p2List <p2SString*> value;

	bool LoadProperties(pugi::xml_node& node);

	int numproperties ;

	p2SString GetProperties(const char * request);

	~Properties()
	{
	}

};

//struct with the infornmation of the parallax layers

struct MapLayer
{
	p2SString	name;
	int			width;
	int			height;
	uint*		data;
	Properties	properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

// Tileset information ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
	
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	iPoint				initpos;
	iPoint				enemy1;
	iPoint				enemy2;
	iPoint				enemy3;
	iPoint				enemy4;
	iPoint				amo1;
	iPoint				ammo2;
	iPoint				ammo3;
	iPoint				finalpos;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	p2List<MapLayer*>	layers;
	Properties          properties;
};

// ----------------------------------------------------

class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw(MapData& data);

	// Called before quitting
	bool CleanUp();


	// Load new map
	bool Load(const char* path, MapData& Data);

	iPoint MapToWorld(int x, int y, MapData& Data) const;
	iPoint WorldToMap(int x, int y, MapData& Data) const;
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer, MapData& Data) const;

	bool ColliderDrawer(MapData& data);

	TileSet* GetTilesetFromTileId(int id, MapData& mapdata) const;

private:

	bool LoadMap(MapData& data);
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);


public:

	MapData data;
	MapData data2;
	int numberStages = 0;
	
	int					offset=0;

private:
	p2SString folder = nullptr;
	p2SString tmp = nullptr;
	pugi::xml_document	map_file;
	bool				map_loaded ;
	int					redCollision = NULL;
	int					redCollision2 = NULL;

	
};

#endif // __j1MAP_H__