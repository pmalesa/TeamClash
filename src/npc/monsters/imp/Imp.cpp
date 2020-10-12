#include "Imp.h"

#include <SceneTree.hpp>
#include <AudioStreamPlayer.hpp>
#include <AnimatedSprite.hpp>
#include <CollisionShape2D.hpp>
#include <cmath>

#include <iostream>

extern std::default_random_engine generator;
std::uniform_real_distribution<float> distribution(0.1f, 3.0f);

using namespace godot;

void Imp::_register_methods()
{
    register_method("_physics_process", &Imp::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Imp::_process, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Imp::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_init", &Imp::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_move", &Imp::_move, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_die", &Imp::_die, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("init", &Imp::init, GODOT_METHOD_RPC_MODE_DISABLED);

	register_method("setSpawnPoint", &Imp::setSpawnPoint, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("getSpawnPoint", &Imp::getSpawnPoint, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("inflictDamage", &Imp::inflictDamage, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("inflictSlow", &Imp::inflictSlow, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("immobilize", &Imp::immobilize, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("playBodyHitSound", &Imp::playBodyHitSound, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("applyThrowback", &Imp::applyThrowback, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("updateSprite", &Imp::updateSprite, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("updateHealthPoints", &Imp::updateHealthPoints, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("updateMovementSpeed", &Imp::updateMovementSpeed, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("setVelocity", &Imp::setVelocity, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("setSlowTime", &Imp::setSlowTime, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("setImmobilizeTime", &Imp::setImmobilizeTime, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("setImmobilize", &Imp::setImmobilize, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("throwback", &Imp::throwback, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("updateHealthBar", &Imp::updateHealthBar, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("showEntanglementEffect", &Imp::showEntanglementEffect, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("hideEntanglementEffect", &Imp::hideEntanglementEffect, GODOT_METHOD_RPC_MODE_REMOTESYNC);

	register_method("_on_SlowTimer_timeout", &Imp::_on_SlowTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_ImmobilizeTimer_timeout", &Imp::_on_ImmobilizeTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_RespawnTimer_timeout", &Imp::_on_RespawnTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_AIProcessingTimer_timeout", &Imp::_on_AIProcessingTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);

	register_property<Imp, int64_t>("healthPoints_", &Imp::healthPoints_, 0, GODOT_METHOD_RPC_MODE_DISABLED);
    register_property<Imp, Vector2>("slavePosition", &Imp::slavePosition, Vector2(), GODOT_METHOD_RPC_MODE_PUPPET);
    register_property<Imp, int64_t>("slaveMovement", &Imp::slaveMovement, static_cast<int64_t>(MoveDirection::NONE), GODOT_METHOD_RPC_MODE_PUPPET);
    register_property<Imp, int64_t>("slaveWeaponState", &Imp::slaveWeaponState, static_cast<int64_t>(WeaponState::IDLE), GODOT_METHOD_RPC_MODE_PUPPET);
	register_property<Imp, int64_t>("nodeName_", &Imp::nodeName_, 0, GODOT_METHOD_RPC_MODE_DISABLED);

	register_property<Imp, Vector2>("facingDirection_", &Imp::facingDirection_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<Imp, Vector2>("throwbackVelocity_", &Imp::throwbackVelocity_, Vector2(), GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_property<Imp, bool>("applyThrowback_", &Imp::applyThrowback_, false, GODOT_METHOD_RPC_MODE_REMOTESYNC);
}

void Imp::_init()
{
	Monster::_init();
	Godot::print("[IMP] Imp initialized.");
}

void Imp::_ready()
{
	Monster::_ready();
	getHealthBar()->set_max(MAX_HP);
	setHealthPoints(MAX_HP);
	updateHealthBar();
	setCurrentMovementSpeed(DEFAULT_MOVEMENT_SPEED);
	setInitialized(true);
	Godot::print("[IMP] Imp ready.");
}

void Imp::init(Vector2 newSpawnPoint)
{
	//setSpawnPoint(newSpawnPoint);
	//set_position(newSpawnPoint);
}

void Imp::_physics_process(float delta)
{
	if (!isInitialized())
		return;
	if (is_network_master())
	{
		processMovement();
		rset_unreliable("slavePosition", get_position());
		rset("slaveMovement", static_cast<int64_t>(moveDirection_));
		_move(static_cast<int64_t>(moveDirection_));
	}
	else
	{
		_move(slaveMovement);
		set_position(slavePosition);
	}
}

void Imp::_process(float delta)
{
	if (!isInitialized())
		return;
    if (is_network_master())
    {
		//acquireTarget();
	}
    else
    {
        moveDirection_ = MoveDirection(slaveMovement);
    }
	updateSprite();
}

void Imp::_on_AIProcessingTimer_timeout()
{
	AIProcessingTimer_->stop();
	if (currentAIState_ == AIState::SEEKING)
	{
		if (moveDirection_ == MoveDirection::NONE)
		{
			AIProcessingTimer_->set_wait_time(distribution(generator));
			if (generator() % 2)
				moveDirection_ = MoveDirection::RIGHT;
			else
				moveDirection_ = MoveDirection::LEFT;
		}
		else
		{
			AIProcessingTimer_->set_wait_time(0.5);
			moveDirection_ = MoveDirection::NONE;
		}
	}
	else if (currentAIState_ == AIState::ENGAGING)
	{
		/* To be implemented */
	}
	else if (currentAIState_ == AIState::FLEEING)
	{
		/* To be implemented */
	}
	else
	{
		/* nothing */
	}
	AIProcessingTimer_->start();
}

void Imp::performAttack()
{
	
}

void Imp::showEntanglementEffect()
{ 
	static_cast<Sprite*>(get_node("body_sprite/Entanglement"))->set_visible(true);
}

void Imp::hideEntanglementEffect()
{ 
	static_cast<Sprite*>(get_node("body_sprite/Entanglement"))->set_visible(false);
}

void Imp::updateSprite()
{
	AnimatedSprite* bodySprite = static_cast<AnimatedSprite*>(get_node("body_sprite"));
	AnimatedSprite* leftHandSprite = static_cast<AnimatedSprite*>(get_node("left_hand_sprite"));
	AnimatedSprite* rightHandSprite = static_cast<AnimatedSprite*>(get_node("right_hand_sprite"));
	Area2D* visionArea = static_cast<Area2D*>(get_node("VisionArea"));
	Area2D* attackArea = static_cast<Area2D*>(get_node("AttackArea"));
	Area2D* obstacleArea = static_cast<Area2D*>(get_node("ObstacleArea"));

	/* process attacking animations here */

	//if (getOwner()->currentWeapon_->getWeaponState() == WeaponState::IDLE)
	//	rightHandSprite->play("idle" + getOwner()->animationNameSuffix_);

	if (moveDirection_ == MoveDirection::RIGHT && !immobilized_)
	{
		bodySprite->set_flip_h(false);
		bodySprite->play("walk");
		rightHandSprite->set_flip_h(false);
		rightHandSprite->set_z_index(2);
		rightHandSprite->play("walk");
		leftHandSprite->set_flip_h(false);
		leftHandSprite->set_z_index(1);
		leftHandSprite->play("walk");
		visionArea->set_scale(Vector2(1, 1));
		attackArea->set_scale(Vector2(1, 1));
		obstacleArea->set_scale(Vector2(1, 1));
		facingDirection_ = Vector2(1, 0);
	}
	else if (moveDirection_ == MoveDirection::LEFT && !immobilized_)
	{
		bodySprite->set_flip_h(true);
		bodySprite->play("walk");
		rightHandSprite->set_flip_h(true);
		rightHandSprite->set_z_index(-3);
		rightHandSprite->play("walk");
		leftHandSprite->set_flip_h(true);
		leftHandSprite->set_z_index(3);
		leftHandSprite->play("walk");
		visionArea->set_scale(Vector2(-1, 1));
		attackArea->set_scale(Vector2(-1, 1));
		obstacleArea->set_scale(Vector2(-1, 1));
		facingDirection_ = Vector2(-1, 0);
	}
	else
	{
		bodySprite->play("idle");
		leftHandSprite->play("idle");
		rightHandSprite->play("idle");
	}
}

void Imp::_on_SlowTimer_timeout()
{
	hideEntanglementEffect();
	static_cast<Timer*>(get_node("SlowTimer"))->stop();
	rpc("updateMovementSpeed", DEFAULT_MOVEMENT_SPEED);
}

void Imp::_on_ImmobilizeTimer_timeout()
{
	static_cast<Timer*>(get_node("ImmobilizeTimer"))->stop();
	rpc("setImmobilize", false);
}

void Imp::_on_RespawnTimer_timeout()
{	
	static_cast<Timer*>(get_node("RespawnTimer"))->stop();
	//set_position(spawnPoint_);
	throwbackVelocity_ = Vector2(0, 0);
	applyThrowback_ = false;
	healthPoints_ = MAX_HP;
	updateHealthBar();
	for (unsigned int i = 0; i < get_child_count(); i++)
	{
		if (get_child(i)->has_method("show"))
		{
			get_child(i)->call("show");
		}
	}
	static_cast<CollisionShape2D*>(get_node("CollisionShape2D"))->set_disabled(false);
	set_physics_process(true);
	set_process(true);
}

void Imp::throwback(Vector2 direction, int64_t throwbackPower)
{
	Vector2 throwbackVelocity = Vector2(0, 0);
	if (direction.x > 0)
		throwbackVelocity.x = throwbackPower;
	else
		throwbackVelocity.x = -throwbackPower;
	if (direction.y > 0)
		throwbackVelocity.y = throwbackPower;
	else
		throwbackVelocity.y = -throwbackPower;
	throwbackVelocity_ = throwbackVelocity;
	applyThrowback_ = true;
}

void Imp::setSlowTime(int64_t slowTime)
{
	static_cast<Timer*>(get_node("SlowTimer"))->set_wait_time(slowTime);
	static_cast<Timer*>(get_node("SlowTimer"))->start();
}

void Imp::setImmobilizeTime(int64_t immobilizeTime)
{
	static_cast<Timer*>(get_node("ImmobilizeTimer"))->set_wait_time(immobilizeTime);
	static_cast<Timer*>(get_node("ImmobilizeTimer"))->start();
}

void Imp::setImmobilize(bool value)
{ 
	immobilized_ = value; 
}

void Imp::playBodyHitSound()
{
	static_cast<AudioStreamPlayer*>(get_node("BodyHitSound"))->play();
}

void Imp::_die(Player* killer)
{
	if (get_tree()->is_network_server())
	{
		if (killer != nullptr)
		{
			// increment team score
			// get_node("/root/Game/ScoreboardLayer/Scoreboard")->call("incrementKillCount", killer->getNodeName());
		}
	}
	static_cast<AudioStreamPlayer*>(get_node("DeathSound"))->play();
	static_cast<Timer*>(get_node("RespawnTimer"))->start();
	movementState_ = MovementState::NONE;
	static_cast<CollisionShape2D*>(get_node("CollisionShape2D"))->set_disabled(true);
	set_physics_process(false);
	set_process(false);
	for (unsigned int i = 0; i < get_child_count(); i++)
	{
		if (get_child(i)->has_method("hide"))
		{
			get_child(i)->call("hide");
		}
	}
	call("_on_SlowTimer_timeout");
}

void Imp::updateHealthPoints(int64_t newHealthPoints)
{
	if (healthPoints_ != newHealthPoints)
	{
		healthPoints_ = newHealthPoints;
	}
}

void Imp::updateHealthBar()
{
	getHealthBar()->setValue(healthPoints_);
}

void Imp::updateMovementSpeed(int64_t newMovementSpeed)
{
	setCurrentMovementSpeed(newMovementSpeed);
}