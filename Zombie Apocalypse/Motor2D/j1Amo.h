#ifndef __j1ORB_H__
#define __j1ORB_H__

#include "j1Entity.h"

struct SDL_Texture;
struct Collider;
struct PathInfo;

struct AmoData {
	
	Animation* glow = nullptr;

	p2SString folder = nullptr;
	p2SString Texture = nullptr;

	SDL_Rect		OrbRect = { 0,0,0,0 };
	iPoint			printingoffset = { 0,0 };

	float			colliding_offset = 0;
	float			animationspeed = 0;
	
	int				areaofaction = 0;
	int				orbID = 0;
	int				orbID2 = 0;
	int				orbID3 = 0;
};

class j1Amo :public j1Entity
{
public:

	j1Amo();
	~j1Amo();

	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	// Called each loop iteration
	void FixedUpdate(float dt);

	// Called each logic iteration
	void LogicUpdate(float dt);


	void OnCollision(Collider* c1, Collider* c2);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// --- Pathfinding ---


	void UpdateMovement(float dt);

	// ----------------------

public:

	AmoData Amoinfo;
	bool touched = false;
};

#endif // __j1ORB_H__