#include "Trap.h"

#include "../../player/Player.h"

#include <Area2D.hpp>
#include <CollisionPolygon2D.hpp>
#include <AnimatedSprite.hpp>
#include <Timer.hpp>
#include <AudioStreamPlayer.hpp>

#include <iostream>
using std::cout;
using std::endl;

using namespace godot;

void Trap::_register_methods()
{
	register_method("_init", &Trap::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Trap::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("init", &Trap::init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &Trap::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Trap::_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_PlacementTimer_timeout", &Trap::_on_PlacementTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_LifeAfterTriggerTimer_timeout", &Trap::_on_LifeAfterTriggerTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("collisionDetected", &Trap::collisionDetected, GODOT_METHOD_RPC_MODE_DISABLED);

	register_property<Trap, Vector2>("initialPosition_", &Trap::initialPosition_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
}

void Trap::_init()
{
	currentState_ = TrapState::PLACEMENT;
	Godot::print("[TRAP] Initialized.");
}

void Trap::_ready()
{
	static_cast<Timer*>(get_node("LifeAfterTriggerTimer"))->set_wait_time(IMMOBILIZE_TIME);
	Godot::print("[TRAP] Trap ready.");
}

void Trap::init(String ownerNodeName, Vector2 initialPosition)
{
	set_position(initialPosition);
	set_name(ownerNodeName);
	set("ownerNodeName_", ownerNodeName);
	set("initialPosition_", initialPosition);
	set("velocity_", Vector2(0, 0));
}

void Trap::_physics_process(float delta)
{
	if (currentState_ == TrapState::PLACEMENT)
		processMovement();
}

void Trap::_process(float delta)
{
	if (is_on_floor() && currentState_ == TrapState::PLACEMENT)
	{
		setup();
	}
	else if (currentState_ == TrapState::ARMED && collisionDetected())
	{
		processTrigger();
	}
}

void Trap::_on_PlacementTimer_timeout()
{
	static_cast<Timer*>(get_node("PlacementTimer"))->stop();
	set_physics_process(true);
	currentState_ = TrapState::ARMED;
	Godot::print("[TRAP] Trap armed.");
}

void Trap::_on_LifeAfterTriggerTimer_timeout()
{
	static_cast<Timer*>(get_node("LifeAfterTriggerTimer"))->stop();
	queue_free();
}

void Trap::processMovement()
{
	if (velocity_.y < MAX_FALLING_SPEED)
		velocity_.y += GRAVITY_PULL;

	move_and_slide(velocity_, Vector2(0, -1));
}

void Trap::setup()
{
	currentState_ = TrapState::SETUP;
	static_cast<Timer*>(get_node("PlacementTimer"))->start();
	static_cast<CollisionPolygon2D*>(get_node("TrapCollisionPolygon2D"))->set_disabled(true);
	static_cast<AnimatedSprite*>(get_node("TrapAnimatedSprite"))->play("placement");
	static_cast<AudioStreamPlayer*>(get_node("PlacementSound"))->play();
}

void Trap::processTrigger()
{
	Array overlappingBodies = static_cast<Area2D*>(get_node("TriggerArea"))->get_overlapping_bodies();
	if (!overlappingBodies.empty())
	{
		Node* overlappedNode = static_cast<Node*>(overlappingBodies[0]);
		if (overlappedNode->is_in_group("Player"))
		{
			currentState_ = TrapState::TRIGGERED;
			static_cast<AnimatedSprite*>(get_node("TrapAnimatedSprite"))->play("trigger");
			static_cast<AudioStreamPlayer*>(get_node("TriggerSound"))->play();
			static_cast<Timer*>(get_node("LifeAfterTriggerTimer"))->start();
			static_cast<Player*>(overlappedNode)->immobilize(IMMOBILIZE_TIME);
			set_physics_process(false);
		}
	}
}

bool Trap::collisionDetected()
{
	return !static_cast<Area2D*>(get_node("TriggerArea"))->get_overlapping_bodies().empty();
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