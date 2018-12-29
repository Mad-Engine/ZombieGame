// ----------------------------------------------------
// Controls all logic entities (enemies/player/etc.) --
// ----------------------------------------------------

#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1EntityManager.h"
#include "Brofiler/Brofiler.h"

j1EntityManager::j1EntityManager() : j1Module()
{
	name.create("entities");
}

// Destructor
j1EntityManager::~j1EntityManager()
{}


// Called before render is available
bool j1EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Setting up Entity manager");
	bool ret = true;
	//logic_updates_per_second = DEFAULT_LOGIC_PER_SECOND;
	//update_ms_cycle = 1.0f / (float)logic_updates_per_second;
    update_ms_cycle = 1.0f / (float)App->framerate_cap;

	LOG("Loading Player Parser");

	pugi::xml_node playernode = config.child("player");

	// --- Player Awake ---
	playerinfo.folder.create(playernode.child("folder").child_value());
	playerinfo.Texture.create(playernode.child("texture").child_value());

	//zombie Game
	playerinfo.Walkknife= LoadAnimation(playerinfo.folder.GetString(), "knife walk");
	playerinfo.WalkShotgun= LoadAnimation(playerinfo.folder.GetString(), "shotgun walk");
	playerinfo.WalkGun = LoadAnimation(playerinfo.folder.GetString(),"gun walk");
	playerinfo.WalkFlame = LoadAnimation(playerinfo.folder.GetString(), "flame walk");

	playerinfo.AttackKnife= LoadAnimation(playerinfo.folder.GetString(), "knife");
	playerinfo.AttackShotgun= LoadAnimation(playerinfo.folder.GetString(), "shotgun");
	playerinfo.AttackGun = LoadAnimation(playerinfo.folder.GetString(), "gun");
	playerinfo.AttackFlame= playerinfo.AttackShotgun = LoadAnimation(playerinfo.folder.GetString(), "flame thrower");
	//.............................................

	int x = playernode.child("collider").attribute("x").as_int();
	int y = playernode.child("collider").attribute("y").as_int();
	int w = playernode.child("collider").attribute("width").as_int();
	int h = playernode.child("collider").attribute("height").as_int();
	playerinfo.playerrect = { x,y,w,h };

	// --- Player main variables ---

	playerinfo.Velocity.x = playernode.child("Velocity").attribute("x").as_float();
	playerinfo.Velocity.y = playernode.child("Velocity").attribute("y").as_float();
	playerinfo.gravity = playernode.child("gravity").attribute("value").as_float();
	playerinfo.jump_force = playernode.child("Velocity").attribute("jump_force").as_float();



	/*playerinfo.deathRight->loop = false;
	playerinfo.deathLeft->loop = false;
	*/


	//zombie Game
	playerinfo.WalkGun->speed = 10.0f;
	playerinfo.AttackGun->speed = 10.0f;


	//--Slime data load ------------

	pugi::xml_node slimenode = config.child("slime");

	slimeinfo.folder.create(slimenode.child("folder").child_value());
	slimeinfo.Texture.create(slimenode.child("texture").child_value());

	x = slimenode.child("collider").attribute("x").as_int();
	y = slimenode.child("collider").attribute("y").as_int();
	w = slimenode.child("collider").attribute("width").as_int();
	h = slimenode.child("collider").attribute("height").as_int();
	slimeinfo.SlimeRect = { x,y,w,h };


	slimeinfo.runRight= LoadAnimation(slimeinfo.folder.GetString(), "slime right");
	slimeinfo.runLeft = LoadAnimation(slimeinfo.folder.GetString(), "slime left");
	slimeinfo.dead = LoadAnimation(slimeinfo.folder.GetString(), "dead");

	slimeinfo.gravity = slimenode.child("gravity").attribute("value").as_float();
	slimeinfo.Velocity.x = slimeinfo.auxVel.x = slimenode.child("Velocity").attribute("x").as_float();
	slimeinfo.Velocity.y = slimeinfo.auxVel.y = slimenode.child("Velocity").attribute("y").as_float();
	slimeinfo.initialVx = slimenode.child("Velocity").attribute("initalVx").as_float();
	slimeinfo.colliding_offset = slimenode.child("colliding_offset").attribute("value").as_float();
	slimeinfo.areaofaction = slimenode.child("areaofaction").attribute("value").as_int();
	slimeinfo.animationspeed = slimenode.child("animationspeed").attribute("value").as_float();
	slimeinfo.printingoffset.x = slimenode.child("printingoffset").attribute("x").as_int();
	slimeinfo.printingoffset.y = slimenode.child("printingoffset").attribute("y").as_int();
	slimeinfo.RefID.x = slimenode.child("entityID").attribute("value1").as_int();
	slimeinfo.RefID.y = slimenode.child("entityID").attribute("value2").as_int();

	//--- ZOMBIE data load --------------------

	pugi::xml_node zombienode = config.child("zombie");

	zombieinfo.folder.create(zombienode.child("folder").child_value());
	zombieinfo.Texture.create(zombienode.child("texture").child_value());

	x = zombienode.child("collider").attribute("x").as_int();
	y = zombienode.child("collider").attribute("y").as_int();
	w = zombienode.child("collider").attribute("width").as_int();
	h = zombienode.child("collider").attribute("height").as_int();
	zombieinfo.BatRect = { x,y,w,h };


	zombieinfo.walk = LoadAnimation(zombieinfo.folder.GetString(), "walk");
	zombieinfo.attack = LoadAnimation(zombieinfo.folder.GetString(), "attack");
	zombieinfo.dead = LoadAnimation(zombieinfo.folder.GetString(), "Dead");

	zombieinfo.Velocity.x = zombieinfo.auxVel.x = zombienode.child("Velocity").attribute("x").as_float();
	zombieinfo.Velocity.y = zombieinfo.auxVel.y = zombienode.child("Velocity").attribute("y").as_float();
	zombieinfo.initialVx = zombienode.child("Velocity").attribute("initalVx").as_float();
	zombieinfo.colliding_offset = zombienode.child("colliding_offset").attribute("value").as_float();
	zombieinfo.areaofaction = zombienode.child("areaofaction").attribute("value").as_int();
	zombieinfo.animationspeed = zombienode.child("animationspeed").attribute("value").as_float();
	zombieinfo.printingoffset.x = zombienode.child("printingoffset").attribute("x").as_int();
	zombieinfo.printingoffset.y = zombienode.child("printingoffset").attribute("y").as_int();
	zombieinfo.RefID.x = zombienode.child("entityID").attribute("value1").as_int();
	zombieinfo.RefID.y = zombienode.child("entityID").attribute("value2").as_int();

	//--- Orbs data load --------------------

	pugi::xml_node orbnode = config.child("orb");

	orbinfo.folder.create(orbnode.child("folder").child_value());
	orbinfo.Texture.create(orbnode.child("texture").child_value());

	x = orbnode.child("collider").attribute("x").as_int();
	y = orbnode.child("collider").attribute("y").as_int();
	w = orbnode.child("collider").attribute("width").as_int();
	h = orbnode.child("collider").attribute("height").as_int();
	orbinfo.OrbRect = { x,y,w,h };


	orbinfo.fly = LoadAnimation(orbinfo.folder.GetString(), "iddle");
	orbinfo.disappear = LoadAnimation(orbinfo.folder.GetString(), "disappear");
	orbinfo.appear = LoadAnimation(orbinfo.folder.GetString(), "appear");

	orbinfo.colliding_offset = orbnode.child("colliding_offset").attribute("value").as_float();
	orbinfo.areaofaction = orbnode.child("areaofaction").attribute("value").as_int();
	orbinfo.animationspeed = orbnode.child("animationspeed").attribute("value").as_float();
	orbinfo.printingoffset.x = orbnode.child("printingoffset").attribute("x").as_int();
	orbinfo.printingoffset.y = orbnode.child("printingoffset").attribute("y").as_int();
	orbinfo.orbID = orbnode.child("entityID").attribute("value1").as_int();
	orbinfo.orbID2 = orbnode.child("entityID").attribute("value2").as_int();
	orbinfo.orbID3 = orbnode.child("entityID").attribute("value3").as_int();

	// ---------------------

	return ret;
}

