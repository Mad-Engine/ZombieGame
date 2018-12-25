#include "j1Bat.h"
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


j1Bat::j1Bat() : j1Entity("Bat", entity_type::BAT)
{

	last_pathfinding = nullptr;
	current_path.Clear();
}

j1Bat::~j1Bat()
{

}

bool j1Bat::Start()
{
	LOG("Loading Bat");
	
	BatInfo = manager->GetBatData();

	entitycollrect = BatInfo.BatRect;
	colliding_offset = BatInfo.colliding_offset;
	entitycoll = App->coll->AddCollider(entitycollrect, COLLIDER_TYPE::COLLIDER_ENEMY_BAT, (j1Module*)manager);

	CurrentAnimation = BatInfo.flyRight;
	BatInfo.flyLeft->speed = BatInfo.animationspeed/2.0f;
	BatInfo.flyRight->speed = BatInfo.animationspeed/2.0f;
 	BatInfo.explote->speed = BatInfo.animationspeed;


	gravity = BatInfo.gravity;

	position.x = NULL;
	position.y = NULL;

	entitystate = FLYING;

	going_right = true;
	going_left = false;
	going_down = false;
	going_up = false;
	
	dead = false;

	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(BatInfo.Texture.GetString());

	entityID = App->entities->entityID;

	active = true;

	return true;
}

bool j1Bat::Update(float dt)
{
	going_down = false;
	going_up = false;
	
	batcolliding = false;
	entitystate = FLYING;

	return true;
}

bool j1Bat::PostUpdate(float dt)
{
	bool ret = true;

	if (active && entitycoll!=nullptr)
	{ 
		if ((position.x)*App->win->GetScale() >= -App->render->camera.x && (position.x)*App->win->GetScale() <= -App->render->camera.x + App->render->camera.w)
	{
		//check for player nearby

		if (!App->scene->player->god_mode &&
			App->scene->player->Future_position.x > position.x - BatInfo.areaofaction &&
			App->scene->player->Future_position.x < position.x + BatInfo.areaofaction &&
			App->scene->player->Future_position.y < position.y + BatInfo.areaofaction &&
			App->scene->player->Future_position.y > position.y - BatInfo.areaofaction)
		{
		
			
			CreatePathfinding({ (int)App->scene->player->Future_position.x, (int)App->scene->player->Future_position.y });

			Pathfind(dt);

		}

		//Debug Purpose (moving bat around)
		/*if (App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
		{
			position.x -= BatInfo.Velocity.x/10;
			going_right=true;
		}
		if (App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT)
		{
			position.x += BatInfo.Velocity.x/10;
			going_right = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT)
		{
			position.y -= BatInfo.Velocity.y/10;
			going_up = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT)
		{
			position.y += BatInfo.Velocity.y/10;
			going_down = true;
		}*/

		else
		{
			if (BatInfo.Velocity != BatInfo.auxVel)
			{
				BatInfo.Velocity = BatInfo.auxVel;
			}

			if (going_right)
			{
				position.x += BatInfo.Velocity.x*dt;
			}
			else if (!going_right)
			{
				position.x -= BatInfo.Velocity.x*dt;

			}

			if (going_up)
			{
				position.y += BatInfo.Velocity.y*dt;

			}
			else if (going_down)
			{
				position.y -= BatInfo.Velocity.y*dt;

			}

		}

		if (going_right)
			CurrentAnimation = BatInfo.flyRight;
		else if (!going_right)
			CurrentAnimation = BatInfo.flyLeft;

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

	//Blitting bat

	if (active)
	{
		App->render->Blit(spritesheet, position.x - BatInfo.printingoffset.x, position.y - BatInfo.printingoffset.y, &CurrentAnimation->GetCurrentFrame(dt));
	}

	if (!active && !CurrentAnimation->Finished())
	{
		App->render->Blit(spritesheet, position.x - BatInfo.printingoffset.x*3, position.y - BatInfo.printingoffset.y, &CurrentAnimation->GetCurrentFrame(dt));
	}
	return ret;
}

void j1Bat::OnCollision(Collider * c1, Collider * c2)
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
		if (c2->type == COLLIDER_TYPE::COLLIDER_FLOOR || c2->type == COLLIDER_TYPE::COLLIDER_PLATFORM || c2->type == COLLIDER_TYPE::COLLIDER_ROOF || c2->type == COLLIDER_TYPE::COLLIDER_SPIKES && dead == false && !lateralcollision)
		{
			if (/*going_up &&*/ c2->rect.y + c2->rect.h == c1->rect.y)
			{
				c1->rect.y += BatInfo.colliding_offset;
				going_up = false;
			}
			else if (/*going_down &&*/ c1->rect.y + c1->rect.h == c2->rect.y)
			{

				c1->rect.y -= BatInfo.colliding_offset;
				going_down = false;
			}

			batcolliding = true;
		}

		if (lateralcollision)
		{
			if (going_right)
			{
				going_right = false;
				c1->rect.x -= BatInfo.colliding_offset;
			}
			else
			{
				going_right = true;
				c1->rect.x += BatInfo.colliding_offset;

			}
			batcolliding = true;
		}
	
		position.x = c1->rect.x;
		position.y = c1->rect.y;
	}

	if (active)
	{

		if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER && !lateralcollision)
		{
			if (dead == false)
			{
				App->audio->PlayFx(App->audio->enemydeathfx);

				BatInfo.explote->Reset();
				CurrentAnimation = BatInfo.explote;

				//score here
				App->scene->player->score += 350;

				// entity dead
				if (entitycoll != nullptr)
				{
					entitycoll->to_delete = true;

				}

				dead = true;
				active = false;
			}
		}
		else if (c1->type == COLLIDER_TYPE::COLLIDER_PLAYER || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER && lateralcollision && App->scene->player->dead == false && !dead)
		{ 
			// -- player death ---
			
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
			App->scene->player->playerinfo.deathRight->Reset();
			App->scene->player->CurrentAnimation = App->scene->player->playerinfo.deathRight;
			App->scene->player->lifes -= 1;
			LOG("now lifes. %i", App->scene->player->lifes);
			App->scene->player->dead = true;
		}
	}
}

