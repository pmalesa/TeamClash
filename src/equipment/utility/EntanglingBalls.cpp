#include "EntanglingBalls.h"

#include "../../player/Player.h"

#include <Area2D.hpp>
#include <Timer.hpp>
#include <AudioStreamPlayer.hpp>

#include <iostream>
using std::cout;
using std::endl;

using namespace godot;

void EntanglingBalls::_register_methods()
{
	register_method("_init", &EntanglingBalls::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &EntanglingBalls::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("init", &EntanglingBalls::init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &EntanglingBalls::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &EntanglingBalls::_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processImpact", &EntanglingBalls::processImpact, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processMovement", &EntanglingBalls::processMovement, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("collisionDetected", &EntanglingBalls::collisionDetected, GODOT_METHOD_RPC_MODE_DISABLED);

	register_property<EntanglingBalls, Vector2>("velocity_", &EntanglingBalls::velocity_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<EntanglingBalls, Vector2>("initialPosition_", &EntanglingBalls::initialPosition_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<EntanglingBalls, int64_t>("shooterNodeName_", &EntanglingBalls::shooterNodeName_, 0, GODOT_METHOD_RPC_MODE_DISABLED);
}

void EntanglingBalls::_init()
{
	objectHit_ = false;
	Godot::print("[ENTANGLING BALLS] Initialized.");
}

void EntanglingBalls::_ready()
{
	set_physics_process(true);
	static_cast<AudioStreamPlayer*>(get_node("ThrowSound"))->play();
	Godot::print("[ENTANGLING BALLS] Entangling balls thrown.");
}

void EntanglingBalls::init(int64_t shooterNodeName, Vector2 initialPosition, Vector2 initialDirection)
{
	set_position(initialPosition);
	set_rotation(-initialDirection.angle_to(Vector2(1, 0)));
	set("velocity_", Vector2(initialDirection.x * INITIAL_BALL_SPEED, initialDirection.y * INITIAL_BALL_SPEED));
	set("shooterNodeName_", shooterNodeName);
	set("initialPosition_", initialPosition);
	set("initialDirection_", initialDirection);
}

void EntanglingBalls::_physics_process(float delta)
{
	if ((get_position() - initialPosition_).length() > MAX_DISTANCE)
		queue_free();
	if (!collisionDetected())
		processMovement();
}

void EntanglingBalls::_process(float delta)
{
	if (!objectHit_ && collisionDetected())
		processImpact();
}


void EntanglingBalls::processImpact()
{
	Array overlapingBodies = static_cast<Area2D*>(get_node("Area"))->get_overlapping_bodies();
	if (overlapingBodies.empty())
		return;
	static_cast<AudioStreamPlayer*>(get_node("ThrowSound"))->set_volume_db(-20);
	for (unsigned int i = 0; i < overlapingBodies.size(); ++i)
	{
		Node* overlappedNode = static_cast<Node*>(overlapingBodies[i]);
		if (overlappedNode->is_in_group("Weapon"))
			return;
		if (overlappedNode->is_in_group("Player"))
		{
			Player* shotPlayer = static_cast<Player*>(overlappedNode);
			Variant shooterNodeName = shooterNodeName_;
			if (shotPlayer->get_name() != String(shooterNodeName))
			{
				Godot::print("[ENTANGLING BALLS] Collision detected / object hit.");
				if (shotPlayer->is_network_master())
					shotPlayer->inflictSlow(SLOW, SLOW_DURATION);

				velocity_ = Vector2(0, 0);
				queue_free();
			}
			else
				return;
		}
		objectHit_ = true;
		queue_free();
	}
}

void EntanglingBalls::processMovement()
{
	set_rotation(-velocity_.angle_to(Vector2(1, 0)));
	move_and_slide(velocity_, Vector2(0, -1));
}


bool EntanglingBalls::collisionDetected()
{
	return !static_cast<Area2D*>(get_node("Area"))->get_overlapping_bodies().empty();
}




































// processImpact(); Below's version of that function does not work properly, however I do not yet know why.

/*
Array overlapingBodies = static_cast<Area2D*>(get_node("BoltArea"))->get_overlapping_bodies();
Node* overlappedNode;
if (overlapingBodies.empty())
	return;
else
	overlappedNode = static_cast<Node*>(overlapingBodies.front());

if (overlappedNode->is_in_group("Player"))
{

	Player* shotPlayer = static_cast<Player*>(overlappedNode);
	Variant shooterNodeName = shooterNodeName_;
	if (shotPlayer->get_name() != String(shooterNodeName))
	{
		if (shotPlayer->is_network_master())
			shotPlayer->inflictDamage(damage_);
	}
	queue_free();
}
else
{
	velocity_ = Vector2(0, 0);
	static_cast<Timer*>(get_node("BoltLifeAfterHitTimer"))->start();
}
*/