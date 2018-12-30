#include "j1Zombie.h"
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
#include "j1Pathfinding.h"
#include <time.h>

j1Zombie::j1Zombie() : j1Entity("zombie", entity_type::ZOMBIE_NORMAL)
{

	last_pathfinding = nullptr;
	current_path.Clear();
}

j1Zombie::~j1Zombie()
{

}

bool j1Zombie::Start()
{
	LOG("Loading zombie");
	srand(time(NULL));
	ZombieInfo = manager->GetBatData();

	entitycollrect = ZombieInfo.BatRect;
	colliding_offset = ZombieInfo.colliding_offset;
	entitycoll = App->coll->AddCollider(entitycollrect, COLLIDER_TYPE::COLLIDER_ENEMY_ZOMBIE, (j1Module*)manager);

	CurrentAnimation = ZombieInfo.walk;
	ZombieInfo.attack->speed = ZombieInfo.animationspeed/2.0f;
	ZombieInfo.walk->speed = ZombieInfo.animationspeed/2.0f;
 	ZombieInfo.dead->speed = ZombieInfo.animationspeed;

	gravity = ZombieInfo.gravity;

	position.x = 300;
	position.y = 300;

	Velocity.x = ZombieInfo.Velocity.x/App->entities->Secretboi;
	Velocity.y = Velocity.x;
	entitystate = LEFT;

	going_right = true;
	going_left = false;
	going_down = false;
	going_up = false;
	
	dead = false;

	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(ZombieInfo.Texture.GetString());

	entityID = App->entities->entityID;

	active = true;

	return true;
}

bool j1Zombie::Update(float dt)
{
	going_down = false;
	going_up = false;
	
	batcolliding = false;
	entitystate = LEFT;

	if (isClicked(entitycoll->rect))
	{
		lifes--;
	}

	if (lifes == 0)
	{
		int index = manager->entities.find(this);
		auxiliarpos.x = position.x;
		auxiliarpos.y = position.y;
		App->scene->zombie_kills++;

		if (index == 1)
		{
			position.x = App->map->data.enemy1.x;
			position.y = App->map->data.enemy1.y;
			
		}
		else if (index == 2)
		{
			position.x = App->map->data.enemy2.x;
			position.y = App->map->data.enemy2.y;
		}
		else if (index == 3)
		{
			position.x = App->map->data.enemy3.x;
			position.y = App->map->data.enemy3.y;
		}

		else if (index == 4)
		{
			position.x = App->map->data.enemy4.x;
			position.y = App->map->data.enemy4.y;
		}

		entitycoll->SetPos(position.x, position.y);
		lifes = 3;
		dropammo = true;
	}


	return true;
}

bool j1Zombie::isClicked(SDL_Rect & rect)
{
	LOG("x vs mouse_x: %i %i", rect.x, manager->click_pos.x);

	
	iPoint realpos = App->render->ScreenToWorld(manager->click_pos.x, manager->click_pos.y);

	return (rect.x < realpos.x &&
		rect.x + rect.w > realpos.x &&
		rect.y <  realpos.y  &&
		rect.h + rect.y >  realpos.y);

}