bool j1Bat::ReestablishVariables()
{
	bool ret = true;

	pathfinding_size = 0;

	return ret;
}

bool j1Bat::CreatePathfinding(const iPoint destination)
{
	bool ret = false;

	if (App->scene->firstStage == true)
	{
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
	}
	else
	{
		if (App->pathfinding->CreatePath(App->map->WorldToMap(position.x, position.y, App->map->data2), App->map->WorldToMap(destination.x, destination.y,App->map->data2)))
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
	}

	return ret;
}

bool j1Bat::Pathfind(float dt)
{
	bool ret = true;

	if (pathfinding_size > 1) {
		if (App->scene->firstStage == true)
		{
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
		{
			iPoint next_node = App->map->MapToWorld(current_path[pathfinding_index].x, current_path[pathfinding_index].y, App->map->data2);
			UpdateMovement(dt);

			if (App->map->WorldToMap(position.x, position.y, App->map->data2) == App->map->WorldToMap(next_node.x, next_node.y, App->map->data2)) {
				if (pathfinding_index < pathfinding_size - 1)
					pathfinding_index++;
			}
			if (App->map->WorldToMap(position.x, position.y, App->map->data2) == current_path[pathfinding_size - 1])
				ret = false;
		}
	}
	else
		ret = false;

	return ret;
}

void j1Bat::UpdateMovement(float dt)
{
	BatInfo.Velocity.x = current_path[pathfinding_index].x - App->map->WorldToMap(position.x, position.y,App->map->data).x;
	BatInfo.Velocity.y = current_path[pathfinding_index].y - App->map->WorldToMap(position.x, position.y, App->map->data).y;

	BatInfo.Velocity.x = BatInfo.Velocity.x*50.0f * dt;
	BatInfo.Velocity.y = BatInfo.Velocity.y*50.0f * dt;
	position.x += BatInfo.Velocity.x;
	position.y += BatInfo.Velocity.y;
}

bool j1Bat::Load(pugi::xml_node &config)
{
	bool ret = true;
	if (entityID == BatInfo.RefID.x)
	{
		position.x = config.child("Entity2").child("Batx").attribute("value").as_float();
		position.y = config.child("Entity2").child("Baty").attribute("value").as_float();
		active = config.child("Entity2").child("active").attribute("value").as_bool();
	}
	else if (entityID == BatInfo.RefID.y)
	{
		position.x = config.child("Entity3").child("Batx").attribute("value").as_float();
		position.y = config.child("Entity3").child("Baty").attribute("value").as_float();
		active = config.child("Entity3").child("active").attribute("value").as_bool();

	}

	return ret;
}

bool j1Bat::Save(pugi::xml_node &config) const
{
	if (entityID == BatInfo.RefID.x)
	{
		config.append_child("Entity2").append_child("Batx").append_attribute("value") = position.x;
		config.child("Entity2").append_child("Baty").append_attribute("value") = position.y;
		config.child("Entity2").append_child("active").append_attribute("value") = active;
	}
	else if (entityID == BatInfo.RefID.y)
	{
		config.append_child("Entity3").append_child("Batx").append_attribute("value") = position.x;
		config.child("Entity3").append_child("Baty").append_attribute("value") = position.y;
		config.child("Entity3").append_child("active").append_attribute("value") = active;
	}
	return true;
}

bool j1Bat::CleanUp()
{
	delete path_info;

	bool ret = true;
	App->tex->UnLoad(spritesheet);

	if (entitycoll != nullptr)
		entitycoll = nullptr;

	return ret;
}

void j1Bat::FixedUpdate(float dt)
{
	PostUpdate(dt);
}

void j1Bat::LogicUpdate(float dt)
{
	Update(dt);

	// --- Set batpos, prevent surpassing colliders ---
	entitycoll->SetPos(position.x, position.y);
	App->coll->QueryCollisions(*entitycoll);
}
