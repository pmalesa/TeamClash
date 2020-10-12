#include "Monster.h"

#include <SceneTree.hpp>
#include <KinematicCollision2D.hpp>
#include <Area2D.hpp>
#include <CollisionShape2D.hpp>
#include <Label.hpp>
#include <Texture.hpp>
#include <TextureRect.hpp>
#include <AnimatedSprite.hpp>
#include <Sprite.hpp>
#include <AnimationPlayer.hpp>
#include <AudioStreamPlayer.hpp>
#include <PackedScene.hpp>
#include <ResourceLoader.hpp>

std::default_random_engine generator(time(0));

using namespace godot;

void Monster::_register_methods()
{
    register_method("_physics_process", &Monster::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Monster::_process, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Monster::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_init", &Monster::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_move", &Monster::_move, GODOT_METHOD_RPC_MODE_DISABLED);

	register_method("inflictDamage", &Monster::inflictDamage, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("inflictSlow", &Monster::inflictSlow, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("immobilize", &Monster::immobilize, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("applyThrowback", &Monster::applyThrowback, GODOT_METHOD_RPC_MODE_DISABLED);

	//register_method("setVelocity", &Monster::setVelocity, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	//register_method("_on_RespawnTimer_timeout", &Monster::_on_RespawnTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	//register_method("setSpawnPoint", &Monster::setSpawnPoint, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	//register_method("throwback", &Monster::throwback, GODOT_METHOD_RPC_MODE_REMOTESYNC);
    //register_method("updateSprite", &Monster::updateSprite, GODOT_METHOD_RPC_MODE_DISABLED);
    //register_method("_die", &Monster::_die, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	//register_method("updateHealthPoints", &Monster::updateHealthPoints, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	//register_method("updateMovementSpeed", &Monster::updateMovementSpeed, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	//register_method("setSlowTime", &Monster::setSlowTime, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	//register_method("setImmobilizeTime", &Monster::setImmobilizeTime, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	//register_method("setImmobilize", &Monster::setImmobilize, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	//register_method("updateHealthBar", &Monster::updateHealthBar, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	//register_method("showEntanglementEffect", &Monster::showEntanglementEffect, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	//register_method("hideEntanglementEffect", &Monster::hideEntanglementEffect, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	//register_method("setCurrentMovementSpeed", &Monster::setCurrentMovementSpeed, GODOT_METHOD_RPC_MODE_DISABLED);
	//register_method("playBodyHitSound", &Monster::playBodyHitSound, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	
	//register_property<Monster, int64_t>("healthPoints_", &Monster::healthPoints_, 0, GODOT_METHOD_RPC_MODE_DISABLED);
 //   register_property<Monster, Vector2>("slavePosition", &Monster::slavePosition, Vector2(), GODOT_METHOD_RPC_MODE_PUPPET);
 //   register_property<Monster, int64_t>("slaveMovement", &Monster::slaveMovement, static_cast<int64_t>(MoveDirection::NONE), GODOT_METHOD_RPC_MODE_PUPPET);
 //   register_property<Monster, int64_t>("slaveWeaponState", &Monster::slaveWeaponState, static_cast<int64_t>(WeaponState::IDLE), GODOT_METHOD_RPC_MODE_PUPPET);
	//register_property<Monster, int64_t>("nodeName_", &Monster::nodeName_, 0, GODOT_METHOD_RPC_MODE_DISABLED);

	//register_property<Monster, Vector2>("facingDirection_", &Monster::facingDirection_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	//register_property<Monster, Vector2>("throwbackVelocity_", &Monster::throwbackVelocity_, Vector2(), GODOT_METHOD_RPC_MODE_REMOTESYNC);
	//register_property<Monster, bool>("applyThrowback_", &Monster::applyThrowback_, false, GODOT_METHOD_RPC_MODE_REMOTESYNC);
}

void Monster::_init()
{
	resourceLoader_ = ResourceLoader::get_singleton();
	healthPoints_ = 0;
	damageFactor_ = 1;
	velocity_ = Vector2(0, 0);
	facingDirection_ = Vector2(1, 0);
	throwbackVelocity_ = Vector2(0, 0);
	applyThrowback_ = false;
	immobilized_ = false;
	moveDirection_ = MoveDirection::NONE;
	movementState_ = MovementState::NONE;
    slavePosition = Vector2();
    slaveMovement = static_cast<int64_t>(MoveDirection::NONE);
    nodeName_ = 0;
	initialized_ = false;
	Godot::print("[MONSTER] Initialized.");
}

void Monster::_ready()
{
	healthBar_ = static_cast<HealthBar*>(get_node("HealthBar/HealthBar"));
	if (is_network_master())
	{
		AIProcessingTimer_ = static_cast<Timer*>(get_node("AIProcessingTimer"));
		AIProcessingTimer_->set_wait_time(2);
		currentAIState_ = AIState::SEEKING;
		AIProcessingTimer_->start();
	}
	Godot::print("[MONSTER] Ready.");
}

void Monster::_move(int64_t direction)
{
	if (immobilized_)
	{
		setVelocity(Vector2(0, 0));
		return;
	}

	if (!is_processing())
		moveDirection_ = MoveDirection::NONE;

    MoveDirection moveDirection = static_cast<MoveDirection>(direction);
	if (applyThrowback_)
	{
		movementState_ = MovementState::THROWBACK;
		applyThrowback_ = false;
	}
	if (is_on_ceiling())
	{
		velocity_.y = 0.0f;
		set_position(Vector2(get_position().x, get_position().y + 1));
	}

	if (movementState_ == MovementState::NONE)
	{
		if (is_on_floor())
			velocity_.y = 10.0f;
		else
			movementState_ = MovementState::FALLING;
	}
	if (movementState_ == MovementState::JUMPED)
	{
		velocity_.y = -JUMP_POWER;
		movementState_ = MovementState::FALLING;
	}
	if (movementState_ == MovementState::THROWBACK)
	{
		throwbackVelocity_.y = -200.0f;
		addVelocity(throwbackVelocity_);
		movementState_ = MovementState::FALLING;
	}
	if (movementState_ == MovementState::FALLING)
	{
		setVelocity(Vector2(getVelocity().x, getVelocity().y + GRAVITY_PULL));

		if (is_on_floor())
			movementState_ = MovementState::NONE;
	}

	switch (moveDirection)
	{
	case MoveDirection::NONE:
		if (throwbackVelocity_.x == 0)
			velocity_.x = 0.0f;
		else
			velocity_.x = throwbackVelocity_.x;
		break;

	case MoveDirection::LEFT:
		if (throwbackVelocity_.x == 0)
			velocity_.x = -currentMovementSpeed_;
		else
			velocity_.x = -currentMovementSpeed_ + throwbackVelocity_.x;
		break;

	case MoveDirection::RIGHT:
		if (throwbackVelocity_.x == 0)
			velocity_.x = currentMovementSpeed_;
		else
			velocity_.x = currentMovementSpeed_ + throwbackVelocity_.x;
		break;
	}

	if (throwbackVelocity_.x > 0)
	{
		throwbackVelocity_.x -= HORIZONTAL_THROWBACK_DECAY;
		if (throwbackVelocity_.x < 0)
			throwbackVelocity_.x = 0;
	}
	if (throwbackVelocity_.x < 0)
	{
		throwbackVelocity_.x += HORIZONTAL_THROWBACK_DECAY;
		if (throwbackVelocity_.x > 0)
			throwbackVelocity_.x = 0;
	}

	throwbackVelocity_.y = 0;
	move_and_slide(velocity_, Vector2(0, -1));
}

//void Monster::_on_AIProcessingTimer_timeout()
//{
//	AIProcessingTimer_->stop();
//	if (currentAIState_ == AIState::SEEKING)
//	{
//		if (moveDirection_ == MoveDirection::NONE)
//		{
//			srand(time(0));
//			if (rand() % 2)
//				moveDirection_ == MoveDirection::RIGHT;
//			else
//				moveDirection_ == MoveDirection::LEFT;
//		}
//		else
//		{
//			moveDirection_ == MoveDirection::NONE;
//		}
//	}
//	else if (currentAIState_ == AIState::ENGAGING)
//	{
//		/* To be implemented */
//	}
//	else if (currentAIState_ == AIState::FLEEING)
//	{
//		/* To be implemented */
//	}
//	else
//	{
//		/* nothing */
//	}
//	AIProcessingTimer_->start();
//}

void Monster::processMovement()
{
	if (!static_cast<Area2D*>(get_node("ObstacleArea"))->get_overlapping_bodies().empty() && moveDirection_ != MoveDirection::NONE && is_on_floor())
		jump();



	//if (currentAIState_ == AIState::SEEKING)
	//{

	//}
	//else if (currentAIState_ == AIState::ENGAGING)
	//{

	//}
	//else if (currentAIState_ == AIState::FLEEING)
	//{
	//	/* To be implemented */
	//}
	//else
	//{
	//	Godot::print("[MONSTER] Error. No such AI state exists!");
	//}
}

void Monster::acquireTarget()
{
	Array visibleNodes = static_cast<Area2D*>(get_node("VisionArea"))->get_overlapping_bodies();
	for (unsigned i = 0; i < visibleNodes.size(); ++i)
	{
		Node* visibleNode = static_cast<Node*>(visibleNodes[i]);
		if (visibleNode->is_in_group("Player"))
		{
			target_ = static_cast<Player*>(visibleNode);
			currentAIState_ = AIState::ENGAGING;
			AIProcessingTimer_->call("_on_AIProcessingTimer_timeout");
			return;
		}
	}
}

void Monster::jump()
{
	movementState_ = MovementState::JUMPED;
}

void Monster::inflictDamage(int64_t value, Player* attacker)
{
	healthPoints_ -= damageFactor_ * value;
	if (healthPoints_ < 0)
		healthPoints_ = 0;
	rpc("playBodyHitSound");
	rpc("updateHealthPoints", healthPoints_);
	rpc("updateHealthBar");
	if (healthPoints_ == 0)
		rpc("_die", attacker);
}

void Monster::inflictSlow(int64_t newSpeed, int64_t slowTime)
{
	rpc("playBodyHitSound");
	rpc("updateMovementSpeed", newSpeed);
	rpc("setSlowTime", slowTime);
	rpc("showEntanglementEffect");
}

void Monster::immobilize(int64_t immobilizeTime)
{
	rpc("setImmobilize", true);
	rpc("setImmobilizeTime", immobilizeTime);
	rset("applyThrowback_", false);
	rpc("throwback", Vector2(0, 0), 0);
}

void Monster::applyThrowback(Vector2 direction, int64_t throwbackPower)
{
	if (!immobilized_)
		rpc("throwback", direction, throwbackPower);
}
