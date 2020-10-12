#include "EntanglingBalls.h"

#include "../../player/Player.h"
#include "../../npc/monsters/Monster.h"

#include <Area2D.hpp>
#include <Timer.hpp>
#include <AudioStreamPlayer.hpp>
#include <CollisionPolygon2D.hpp>

using namespace godot;

void EntanglingBalls::_register_methods()
{
	register_method("_init", &EntanglingBalls::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &EntanglingBalls::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &EntanglingBalls::_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processImpact", &EntanglingBalls::processImpact, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processMovement", &EntanglingBalls::processMovement, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("collisionDetected", &EntanglingBalls::collisionDetected, GODOT_METHOD_RPC_MODE_DISABLED);

	register_method("activate", &EntanglingBalls::activate, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("deactivate", &EntanglingBalls::deactivate, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("playAttackSound", &EntanglingBalls::playAttackSound, GODOT_METHOD_RPC_MODE_PUPPETSYNC);

	register_property<EntanglingBalls, Vector2>("velocity_", &EntanglingBalls::velocity_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<EntanglingBalls, Vector2>("initialPosition_", &EntanglingBalls::initialPosition_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<EntanglingBalls, int64_t>("shooterNodeName_", &EntanglingBalls::shooterNodeName_, 0, GODOT_METHOD_RPC_MODE_DISABLED);

	register_property<EntanglingBalls, Vector2>("slavePosition_", &EntanglingBalls::slavePosition_, Vector2(), GODOT_METHOD_RPC_MODE_PUPPET);
	register_property<EntanglingBalls, real_t>("slaveRotation_", &EntanglingBalls::slaveRotation_, 0, GODOT_METHOD_RPC_MODE_PUPPET);
	register_property<EntanglingBalls, bool>("activated_", &EntanglingBalls::activated_, false, GODOT_METHOD_RPC_MODE_DISABLED);
}

void EntanglingBalls::_init()
{
	activated_ = false;
	objectHit_ = false;
	shooterNodeName_ = 0;
	slavePosition_ = Vector2();
	slaveRotation_ = 0;
}

void EntanglingBalls::_physics_process(float delta)
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

void EntanglingBalls::_process(float delta)
{
	if (!activated_)
		return;
	if (is_network_master())
	{
		if (!objectHit_ && collisionDetected())
			processImpact();
	}
}


void EntanglingBalls::processImpact()
{
	Array overlapingBodies = static_cast<Area2D*>(get_node("Area"))->get_overlapping_bodies();
	if (overlapingBodies.empty())
		return;
	objectHit_ = true;
	velocity_ = Vector2(0, 0);
	for (unsigned int i = 0; i < overlapingBodies.size(); ++i)
	{
		Node* overlappedNode = static_cast<Node*>(overlapingBodies[i]);
		if (overlappedNode->is_in_group("Player"))
		{
			Player* shotPlayer = static_cast<Player*>(overlappedNode);
			Variant shooterNodeName = shooterNodeName_;
			if (shotPlayer->get_name() != String(shooterNodeName))
				shotPlayer->inflictSlow(SLOW, SLOW_DURATION);
			break;
		}
		else if (overlappedNode->is_in_group("Monster") && !overlappedNode->is_in_group("Boss"))
		{
			Monster* shotMonster = static_cast<Monster*>(overlappedNode);
			shotMonster->inflictSlow(SLOW, SLOW_DURATION);
			break;
		}
	}
	rpc("deactivate");
}

void EntanglingBalls::processMovement()
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

void EntanglingBalls::playAttackSound()
{
	static_cast<AudioStreamPlayer*>(get_node("ThrowSound"))->play();
}

void EntanglingBalls::activate(int64_t shooterNodeName, Vector2 initialPosition, Vector2 initialDirection)
{
	static_cast<CollisionPolygon2D*>(get_node("Area/CollisionPolygon2D"))->set_disabled(false);
	set_process(true);
	set_physics_process(true);
	set_visible(true);
	set_position(initialPosition);
	initialPosition_ = initialPosition;
	set_rotation(-initialDirection.angle_to(Vector2(1, 0)));
	set("velocity_", Vector2(initialDirection.x * INITIAL_ENTANGLING_BALLS_SPEED, initialDirection.y * INITIAL_ENTANGLING_BALLS_SPEED));
	set("shooterNodeName_", shooterNodeName);
	set("initialDirection_", initialDirection);
	activated_ = true;
	objectHit_ = false;
	static_cast<AudioStreamPlayer*>(get_node("ThrowSound"))->play();
	if (is_network_master())
		get_node("/root/Game")->call("setEntanglingBallsToActivated", this);
}

void EntanglingBalls::deactivate()
{
	activated_ = false;
	objectHit_ = false;
	static_cast<CollisionPolygon2D*>(get_node("Area/CollisionPolygon2D"))->set_disabled(true);
	set_process(false);
	set_physics_process(false);
	set_visible(false);
	set_position(Vector2(0, 0));
	set_rotation(0);
	if (is_network_master())
	{
		get_node("/root/Game")->call("setEntanglingBallsToDeactivated", this);
		get_node("/root/Game")->call("putEntanglingBallsOnStack", this);
	}
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