// Called before the first frame
bool j1EntityManager::Start()
{
	LOG("start j1EntityManager");
	bool ret = true;
	return ret;
}

// Called each loop iteration
bool j1EntityManager::PreUpdate()
{
	BROFILER_CATEGORY("EntityManager_Pre_Update", Profiler::Color::Chartreuse);

	//do_logic = false;
	return true;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManager_Update", Profiler::Color::Chocolate);

	//accumulated_time += dt;

	//if (accumulated_time >= update_ms_cycle)
	//{
	//	do_logic = true;
	//}

	if(dt<update_ms_cycle*1.25f && dt > 0.0f)
	UpdateEntity(dt);

	//if (do_logic == true)
	//{
	//	LOG("Did logic step after %f", accumulated_time);
	//	accumulated_time = 0.0f;
	//	do_logic = false;
	//}

	return true;
}

void j1EntityManager::UpdateEntity(float dt)
{
	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{

		/*if (do_logic == true)
		{*/
			entity->data->LogicUpdate(dt);
		//}

		entity = entity->next;
	}
}

bool j1EntityManager::PostUpdate(float dt)
{
	BROFILER_CATEGORY("EntityManager_Post_Update", Profiler::Color::Coral);

	p2List_item <j1Entity*> *entity = entities.start;
	
	while (entity != NULL )
	{
	    entity->data->FixedUpdate(dt);

	    entity = entity->next;
    }
	


	return true;
}

