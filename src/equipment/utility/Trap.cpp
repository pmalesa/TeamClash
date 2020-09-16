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
	register_method("setup", &Trap::setup, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("processTrigger", &Trap::processTrigger, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("activate", &Trap::activate, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("deactivate", &Trap::deactivate, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("_physics_process", &Trap::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Trap::_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_PlacementTimer_timeout", &Trap::_on_PlacementTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_LifeAfterTriggerTimer_timeout", &Trap::_on_LifeAfterTriggerTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("playerCollisionDetected", &Trap::playerCollisionDetected, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("playTriggerAnimation", &Trap::playTriggerAnimation, GODOT_METHOD_RPC_MODE_PUPPETSYNC);

	register_property<Trap, Vector2>("velocity_", &Trap::velocity_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<Trap, Vector2>("initialPosition_", &Trap::initialPosition_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<Trap, String>("ownerNodeName_", &Trap::ownerNodeName_, String(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<Trap, Vector2>("slavePosition_", &Trap::slavePosition_, Vector2(), GODOT_METHOD_RPC_MODE_PUPPET);
	register_property<Trap, bool>("activated_", &Trap::activated_, false, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Trap::_init()
{
	ownerNodeName_ = String();
	velocity_ = Vector2();
	initialPosition_ = Vector2();
	slavePosition_ = Vector2();
	activated_ = false;
	currentState_ = TrapState::DEACTIVATED;
}

void Trap::_ready()
{
	static_cast<Timer*>(get_node("LifeAfterTriggerTimer"))->set_wait_time(IMMOBILIZE_TIME);
}

void Trap::activate(String ownerNodeName, Vector2 initialPosition)
{
	static_cast<AnimatedSprite*>(get_node("TrapAnimatedSprite"))->set_animation("placement");
	static_cast<AnimatedSprite*>(get_node("TrapAnimatedSprite"))->set_frame(0);
	static_cast<AnimatedSprite*>(get_node("TrapAnimatedSprite"))->set_visible(true);
	static_cast<CollisionPolygon2D*>(get_node("TrapCollisionPolygon2D"))->set_disabled(false);
	static_cast<CollisionPolygon2D*>(get_node("TriggerArea/TriggerCollisionPolygon2D"))->set_disabled(true);
	set_position(initialPosition);
	set("ownerNodeName_", ownerNodeName);
	set("initialPosition_", initialPosition);
	set("velocity_", Vector2(0, 0));
	set_process(true);
	set_physics_process(true);
	set_visible(true);
	currentState_ = TrapState::PLACEMENT;
	activated_ = true;
	if (is_network_master())
		get_node("/root/Game")->call("setTrapToActivated", this);
}

void Trap::deactivate()
{
	activated_ = false;
	set_visible(false);
	set_process(false);
	set_physics_process(false);
	static_cast<AnimatedSprite*>(get_node("TrapAnimatedSprite"))->set_visible(false);
	static_cast<AnimatedSprite*>(get_node("TrapAnimatedSprite"))->stop();
	static_cast<AnimatedSprite*>(get_node("TrapAnimatedSprite"))->set_animation("placement");
	static_cast<AnimatedSprite*>(get_node("TrapAnimatedSprite"))->set_frame(0);
	static_cast<CollisionPolygon2D*>(get_node("TrapCollisionPolygon2D"))->set_disabled(true);
	static_cast<CollisionPolygon2D*>(get_node("TriggerArea/TriggerCollisionPolygon2D"))->set_disabled(true);
	set_position(Vector2(0, 0));
	currentState_ = TrapState::DEACTIVATED;
	if (is_network_master())
	{
		get_node("/root/Game")->call("setTrapToDeactivated", this);
		get_node("/root/Game")->call("putTrapOnStack", this);
	}
}

void Trap::_physics_process(float delta)
{
	if (!activated_)
		return;
	if (is_network_master())
	{
		if (!playerCollisionDetected())
			processMovement();
		rset_unreliable("slavePosition_", get_position());
	}
	else
		set_position(slavePosition_);
}

void Trap::_process(float delta)
{
	if (!activated_)
		return;
	if (is_network_master())
	{
		if (is_on_floor() && currentState_ == TrapState::PLACEMENT)
			rpc("setup");
		else if (currentState_ == TrapState::ARMED && playerCollisionDetected())
			processTrigger();
	}
}

void Trap::_on_PlacementTimer_timeout()
{
	static_cast<Timer*>(get_node("PlacementTimer"))->stop();
	set_physics_process(true);
	currentState_ = TrapState::ARMED;
	static_cast<CollisionPolygon2D*>(get_node("TriggerArea/TriggerCollisionPolygon2D"))->set_disabled(false);
}

void Trap::_on_LifeAfterTriggerTimer_timeout()
{
	static_cast<Timer*>(get_node("LifeAfterTriggerTimer"))->stop();
	rpc("deactivate");
}

void Trap::processMovement()
{
	if (is_on_floor())
		velocity_.y = 0;
	else if (!is_on_floor() && velocity_.y < MAX_FALLING_SPEED)
		velocity_.y += GRAVITY_PULL;

	move_and_slide(velocity_, Vector2(0, -1));
}

void Trap::setup()
{
	currentState_ = TrapState::SETUP;
	static_cast<Timer*>(get_node("PlacementTimer"))->start();
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
			rpc("playTriggerAnimation");
			static_cast<Timer*>(get_node("LifeAfterTriggerTimer"))->start();
			static_cast<Player*>(overlappedNode)->immobilize(IMMOBILIZE_TIME);
			set_physics_process(false);
		}
	}
}

bool Trap::playerCollisionDetected()
{
	return !static_cast<Area2D*>(get_node("TriggerArea"))->get_overlapping_bodies().empty();
}

void Trap::playTriggerAnimation()
{
	static_cast<AnimatedSprite*>(get_node("TrapAnimatedSprite"))->play("trigger");
	static_cast<AudioStreamPlayer*>(get_node("TriggerSound"))->play();
}
