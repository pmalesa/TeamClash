#include "Bolt.h"

#include "../../player/Player.h"

#include <SceneTree.hpp>
#include <Area2D.hpp>
#include <CollisionPolygon2D.hpp>
#include <Timer.hpp>
#include <AudioStreamPlayer.hpp>

#include <iostream>
using std::cout;
using std::endl;

using namespace godot;

void Bolt::_register_methods()
{
	register_method("_init", &Bolt::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &Bolt::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Bolt::_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processImpact", &Bolt::processImpact, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("processMovement", &Bolt::processMovement, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_BoltLifeAfterHitTimer_timeout", &Bolt::_on_BoltLifeAfterHitTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_BoltMaximumLifeTimer_timeout", &Bolt::_on_BoltMaximumLifeTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("collisionDetected", &Bolt::collisionDetected, GODOT_METHOD_RPC_MODE_DISABLED);

	register_method("activate", &Bolt::activate, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("deactivate", &Bolt::deactivate, GODOT_METHOD_RPC_MODE_REMOTESYNC);

	register_property<Bolt, Vector2>("velocity_", &Bolt::velocity_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<Bolt, Vector2>("initialDirection_", &Bolt::initialDirection_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<Bolt, Player*>("shooter_", &Bolt::shooter_, nullptr, GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<Bolt, Vector2>("slavePosition_", &Bolt::slavePosition_, Vector2(), GODOT_METHOD_RPC_MODE_PUPPET);
	register_property<Bolt, real_t>("slaveRotation_", &Bolt::slaveRotation_, 0, GODOT_METHOD_RPC_MODE_PUPPET);
	register_property<Bolt, bool>("activated_", &Bolt::activated_, false, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Bolt::_init()
{
	activated_ = false;
	objectHit_ = false;
	shooter_ = nullptr;
	slavePosition_ = Vector2();
	slaveRotation_ = 0;
}

void Bolt::_physics_process(float delta)
{
	if (!activated_)
		return;
	if (is_network_master())
	{
		if (!collisionDetected())
			processMovement();

		rset_unreliable("slavePosition_", get_position());
		rset_unreliable("slaveRotation_", get_rotation());
	}
	else
	{
		set_position(slavePosition_);
		set_rotation(slaveRotation_);
	}
}

void Bolt::_process(float delta)
{
	if (!activated_)
		return;
	if (is_network_master())
	{
		if (!objectHit_ && collisionDetected())
			processImpact();
	}
}


void Bolt::processImpact()
{
	Array overlapingBodies = static_cast<Area2D*>(get_node("BoltArea"))->get_overlapping_bodies();
	if (overlapingBodies.empty())
		return;

	objectHit_ = true;
	velocity_ = Vector2(0, 0);
	static_cast<Timer*>(get_node("BoltLifeAfterHitTimer"))->start();
	for (unsigned int i = 0; i < overlapingBodies.size(); ++i)
	{
		Node* overlappedNode = static_cast<Node*>(overlapingBodies[i]);
		if (overlappedNode->is_in_group("Player"))
		{
			Player* shotPlayer = static_cast<Player*>(overlapingBodies[i]);
			if (shotPlayer->get_name() != shooter_->get_name())
				shotPlayer->inflictDamage(damage_, shooter_);
			rpc("deactivate");
			return;
		}
	}
}

void Bolt::processMovement()
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
	if (is_network_master())
		rpc("deactivate");
}

void Bolt::_on_BoltMaximumLifeTimer_timeout()
{
	static_cast<Timer*>(get_node("BoltMaximumLifeTimer"))->stop();
	if (is_network_master())
		rpc("deactivate");
}

void Bolt::activate(Player* shooter, Vector2 initialPosition, Vector2 initialDirection)
{
	static_cast<CollisionPolygon2D*>(get_node("BoltArea/CollisionPolygon2D"))->set_disabled(false);
	set_position(initialPosition);
	set_rotation(-initialDirection.angle_to(Vector2(1, 0)));
	set("velocity_", Vector2(initialDirection.x * INITIAL_PROJECTILE_SPEED, initialDirection.y * INITIAL_PROJECTILE_SPEED));
	shooter_ = shooter;
	set("initialDirection_", initialDirection);
	objectHit_ = false;
	set_process(true);
	set_physics_process(true);
	set_visible(true);
	activated_ = true;
	static_cast<Timer*>(get_node("BoltMaximumLifeTimer"))->start();
	static_cast<AudioStreamPlayer*>(get_node("BoltShootSound"))->play();
	if (is_network_master())
		get_node("/root/Game")->call("setBoltToActivated", this);
}

void Bolt::deactivate()
{
	activated_ = false;
	set_visible(false);
	set_process(false);
	set_physics_process(false);
	static_cast<CollisionPolygon2D*>(get_node("BoltArea/CollisionPolygon2D"))->set_disabled(true);
	static_cast<Timer*>(get_node("BoltMaximumLifeTimer"))->stop();
	static_cast<Timer*>(get_node("BoltLifeAfterHitTimer"))->stop();
	set_position(Vector2(0, 0));
	set_rotation(0);
	objectHit_ = false;
	shooter_ = nullptr;
	if (is_network_master())
	{
		get_node("/root/Game")->call("setBoltToDeactivated", this);
		get_node("/root/Game")->call("putBoltOnStack", this);
	}
}

bool Bolt::collisionDetected()
{
	return !static_cast<Area2D*>(get_node("BoltArea"))->get_overlapping_bodies().empty();
}