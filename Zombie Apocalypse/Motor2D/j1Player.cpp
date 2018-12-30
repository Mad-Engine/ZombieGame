#include "j1Player.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1EntityManager.h"
#include "j1Audio.h"

j1Player::j1Player() : j1Entity("player",entity_type::PLAYER)
{
}

j1Player::~j1Player()
{

}

bool j1Player::Start()
{
	LOG("--- Loading player ---");

	// --- Get every data loaded by Entity Manager ---
	playerinfo = manager->GetPlayerData();

	// --- Player's bounding box ---
	entitycollrect = playerinfo.playerrect;
	entitycoll = App->coll->AddCollider(entitycollrect,COLLIDER_TYPE::COLLIDER_PLAYER, (j1Module*) manager);

	// --- Current Player Position ---
	position.x = 300;
	position.y = 300;

	entitycoll->SetPos(position.x, position.y);

	Future_position.x = position.x;
	Future_position.y = position.y;

	// --- Currently playing Animation ---
	//CurrentAnimation = playerinfo.idleRight;
	
	// --- Entity Spritesheet ---
	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(playerinfo.Texture.GetString());

	// --- Entity ID for save purposes ---
	entityID = App->entities->entityID;
	
	// --- Entity Velocity ---
	Velocity.x = playerinfo.Velocity.x;
	Velocity.y = playerinfo.Velocity.y;

	// -- Player lifes ----
	lifes = 3;

	//-- active ----
	active = true;
	float testx;
	float testy;
	// -- score ---
	score = 0;
	totalscore = 0;

	memset(vec, 0, 360);

	int changing_value_x [90];

	int changing_value_y [90];

	memset(changing_value_x, 0, 90);
	memset(changing_value_y, 0, 90);

	for (int i = 0; i < 90; ++i)
	{
		changing_value_x[i] = i / 3.0f;

		changing_value_y[i] = i / 3.0f;
	}

	// 0 --- 90
	for (int i = 0; i < 90; ++i)
	{
		vec[i].x = changing_value_x[i];
		vec[i].y = -changing_value_y[i];
	}

	// 90 --- 180
	for (int j = 0; j < 90; ++j)
	{
		vec[90 + j].x = changing_value_x[89] - changing_value_x[j];
		vec[90 + j].y = -(changing_value_x[89] + changing_value_y[j]);
	}

	// 180 --- 270

	for (int z = 0; z < 90; ++z)
	{
		vec[180 + z].x = vec[179].x - changing_value_x[z];
		vec[180 + z].y = -(-vec[179].y - changing_value_y[z]);
	}

	// 270 --- 360

	for (int f = 0; f < 90; ++f)
	{
		vec[270 + f].x = - changing_value_x[89] + changing_value_x[f];
		vec[270 + f].y = - changing_value_y[89] + changing_value_y[f];
	}

	rot = 0;

	CurrentAnimation = playerinfo.WalkGun;

	deathtimer.Start();

	return true;
}

