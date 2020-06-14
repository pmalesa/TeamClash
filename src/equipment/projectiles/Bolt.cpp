#include "Bolt.h"

#include "../../player/Player.h"

#include <Area2D.hpp>
#include <Timer.hpp>
#include <AudioStreamPlayer.hpp>

#include <iostream>
using std::cout;
using std::endl;

using namespace godot;

void Bolt::_register_methods()
{
	register_method("_init", &Bolt::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Bolt::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("init", &Bolt::init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &Bolt::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Bolt::_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processImpact", &Bolt::processImpact, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processBoltMovement", &Bolt::processBoltMovement, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_BoltLifeAfterHitTimer_timeout", &Bolt::_on_BoltLifeAfterHitTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_BoltMaximumLifeTimer_timeout", &Bolt::_on_BoltMaximumLifeTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("collisionDetected", &Bolt::collisionDetected, GODOT_METHOD_RPC_MODE_DISABLED);

	register_property<Bolt, Vector2>("velocity_", &Bolt::velocity_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<Bolt, String>("shooterNodeName_", &Bolt::shooterNodeName_, String(), GODOT_METHOD_RPC_MODE_DISABLED);
}

void Bolt::_init()
{
	objectHit_ = false;
	Godot::print("[BOLT] Initialized.");
}

void Bolt::_ready()
{
	set_physics_process(true);
	static_cast<Timer*>(get_node("BoltMaximumLifeTimer"))->start();
	static_cast<AudioStreamPlayer*>(get_node("BoltShootSound"))->play();
	Godot::print("[BOLT] Ready and fired.");
}

void Bolt::init(String shooterNodeName, Vector2 initialPosition, Vector2 initialDirection)
{
	set_position(initialPosition);
	set_rotation(-initialDirection.angle_to(Vector2(1, 0)));
	set("velocity_", Vector2(initialDirection.x * INITIAL_PROJECTILE_SPEED, initialDirection.y * INITIAL_PROJECTILE_SPEED));
	set("shooterNodeName_", shooterNodeName);
	set("initialDirection_", initialDirection);
}

void Bolt::_physics_process(float delta)
{
	if (!collisionDetected())
		processBoltMovement();
}

void Bolt::_process(float delta)
{
	if (!objectHit_ && collisionDetected())
		processImpact();
}


void Bolt::processImpact()
{
	Godot::print("[BOLT] Collision detected / object hit.");
	Array overlapingBodies = static_cast<Area2D*>(get_node("BoltArea"))->get_overlapping_bodies();
	if (overlapingBodies.empty())
		return;

	for (unsigned int i = 0; i < overlapingBodies.size(); ++i)
	{
		Node* overlappedNode = static_cast<Node*>(overlapingBodies[i]);
		if (overlappedNode->is_in_group("Player"))
		{
			Player* shotPlayer = static_cast<Player*>(overlapingBodies[i]);
			if (shotPlayer->get_name() != shooterNodeName_)
				shotPlayer->inflictDamage(damage_);
			queue_free();
		}
		else
		{
			velocity_ = Vector2(0, 0);
			static_cast<Timer*>(get_node("BoltLifeAfterHitTimer"))->start();
		}
	}
	objectHit_ = true;
	// Test behavior of this function, when the bolt hits an attacking melee weapon!
}

void Bolt::processBoltMovement()
{
	if (velocity_.x > 0)
		velocity_.x -= AIR_RESISTANCE;
	else if (velocity_.x < 0)
		velocity_.x += AIR_RESISTANCE;

	if (velocity_.x < 0 && initialDirection_.x > 0)
		velocity_.x = 0;

	else if (velocity_.x > 0 && initialDirection_.x < 0)
		velocity_.x = 0;

	if (velocity_.y < MAX_FALLING_SPEED)
		velocity_.y += GRAVITY_PULL;

	set_rotation(-velocity_.angle_to(Vector2(1, 0)));
	move_and_slide(velocity_, Vector2(0, -1));
}

void Bolt::_on_BoltLifeAfterHitTimer_timeout()
{
	static_cast<Timer*>(get_node("BoltMaximumLifeTimer"))->stop();
	static_cast<Timer*>(get_node("BoltLifeAfterHitTimer"))->stop();
	queue_free();
}

void Bolt::_on_BoltMaximumLifeTimer_timeout()
{
	static_cast<Timer*>(get_node("BoltMaximumLifeTimer"))->stop();
	queue_free();
}

bool Bolt::collisionDetected()
{
	return !static_cast<Area2D*>(get_node("BoltArea"))->get_overlapping_bodies().empty();
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