bool j1Zombie::PostUpdate(float dt)
{
	bool ret = true;

	if (active)
	{

		Pathfind(dt);
		CurrentAnimation = ZombieInfo.walk;

	}

	if (active && entitycoll != nullptr)
	{
		if (App->scene->player->Future_position.x > position.x)
		{
			going_right = true;

		}

		else if (App->scene->player->Future_position.x < position.x)
		{

			going_right = false;
		}

		if (App->scene->player->Future_position.y > position.y)
		{
			going_down = false;
			going_up = true;
		}

		else if (App->scene->player->Future_position.y < position.y)
		{

			going_down = true;
			going_up = false;
		}

		else if (App->scene->player->Future_position.x == position.y)
		{
			going_down = false;
			going_up = false;

		}




		if (dt > 0.000000)
		{
			if (going_right)
			{
				position.x += walking;

			}
			else if (!going_right)
			{
				position.x -= walking;

			}

			if (going_up)
			{
			position.y += walking;

			}
			else if (going_down)
			{
			position.y -= walking;

			}
		}

	
		if ((position.x)*App->win->GetScale() >= -App->render->camera.x && (position.x)*App->win->GetScale() <= -App->render->camera.x + App->render->camera.w)
		{
			//check for player nearby

			if (!App->scene->player->god_mode &&
				App->scene->player->Future_position.x > position.x - ZombieInfo.areaofaction &&
				App->scene->player->Future_position.x < position.x + ZombieInfo.areaofaction &&
				App->scene->player->Future_position.y < position.y + ZombieInfo.areaofaction &&
				App->scene->player->Future_position.y > position.y - ZombieInfo.areaofaction)
			{
				
				//CreatePathfinding({ (int)App->scene->player->Future_position.x, (int)App->scene->player->Future_position.y });



				CurrentAnimation = ZombieInfo.attack;
			}

				

			//Debug Purpose (moving zombie around)
			/*if (App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
			{
				position.x -= ZombieInfo.Velocity.x/10;
				going_right=true;
			}
			if (App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT)
			{
				position.x += ZombieInfo.Velocity.x/10;
				going_right = false;
			}
			if (App->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT)
			{
				position.y -= ZombieInfo.Velocity.y/10;
				going_up = true;
			}
			if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT)
			{
				position.y += ZombieInfo.Velocity.y/10;
				going_down = true;
			}*/

			/*else
			{
				if (ZombieInfo.Velocity != ZombieInfo.auxVel)
				{
					ZombieInfo.Velocity = ZombieInfo.auxVel;
				}

				if (going_right)
				{
					position.x += ZombieInfo.Velocity.x*dt;
				}
				else if (!going_right)
				{
					position.x -= ZombieInfo.Velocity.x*dt;

				}

				if (going_up)
				{
					position.y += ZombieInfo.Velocity.y*dt;

				}
				else if (going_down)
				{
					position.y -= ZombieInfo.Velocity.y*dt;

				}

			}*/

		/*	else
			{

				if (App->scene->player->Future_position.x >= position.x)
				{
					position.x += Velocity.x *5.0f* dt;
				}
				else if (App->scene->player->Future_position.x <= position.x)
				{
					position.x -= Velocity.x *5.0f* dt;
				}

				if (App->scene->player->Future_position.y >= position.y)
				{
					position.y += Velocity.x *5.0f* dt;
				}

				if (App->scene->player->Future_position.y >= position.y)
				{
					position.y -= Velocity.x *5.0f* dt;
				}

			}*/


		
			
	

		//check for limits
		if (position.x < 0)
		{
			position.x = 0;
			entitycoll->rect.x = 0;
		}
		else if (position.x > App->map->data.width*App->map->data.tile_width)
		{
			position.x = App->map->data.width*App->map->data.tile_width;
		}

		
		}
	}
	else if (!active && entitycoll != nullptr)
	{
		entitycoll->SetPos(-50, -50);
	}

	//Blitting zombie
	
	if (active)
	{
		App->render->Blit(spritesheet, position.x -50, position.y -25 , &CurrentAnimation->GetCurrentFrame(dt));
	}

	


	return ret;
}

void j1Zombie::OnCollision(Collider * c1, Collider * c2)
{
	bool lateralcollision = true;

	if (c1->rect.y + c1->rect.h == c2->rect.y || (c1->rect.y <= c2->rect.y+ c2->rect.h&& c1->rect.y+3 >= c2->rect.y + c2->rect.h))
	{
		lateralcollision = false;
	}

	if (c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h <= c2->rect.y + 2)
	{
		lateralcollision = false;
	}

	if (active)
	{
		if (c2->type == COLLIDER_TYPE::COLLIDER_FLOOR  && dead == false && !lateralcollision)
		{
			SDL_IntersectRect(&c1->rect, &c2->rect, &Intersection);

			if ( c2->rect.y + c2->rect.h >= c1->rect.y)
			{
				c1->rect.y += Intersection.h*2;
				going_up = false;
			}
			else if ( c1->rect.y + c1->rect.h <= c2->rect.y)
			{

				c1->rect.y -= Intersection.h*2;
				going_down = false;
			}

			batcolliding = true;
		}

		else if (lateralcollision)
		{
			SDL_IntersectRect(&c1->rect, &c2->rect, &Intersection);

			if (going_right)
			{
				
				c1->rect.x += Intersection.w*2;
			}
			else
			{
			
				c1->rect.x -= Intersection.w*2;
			}
			batcolliding = true;
		}
	
		position.x = c1->rect.x;
		position.y = c1->rect.y;
	}

	if (active)
	{
		if (nohit.ReadSec() >= 10 && stop == true)
		{
			stop = false;

		}
		else if (stop == false && c1->type == COLLIDER_TYPE::COLLIDER_PLAYER || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER && App->scene->player->dead == false && !dead)
		{ 
			// -- player death ---
			 nohit.Start();
			 stop = true;
			entitycoll->SetPos(-50, -50);
			

			if (going_right)
			{
				entitystate = LEFT;
				going_left = true;
				going_right = false;
				c2->rect.x = c2->rect.x + c1->rect.w * 2;
			}
			else
			{
				going_right = true;
				entitystate = RIGHT;
				going_left = false;
   				c2->rect.x = c2->rect.x - c1->rect.w * 2;
			}

			
			LOG("actual lifes. %i", App->scene->player->lifes);
			App->scene->player->playerinfo.Death->Reset();
			App->scene->player->CurrentAnimation = App->scene->player->playerinfo.Death;
			
			App->scene->player->dead = true;
		}
	}
}