void j1Player::UpdateEntityMovement(float dt)
{

	switch (EntityMovement)
	{

	case MOVEMENT::RIGHTWARDS:
		//Accumulative_pos_Right += Velocity.x*dt;

		//if (Accumulative_pos_Right > 1.1)
		//{
		//	Future_position.x += Accumulative_pos_Right;
		//	Accumulative_pos_Right -= Accumulative_pos_Right;
		//}
		Future_position.x += Velocity.x * dt/1.5f;
		break;
	case MOVEMENT::LEFTWARDS:
		Future_position.x -= Velocity.x * dt/1.25f;

		/*Accumulative_pos_Left += Velocity.x*dt;

		if (on_air)
		{
			if (Accumulative_pos_Left > 1.0)
			{
				Future_position.x -= Accumulative_pos_Left;
				Future_position.x -= Accumulative_pos_Left;

				Accumulative_pos_Left -= Accumulative_pos_Left;
			}
		}

		if (Accumulative_pos_Left > 1.5)
		{
			Future_position.x -= Accumulative_pos_Left;
			Accumulative_pos_Left -= Accumulative_pos_Left;
		}*/

		break;
	case MOVEMENT::UPWARDS:

		Future_position.y -= Velocity.x*dt;
		/*Future_position.y -= Velocity.x*dt;
		Accumulative_pos_Up += Velocity.y*dt;

		if (Accumulative_pos_Up > 0.75)
		{
			Future_position.y -= Accumulative_pos_Up;
			Accumulative_pos_Up -= Accumulative_pos_Up;
		}*/
		break;
	case MOVEMENT::DOWNWARDS:

		Future_position.y += Velocity.x*2.0f*dt;
		/*Future_position.y += Velocity.x*dt;

		Accumulative_pos_Down += Velocity.y*dt;

		if (Accumulative_pos_Down > 1.0)
		{
			Future_position.y += Accumulative_pos_Down;
			Accumulative_pos_Down -= Accumulative_pos_Down;
		}*/

		break;
	}

	entitycoll->SetPos(Future_position.x, Future_position.y);

	App->coll->QueryCollisions(*entitycoll);

	MOVEMENT EntityMovement = MOVEMENT::STATIC;
}

void j1Player::God_Movement(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		Future_position.x += Velocity.x*3*dt;


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		Future_position.x -= Velocity.x*3*dt;


	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		Future_position.y -= Velocity.x*3.0f*dt;

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		Future_position.y += Velocity.x*3.0f*dt;

}

inline void j1Player::Apply_Vertical_Impulse(float dt)
{
	//Velocity.y += playerinfo.jump_force;
}

void j1Player::Handle_Ground_Animations()
{
	// --- Handling Ground Animations ---
	
		//--- TO RUN ---

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			/*if (CurrentAnimation == playerinfo.runRight)
				CurrentAnimation = playerinfo.idleRight;
			else if (CurrentAnimation == playerinfo.runLeft)
				CurrentAnimation = playerinfo.idleLeft;*/
		}
		else
		{

			/*if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT &&
				CurrentAnimation != playerinfo.jumpingRight      &&
				CurrentAnimation != playerinfo.fallingRight)
			{
				CurrentAnimation = playerinfo.runRight;
			}

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT &&
				CurrentAnimation != playerinfo.jumpingLeft       &&
				CurrentAnimation != playerinfo.fallingLeft)
			{
				CurrentAnimation = playerinfo.runLeft;
			}*/
		}

		//--- TO IDLE ---

    //--------------    ---------------

	//testing life system

	/*	if (App->input->GetKey(SDL_SCANCODE_H) == KEY_REPEAT)
		{
			playerinfo.deathRight->Reset();
			playerinfo.deathLeft->Reset();
			CurrentAnimation = playerinfo.deathRight;
			lifes -= 1;
			dead = true;
			score -= 100;
			
		}*/
		
		//if (no_timer.ReadSec() >= 5 && stop == true)
		//{
		//	stop = false;
		//}

		//if (stop == false)
		//{
		//	if (lifes == 0)
		//	{
		//		App->scene->Activate_MainMenu = true;
		//		App->scene->Activate_HUD = false;
		//	}

		//	if (lifes < 0 )
		//	{
		//		lifes = 0;
		//		dead = false;
		//		App->scene->change_scene(App->scene->StageList.start->data->GetString());
		//		help = true;
		//	}
		//	else if (dead == true)
		//	{
		//		dead = false;
		//		/*App->scene->player->lifes -= 1;*/
		//		LOG("now lifes. %i", App->scene->player->lifes);
		//		stop = true;
		//		no_timer.Start();
		//		//CurrentAnimation = playerinfo.idleRight;
		//	}
		//}
}


