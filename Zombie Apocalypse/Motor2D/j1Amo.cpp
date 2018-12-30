#include "j1Amo.h"
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


j1Amo::j1Amo() : j1Entity("ammo", entity_type::AMMO)
{

}

j1Amo::~j1Amo()
{

}

bool j1Amo::Start()
{
	LOG("Loading Slime");

	Amoinfo = manager->GetOrbData();

	entitycollrect = Amoinfo.OrbRect;
	colliding_offset = Amoinfo.colliding_offset;

	entitycoll = App->coll->AddCollider(entitycollrect, COLLIDER_TYPE::COLLIDER_AMMO, (j1Module*)manager);

	touched = false;
	

	CurrentAnimation = Amoinfo.glow;
	Amoinfo.glow->speed = Amoinfo.animationspeed;
	Amoinfo.glow->loop = true;
	Amoinfo.glow->speed = Amoinfo.animationspeed;


	entitycoll->SetPos(position.x, position.y);

	entitystate = IDLE;


	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(Amoinfo.Texture.GetString());

	entityID = App->entities->entityID;

	active = true;
	Timer4active.Start();

	return true;
}

bool j1Amo::Update(float dt)
{
	/*if (Timer4active.ReadSec() >= 60)
	{
		active
	}*/
	if (active && entitycoll != nullptr)
	{
		entitycoll->SetPos(position.x, position.y); 
		CurrentAnimation = Amoinfo.glow;
	}
	else if (!active && entitycoll != nullptr)
	{
		entitycoll->SetPos(-10, -10);
	}



	return true;
}

bool j1Amo::PostUpdate(float dt)
{
	bool ret = true;

	
	//Blitting ammo
	if (active)
	{
		App->render->Blit(spritesheet, position.x, position.y, &CurrentAnimation->GetCurrentFrame(dt));
	}
	


	return ret;
}



void j1Amo::OnCollision(Collider * c1, Collider * c2)
{

	if (active)
	{
		if (c2->type == COLLIDER_TYPE::COLLIDER_AMMO || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER)
		{
			
				App->audio->PlayFx(App->audio->orbfx);
				App->scene->player->score += 1000;
				
				entitycoll->SetPos(0, 0);
				CurrentAnimation = Amoinfo.glow;
				int extra = App->scene->SecretNumber();
				App->scene->player->ammo += extra + 1;
				
				active = false;
			
		}
	}

	

}


void j1Amo::UpdateMovement(float dt)
{

}



bool j1Amo::Load(pugi::xml_node &config)
{
	bool ret = true;

	if (entityID == Amoinfo.orbID)
	{
		touched = config.child("Orb1").child("touched").attribute("value").as_bool();
		active =  config.child("Orb1").child("active").attribute("value").as_bool();
	}
	else if (entityID == Amoinfo.orbID2)
	{
		touched = config.child("Orb2").child("touched").attribute("value").as_bool();
		active =  config.child("Orb2").child("active").attribute("value").as_bool();
	}
	else if (entityID == Amoinfo.orbID3)
	{
		touched = config.child("Orb3").child("touched").attribute("value").as_bool();
		active =  config.child("Orb3").child("active").attribute("value").as_bool();
	}



	return ret;
}

bool j1Amo::Save(pugi::xml_node &config) const
{
	if (entityID == Amoinfo.orbID)
	{
		config.append_child("Orb1").append_child("touched").append_attribute("value") = touched;
		config.child("Orb1").append_child("active").append_attribute("value") = active;
		
	}
	else if (entityID == Amoinfo.orbID2)
	{
		config.append_child("Orb2").append_child("touched").append_attribute("value") = touched;
		config.child("Orb2").append_child("active").append_attribute("value") = active;

	}
	else if (entityID == Amoinfo.orbID3)
	{
		config.append_child("Orb3").append_child("touched").append_attribute("value") = touched;
		config.child("Orb3").append_child("active").append_attribute("value") = active;

	}

	return true;
}

bool j1Amo::CleanUp()
{
	bool ret = true;

	App->tex->UnLoad(spritesheet);

	if (entitycoll != nullptr)
		entitycoll = nullptr;

	return ret;
}

void j1Amo::FixedUpdate(float dt)
{
	PostUpdate(dt);
}

void j1Amo::LogicUpdate(float dt)
{
	Update(dt);

}