bool j1Zombie::ReestablishVariables()
{
	bool ret = true;

	pathfinding_size = 0;
	return ret;
}

bool j1Zombie::CreatePathfinding(const iPoint destination)
{
	bool ret = false;

		if (App->pathfinding->CreatePath(App->map->WorldToMap(position.x, position.y, App->map->data), App->map->WorldToMap(destination.x, destination.y,App->map->data)))
		{
			last_pathfinding = App->pathfinding->GetLastPath();
			pathfinding_size = last_pathfinding->Count();
			pathfinding_index = 1;
			current_path.Clear();

			for (int i = 0; i < pathfinding_size; ++i) {
				current_path.PushBack(*last_pathfinding->At(i));
				ret = true;
			}
		}
	

	return ret;
}

bool j1Zombie::Pathfind(float dt)
{
	bool ret = true;

	if (pathfinding_size > 1) {
	
			iPoint next_node = App->map->MapToWorld(current_path[pathfinding_index].x, current_path[pathfinding_index].y,App->map->data);
			UpdateMovement(dt);

			if (App->map->WorldToMap(position.x, position.y,App->map->data) == App->map->WorldToMap(next_node.x, next_node.y,App->map->data)) {
				if (pathfinding_index < pathfinding_size - 1)
					pathfinding_index++;
			}
			if (App->map->WorldToMap(position.x, position.y,App->map->data) == current_path[pathfinding_size - 1])
				ret = false;
	}
	else
		ret = false;

	return ret;
}

void j1Zombie::UpdateMovement(float dt)
{
	ZombieInfo.Velocity.x = current_path[pathfinding_index].x - App->map->WorldToMap(position.x, position.y,App->map->data).x;
	ZombieInfo.Velocity.y = current_path[pathfinding_index].y - App->map->WorldToMap(position.x, position.y, App->map->data).y;

	ZombieInfo.Velocity.x = ZombieInfo.Velocity.x*150.0f * dt;
	ZombieInfo.Velocity.y = ZombieInfo.Velocity.y*150.0f * dt;
	position.x += ZombieInfo.Velocity.x;
	position.y += ZombieInfo.Velocity.y;
}

bool j1Zombie::Load(pugi::xml_node &config)
{
	bool ret = true;
	if (entityID == ZombieInfo.RefID.x)
	{
		position.x = config.child("Entity2").child("Batx").attribute("value").as_float();
		position.y = config.child("Entity2").child("Baty").attribute("value").as_float();
		active = config.child("Entity2").child("active").attribute("value").as_bool();
	}
	else if (entityID == ZombieInfo.RefID.y)
	{
		position.x = config.child("Entity3").child("Batx").attribute("value").as_float();
		position.y = config.child("Entity3").child("Baty").attribute("value").as_float();
		active = config.child("Entity3").child("active").attribute("value").as_bool();

	}

	return ret;
}

bool j1Zombie::Save(pugi::xml_node &config) const
{
	if (entityID == ZombieInfo.RefID.x)
	{
		config.append_child("Entity2").append_child("Batx").append_attribute("value") = position.x;
		config.child("Entity2").append_child("Baty").append_attribute("value") = position.y;
		config.child("Entity2").append_child("active").append_attribute("value") = active;
	}
	else if (entityID == ZombieInfo.RefID.y)
	{
		config.append_child("Entity3").append_child("Batx").append_attribute("value") = position.x;
		config.child("Entity3").append_child("Baty").append_attribute("value") = position.y;
		config.child("Entity3").append_child("active").append_attribute("value") = active;
	}
	return true;
}

bool j1Zombie::CleanUp()
{
	delete path_info;

	bool ret = true;
	App->tex->UnLoad(spritesheet);

	if (entitycoll != nullptr)
		entitycoll = nullptr;

	return ret;
}

void j1Zombie::FixedUpdate(float dt)
{
	PostUpdate(dt);
}

void j1Zombie::LogicUpdate(float dt)
{
	Update(dt);

	// --- Set batpos, prevent surpassing colliders ---
	entitycoll->SetPos(position.x, position.y);
	App->coll->QueryCollisions(*entitycoll);
}