void j1Player::Handle_Aerial_Animations()
{
	// --- Handling Aerial Animations ---

	
		////--- TO JUMP ---

		//if (Velocity.y > playerinfo.jump_force / 2)
		//{
		//	if (CurrentAnimation == playerinfo.runRight || CurrentAnimation == playerinfo.idleRight)
		//		CurrentAnimation = playerinfo.jumpingRight;

		//	else if (CurrentAnimation == playerinfo.runLeft || CurrentAnimation == playerinfo.idleLeft)
		//		CurrentAnimation = playerinfo.jumpingLeft;
		//}

		////--- TO FALL ---

		//else if (Velocity.y < playerinfo.jump_force / 2)
		//{
		//	if (CurrentAnimation == playerinfo.jumpingRight || CurrentAnimation == playerinfo.runRight || CurrentAnimation == playerinfo.idleRight)
		//		CurrentAnimation = playerinfo.fallingRight;
		//	else if (CurrentAnimation == playerinfo.jumpingLeft || CurrentAnimation == playerinfo.runLeft || CurrentAnimation == playerinfo.idleLeft)
		//		CurrentAnimation = playerinfo.fallingLeft;
		//}


	//------------ --------------
}

bool j1Player::Update(float dt)
{
	// -- health

	if (lifes == 1 && help == true)
	{
		App->audio->PlayFx(App->audio->die);
		help = false;
	}
	// --- LOGIC --------------------

	//if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	//{
	//	if (!App->scene->Activate_MainMenu
	//		&& !App->scene->Activate_Ingamemenu
	//		&& !App->scene->Activate_Credits
	//		&& !App->scene->Activate_InGameSettings
	//		&& !App->scene->Activate_MainMenuSettings)
	//	{
	//		god_mode = !god_mode;
	//	}
	//}

	if (god_mode)
	{
		God_Movement(dt);
	}

	else
	{

		// --- RIGHT --
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT &&
			App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
		{
			EntityMovement = MOVEMENT::RIGHTWARDS;
		}

		if (EntityMovement != MOVEMENT::STATIC)
			UpdateEntityMovement(dt);

		// --- LEFT ---
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT &&
			App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
		{
			EntityMovement = MOVEMENT::LEFTWARDS;
		}

		if (EntityMovement != MOVEMENT::STATIC)
			UpdateEntityMovement(dt);

		// --- UP ---
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT &&
			App->input->GetKey(SDL_SCANCODE_S) != KEY_REPEAT)
		{
			EntityMovement = MOVEMENT::UPWARDS;
		}

		if (EntityMovement != MOVEMENT::STATIC)
			UpdateEntityMovement(dt);

		// --- DOWN ---
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT &&
			App->input->GetKey(SDL_SCANCODE_W) != KEY_REPEAT)
		{
			EntityMovement = MOVEMENT::DOWNWARDS;
		}

		if (EntityMovement != MOVEMENT::STATIC)
			UpdateEntityMovement(dt);
		//-------------------------------

		// --- Handling animations ---

		Handle_Ground_Animations();

		if (lifes == 0)
		{
			App->scene->change_scene(App->scene->StageList.start->data->GetString());
		}

	}

	return true;
}

