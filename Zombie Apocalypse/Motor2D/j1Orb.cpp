#include "j1Orb.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1Audio.h"
#include "j1EntityManager.h"
#include "j1Player.h"


j1Orb::j1Orb() : j1Entity("Orb", entity_type::ORB)
{

}

j1Orb::~j1Orb()
{

}

bool j1Orb::Start()
{
	LOG("Loading Slime");

	Orbinfo = manager->GetOrbData();

	entitycollrect = Orbinfo.OrbRect;
	colliding_offset = Orbinfo.colliding_offset;

	entitycoll = App->coll->AddCollider(entitycollrect, COLLIDER_TYPE::COLLIDER_ORB, (j1Module*)manager);

	touched = false;
	

	CurrentAnimation = Orbinfo.fly;
	Orbinfo.fly->speed = Orbinfo.animationspeed;
	Orbinfo.disappear->loop = false;
	Orbinfo.disappear->speed = Orbinfo.animationspeed;

	

	position.x = -1;
	position.y = -1;

	entitycoll->SetPos(position.x, position.y);

	entitystate = IDLE;


	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(Orbinfo.Texture.GetString());

	entityID = App->entities->entityID;

	active = true;

	return true;
}

bool j1Orb::Update(float dt)
{
	if (active && entitycoll != nullptr)
	{
		entitycoll->SetPos(position.x, position.y); 
		CurrentAnimation = Orbinfo.fly;
	}
	else if (!active && entitycoll != nullptr)
	{
		entitycoll->SetPos(-10, -10);
	}



	return true;
}

bool j1Orb::PostUpdate(float dt)
{
	bool ret = true;

	
	//Blitting orb
	App->render->Blit(spritesheet, position.x, position.y, &CurrentAnimation->GetCurrentFrame(dt));
	


	return ret;
}



void j1Orb::OnCollision(Collider * c1, Collider * c2)
{
	bool lateralcollision = true;

	if (active)
	{
		if (c2->type == COLLIDER_TYPE::COLLIDER_ORB || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER)
		{
			if (touched == false)
			{
				App->audio->PlayFx(App->audio->orbfx);
				App->scene->player->score += 1000;
				App->scene->player->orbs_number += 1;
				if (entitycoll != nullptr)
				{
					entitycoll->to_delete = true;
					CurrentAnimation = Orbinfo.disappear;
				}

				touched = true;
				active = false;
			}
		}
	}

	

}


void j1Orb::UpdateMovement(float dt)
{

}



bool j1Orb::Load(pugi::xml_node &config)
{
	bool ret = true;

	if (entityID == Orbinfo.orbID)
	{
		touched = config.child("Orb1").child("touched").attribute("value").as_bool();
		active =  config.child("Orb1").child("active").attribute("value").as_bool();
	}
	else if (entityID == Orbinfo.orbID2)
	{
		touched = config.child("Orb2").child("touched").attribute("value").as_bool();
		active =  config.child("Orb2").child("active").attribute("value").as_bool();
	}
	else if (entityID == Orbinfo.orbID3)
	{
		touched = config.child("Orb3").child("touched").attribute("value").as_bool();
		active =  config.child("Orb3").child("active").attribute("value").as_bool();
	}



	return ret;
}

bool j1Orb::Save(pugi::xml_node &config) const
{
	if (entityID == Orbinfo.orbID)
	{
		config.append_child("Orb1").append_child("touched").append_attribute("value") = touched;
		config.child("Orb1").append_child("active").append_attribute("value") = active;
		
	}
	else if (entityID == Orbinfo.orbID2)
	{
		config.append_child("Orb2").append_child("touched").append_attribute("value") = touched;
		config.child("Orb2").append_child("active").append_attribute("value") = active;

	}
	else if (entityID == Orbinfo.orbID3)
	{
		config.append_child("Orb3").append_child("touched").append_attribute("value") = touched;
		config.child("Orb3").append_child("active").append_attribute("value") = active;

	}

	return true;
}

bool j1Orb::CleanUp()
{
	bool ret = true;

	App->tex->UnLoad(spritesheet);

	if (entitycoll != nullptr)
		entitycoll = nullptr;

	return ret;
}

void j1Orb::FixedUpdate(float dt)
{
	PostUpdate(dt);
}

void j1Orb::LogicUpdate(float dt)
{
	Update(dt);

}