// Called before quitting
bool j1EntityManager::CleanUp()
{
	LOG("cleanup j1EntityManager");

	// release all entities
	p2List_item<j1Entity*>* entity = entities.start;

	while (entity != NULL)
	{
		entity->data->CleanUp();
		RELEASE(entity->data);
		entity = entity->next;
	}

	entities.clear();
	return true;
}

bool j1EntityManager::Load(pugi::xml_node &file)
{
	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{
		entity->data->Load(file);

		entity = entity->next;
	}


	return true;
}

bool j1EntityManager::Save(pugi::xml_node &file) const
{
	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{
		entity->data->Save(file);

		entity = entity->next;
	}

	return true;
}

// Create a new empty entity
j1Entity* const j1EntityManager::CreateEntity(const char* entname, entity_type entitytype)
{
	j1Entity* entity = nullptr;

	switch (entitytype)
	{
	case entity_type::SLIME:
		entity = new j1Slime();
		break;
	case entity_type::ZOMBIE_NORMAL:
		entity = new j1Zombie();
		break;
	case entity_type::PLAYER:
		entity = new j1Player();
		break;
	case entity_type::ORB:
		entity = new j1Orb();
		break;
	
	}
	entityID++;
	entity->Init(this);
	entity->Start();
	entities.add(entity);
	return(entity);
}

void j1EntityManager::DestroyEntity(j1Entity* entity)
{
	p2List_item <j1Entity*> *entity_item = entities.At(entities.find(entity));
	
	entities.del(entity_item);
	entityID--;

}

void j1EntityManager::OnCollision(Collider * c1, Collider * c2)
{
	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{
		if (entity->data->entitycoll == c1)
		{
			entity->data->OnCollision(c1, c2);
			break;
		}
		
	  entity = entity->next;
	}
}

Animation* j1EntityManager::LoadAnimation(const char* animationPath, const char* animationName) {

	Animation* animation = new Animation();

	bool anim = false;

	pugi::xml_document animationDocument;
	pugi::xml_parse_result result = animationDocument.load_file(animationPath);


	if (result == NULL)
	{
		LOG("Issue loading animation");
	}

	pugi::xml_node objgroup;
	for (objgroup = animationDocument.child("map").child("objectgroup"); objgroup; objgroup = objgroup.next_sibling("objectgroup"))
	{
		p2SString name = objgroup.attribute("name").as_string();
		if (name == animationName)
		{
			anim = true;
			int x, y, h, w;

			for (pugi::xml_node sprite = objgroup.child("object"); sprite; sprite = sprite.next_sibling("object"))
			{
				x = sprite.attribute("x").as_int();
				y = sprite.attribute("y").as_int();
				w = sprite.attribute("width").as_int();
				h = sprite.attribute("height").as_int();

				animation->PushBack({ x, y, w, h });
			}

		}
	}
	if (anim = true)
		return animation;
	else
		return nullptr;

}