bool j1Player::PostUpdate(float dt)
{
	bool ret = true;

	// --- Handling flip ---

	App->input->GetMousePosition(mousepos.x,mousepos.y);

	//mousepos.x = mousepos.x * 3;
	//mousepos.y = mousepos.y * 3;

	//Future_position.x = Future_position.x * 2.0f;
	//Future_position.y = Future_position.y * 2.0f;

	mousepos = App->render->ScreenToWorld(mousepos.x, mousepos.y);

	// ---------------------- //

	// --- Blitting player ---

	if ( App->input->GetMouseButtonDown(1) == KEY_DOWN
		&&  !App->scene->Activate_Ingamemenu 
	    &&	!App->scene->Activate_MainMenu
	    &&	!App->scene->Activate_InGameSettings
	    &&	!App->scene->Activate_MainMenuSettings
	    &&	!App->scene->Activate_Credits)
	{
		if ( ammo > 0)
		{
			CurrentAnimation = playerinfo.AttackGun;
			playerinfo.WalkGun->Reset();
			App->audio->PlayFx(App->audio->shotfx);
			ammo--;
		}
		
	}
	
	if (CurrentAnimation == playerinfo.AttackGun && CurrentAnimation->Finished())
	{
		CurrentAnimation = playerinfo.WalkGun;
		playerinfo.AttackGun->Reset();
	}


	float vec_x = mousepos.x - Future_position.x;
	float vec_y = mousepos.y - Future_position.y;

	if (mousepos.x != 0 && mousepos.y != 0)
	{

		if (mousepos.y >= Future_position.y + entitycoll->rect.h / 2 && mousepos.x >= Future_position.x + entitycoll->rect.w / 2)
		{
			rot = atan(((float)(vec_x) / (float)(vec_y)))*180.0f / 3.14f;

			//rot += 180;
		}

		else if (mousepos.y >= Future_position.y + entitycoll->rect.h / 2 && mousepos.x < Future_position.x + entitycoll->rect.w / 2)
		{

			rot = atan(((float)(vec_x) / (float)(vec_y)))*180.0f / 3.14f;

			//rot += 180;
		}

		else if (mousepos.y <= Future_position.y + entitycoll->rect.h / 2 && mousepos.x <= Future_position.x + entitycoll->rect.w / 2)
		{
			rot = atan(((float)(vec_y) / (float)(-vec_x)))*180.0f / 3.14f;

			rot += 270;
		}

		else if (mousepos.y <= Future_position.y + entitycoll->rect.h / 2 && mousepos.x >= Future_position.x + entitycoll->rect.w / 2)
		{
			rot = -atan(((float)(vec_y) / (float)(vec_x)))*180.0f / 3.14f;

			rot += 90;
		}

	}

	if (rot == 0 || rot == 360)
	{
		rot = 1;
	}
	
	diff_x = vec[(int)rot - 1].x;
	diff_y = vec[(int)rot - 1].y;

	//Future_position.x = Future_position.x / 2.0f;
	//Future_position.y = Future_position.y / 2.0f;

	App->render->Blit(spritesheet, Future_position.x - 40.0f + diff_x, Future_position.y - 10.0f + diff_y, &CurrentAnimation->GetCurrentFrame(dt),true,1.0f,-rot,flip);
	
	
	// ---------------------- //

	return ret;
}

void j1Player::OnCollision(Collider * entitycollider, Collider * to_check)
{
	if (!god_mode)
	{
		switch (EntityMovement)
		{
		case MOVEMENT::RIGHTWARDS:
			Right_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::LEFTWARDS:
			Left_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::UPWARDS:
			Up_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::DOWNWARDS:
			Down_Collision(entitycollider, to_check);
			break;

		}

		Future_position.x = entitycollider->rect.x;
		Future_position.y = entitycollider->rect.y;
	}
}

void j1Player::Right_Collision(Collider * entitycollider, const Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
		case COLLIDER_TYPE::COLLIDER_FLOOR:
			entitycollider->rect.x -= Intersection.w;
			App->render->camera.x = camera_pos_backup.x;
			break;
		case COLLIDER_TYPE::COLLIDER_PLATFORM:
			entitycollider->rect.x -= Intersection.w;
			App->render->camera.x = camera_pos_backup.x;
			break;
		case COLLIDER_TYPE::COLLIDER_ROOF:
			entitycollider->rect.x -= Intersection.w;
			App->render->camera.x = camera_pos_backup.x;
			break;
		case COLLIDER_TYPE::COLLIDER_ENEMY_ZOMBIE:

			if (deathtimer.ReadSec() > 5)
			{
				deathtimer.Start();
				lifes--;
			}
			break;
	}
}

