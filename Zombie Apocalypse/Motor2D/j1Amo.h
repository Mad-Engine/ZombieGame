#ifndef __j1ORB_H__
#define __j1ORB_H__

#include "j1Entity.h"
#include "j1Timer.h"
#include "j1App.h"

struct SDL_Texture;
struct Collider;
struct PathInfo;

struct AmoData {
	
	Animation* Ammo_glow = nullptr;
	Animation* Health_glow = nullptr;
	Animation* Money_glow = nullptr;

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

	void SetType(int type);
	// --- Pathfinding ---


	void UpdateMovement(float dt);

	// ------Timer--------
	j1Timer Timer4active;
	j1Timer Timetocollect;
	int ControlVar_timetocollect = 0;
	p2SString cvarname_timecollect;
	p2SString cvarattr_timecollect;
	cvar cvartimetocollect;

public:

	AmoData Amoinfo;
	bool touched = false;
	int typeof = 1;
};

#endif // __j1ORB_H__