void j1Player::Left_Collision(Collider * entitycollider, const Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
		case COLLIDER_TYPE::COLLIDER_FLOOR:
			entitycollider->rect.x += Intersection.w;
			App->render->camera.x = camera_pos_backup.x;
			break;
		case COLLIDER_TYPE::COLLIDER_PLATFORM:
			entitycollider->rect.x += Intersection.w;
			App->render->camera.x = camera_pos_backup.x;
			break;
		case COLLIDER_TYPE::COLLIDER_ROOF:
			entitycollider->rect.x += Intersection.w;
			App->render->camera.x = camera_pos_backup.x;
			break;
		case COLLIDER_TYPE::COLLIDER_ENEMY_ZOMBIE:

			if (deathtimer.ReadSec() > 5)
			{
				deathtimer.Start();
				lifes--;
			}
			break;
	}
}

void j1Player::Up_Collision(Collider * entitycollider, const Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	coll_up = true;

	switch (to_check->type)
	{
		case COLLIDER_TYPE::COLLIDER_FLOOR:
			entitycollider->rect.y += Intersection.h;
			break;
		case COLLIDER_TYPE::COLLIDER_PLATFORM:
			entitycollider->rect.y += Intersection.h;
			break;
		case COLLIDER_TYPE::COLLIDER_ROOF:
			entitycollider->rect.y += Intersection.h;
			break;
		case COLLIDER_TYPE::COLLIDER_ENEMY_ZOMBIE:

			if (deathtimer.ReadSec() > 5)
			{
				deathtimer.Start();
				lifes--;
			}
			break;
	}

	Velocity.y = 0.0f;
}

void j1Player::Down_Collision(Collider * entitycollider, const Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
		case COLLIDER_TYPE::COLLIDER_FLOOR:
			entitycollider->rect.y -= Intersection.h;
			break;
		case COLLIDER_TYPE::COLLIDER_PLATFORM:
			entitycollider->rect.y -= Intersection.h;
			break;
		case COLLIDER_TYPE::COLLIDER_ROOF:
			entitycollider->rect.y -= Intersection.h;
			break;
		case COLLIDER_TYPE::COLLIDER_ENEMY_ZOMBIE:

			if (deathtimer.ReadSec() > 5)
			{
				deathtimer.Start();
				lifes--;
			}
			break;
	
	}

	double_jump = false;
	coll_up = false;
	Velocity.y = 0.0f;
	on_air = false;
}

bool j1Player::Load(pugi::xml_node &config)
{
	Future_position.x= config.child("Player").child("Playerx").attribute("value").as_float();
	Future_position.y = config.child("Player").child("Playery").attribute("value").as_float();
	orbs_number = config.child("Player").child("orbs_number").attribute("value").as_int();
	score = config.child("Player").child("score").attribute("value").as_int();
	TimeAuxload =config.child("Player").child("time").attribute("value").as_uint();
	TimePausedSave = config.child("Player").child("timePaused").attribute("value").as_uint();
	//lifes = config.child("Player").child("life").attribute("value").as_int();

	//App->scene->sceneTimer.Loadstart(TimeAuxload);
	return true;
}

bool j1Player::Save(pugi::xml_node &config) const
{ 
	config.append_child("Player").append_child("Playerx").append_attribute("value")= Future_position.x;
	config.child("Player").append_child("Playery").append_attribute("value")= Future_position.y;
	config.child("Player").append_child("orbs_number").append_attribute("value") = orbs_number;
	config.child("Player").append_child("score").append_attribute("value") = score;
	//config.child("Player").append_child("life").append_attribute("value") = lifes;
	config.child("Player").append_child("time").append_attribute("value") = App->scene->sceneTimer.Read();
	config.child("Player").append_child("timePaused").append_attribute("value") = App->scene->sceneTimer.getPausd();

	return true;
}

bool j1Player::CleanUp()
{
	App->tex->UnLoad(spritesheet);

	if(entitycoll != nullptr)
	entitycoll = nullptr;

	return true;
}

void j1Player::FixedUpdate(float dt)
{
	// --- Update we do every frame ---

	PostUpdate(dt);

}

void j1Player::LogicUpdate(float dt)
{
	// --- Update we may not do every frame ---

	camera_pos_backup.x = App->render->camera.x;
	camera_pos_backup.y = App->render->camera.y;

	EntityMovement = MOVEMENT::STATIC;

	Update(dt);
}

