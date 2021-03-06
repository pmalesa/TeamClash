#include "ExplosiveBolt.h"

#include "../../player/Player.h"
#include "../../npc/monsters/Monster.h"

#include <Area2D.hpp>
#include <Timer.hpp>
#include <Node2D.hpp>
#include <CollisionPolygon2D.hpp>
#include <CanvasItem.hpp>
#include <AnimatedSprite.hpp>
#include <AudioStreamPlayer.hpp>

using namespace godot;

void ExplosiveBolt::_register_methods()
{
	register_method("_init", &ExplosiveBolt::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &ExplosiveBolt::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &ExplosiveBolt::_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processImpact", &ExplosiveBolt::processImpact, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processMovement", &ExplosiveBolt::processMovement, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_ExplosiveBoltMaximumLifeTimer_timeout", &ExplosiveBolt::_on_ExplosiveBoltMaximumLifeTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_ExplosiveBoltAfterExplosionLifeTime_timeout", &ExplosiveBolt::_on_ExplosiveBoltAfterExplosionLifeTime_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("collisionDetected", &ExplosiveBolt::collisionDetected, GODOT_METHOD_RPC_MODE_DISABLED);

	register_method("activate", &ExplosiveBolt::activate, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("deactivate", &ExplosiveBolt::deactivate, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("playExplosionAnimation", &ExplosiveBolt::playExplosionAnimation, GODOT_METHOD_RPC_MODE_PUPPETSYNC);

	register_property<ExplosiveBolt, Vector2>("velocity_", &ExplosiveBolt::velocity_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<ExplosiveBolt, Vector2>("initialDirection_", &ExplosiveBolt::initialDirection_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<ExplosiveBolt, Player*>("shooter_", &ExplosiveBolt::shooter_, nullptr, GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<ExplosiveBolt, Vector2>("slavePosition_", &ExplosiveBolt::slavePosition_, Vector2(), GODOT_METHOD_RPC_MODE_PUPPET);
	register_property<ExplosiveBolt, real_t>("slaveRotation_", &ExplosiveBolt::slaveRotation_, 0, GODOT_METHOD_RPC_MODE_PUPPET);
	register_property<ExplosiveBolt, bool>("activated_", &ExplosiveBolt::activated_, false, GODOT_METHOD_RPC_MODE_DISABLED);
}

void ExplosiveBolt::_init()
{
	activated_ = false;
	objectHit_ = false;
	shooter_ = nullptr;
	slavePosition_ = Vector2();
	slaveRotation_ = 0;
	objectHit_ = false;
}

void ExplosiveBolt::_physics_process(float delta)
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

void ExplosiveBolt::_process(float delta)
{
	if (!activated_)
		return;
	if (is_network_master())
	{
		if (!objectHit_ && collisionDetected())
			processImpact();
	}
}


void ExplosiveBolt::processImpact()
{
	rpc("playExplosionAnimation");
	objectHit_ = true;
	velocity_ = Vector2(0, 0);
	static_cast<Timer*>(get_node("ExplosiveBoltAfterExplosionLifeTime"))->start();
	Array explosionOverlapingBodies = static_cast<Area2D*>(get_node("Explosion"))->get_overlapping_bodies();
	for (unsigned int i = 0; i < explosionOverlapingBodies.size(); ++i)
	{
		Node* overlappedNode = static_cast<Node*>(explosionOverlapingBodies[i]);
		if (overlappedNode->is_in_group("Player"))
		{
			Player* shotPlayer = static_cast<Player*>(explosionOverlapingBodies[i]);
			shotPlayer->inflictDamage(damage_, shooter_);
			shotPlayer->applyThrowback(shotPlayer->get_position() - get_position(), EXPLOSION_THROWBACK);
		}
		else if (overlappedNode->is_in_group("Monster"))
		{
			Monster* shotMonster = static_cast<Monster*>(explosionOverlapingBodies[i]);
			shotMonster->inflictDamage(damage_, shooter_);
			//if (!overlappedNode->is_in_group("Elite"))
				shotMonster->applyThrowback(shotMonster->get_position() - get_position(), EXPLOSION_THROWBACK);
		}
	}
}

void ExplosiveBolt::processMovement()
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

void ExplosiveBolt::_on_ExplosiveBoltMaximumLifeTimer_timeout()
{
	static_cast<Timer*>(get_node("ExplosiveBoltMaximumLifeTimer"))->stop();
	if (is_network_master())
		rpc("deactivate");
}

void ExplosiveBolt::_on_ExplosiveBoltAfterExplosionLifeTime_timeout()
{
	static_cast<Timer*>(get_node("ExplosiveBoltMaximumLifeTimer"))->stop();
	static_cast<Timer*>(get_node("ExplosiveBoltAfterExplosionLifeTime"))->stop();
	if (is_network_master())
		rpc("deactivate");
}

void ExplosiveBolt::activate(Player* shooter, Vector2 initialPosition, Vector2 initialDirection)
{
	static_cast<CollisionPolygon2D*>(get_node("ExplosiveBoltArea/CollisionPolygon2D"))->set_disabled(false);
	static_cast<AnimatedSprite*>(get_node("Explosion/ExplosionAnimatedSprite"))->set_frame(0);
	static_cast<AnimatedSprite*>(get_node("Explosion/ExplosionAnimatedSprite"))->set_visible(false);
	static_cast<CanvasItem*>(get_node("explosive_bolt_sprite"))->set_visible(true);
	set_position(initialPosition);
	set_rotation(-initialDirection.angle_to(Vector2(1, 0)));
	set("velocity_", Vector2(initialDirection.x * INITIAL_PROJECTILE_SPEED, initialDirection.y * INITIAL_PROJECTILE_SPEED));
	shooter_ = shooter;
	set("initialDirection_", initialDirection);
	set_process(true);
	set_physics_process(true);
	set_visible(true);
	activated_ = true;
	static_cast<Timer*>(get_node("ExplosiveBoltMaximumLifeTimer"))->start();
	static_cast<AudioStreamPlayer*>(get_node("ExplosiveBoltShootSound"))->play();
	if (is_network_master())
		get_node("/root/Game")->call("setExplosiveBoltToActivated", this);
}

void ExplosiveBolt::deactivate()
{
	activated_ = false;
	set_visible(false);
	set_process(false);
	set_physics_process(false);
	static_cast<CanvasItem*>(get_node("explosive_bolt_sprite"))->set_visible(false);
	static_cast<CollisionPolygon2D*>(get_node("ExplosiveBoltArea/CollisionPolygon2D"))->set_disabled(true);
	static_cast<CanvasItem*>(get_node("Explosion/ExplosionAnimatedSprite"))->set_visible(false);
	set_position(Vector2(0, 0));
	set_rotation(0);
	static_cast<Node2D*>(get_node("Explosion"))->set_rotation(0);
	objectHit_ = false;
	shooter_ = nullptr;
	if (is_network_master())
	{
		get_node("/root/Game")->call("setExplosiveBoltToDeactivated", this);
		get_node("/root/Game")->call("putExplosiveBoltOnStack", this);
	}
}

bool ExplosiveBolt::collisionDetected()
{
	return !static_cast<Area2D*>(get_node("ExplosiveBoltArea"))->get_overlapping_bodies().empty();
}

void ExplosiveBolt::playExplosionAnimation()
{
	Node2D* explosionNode = static_cast<Node2D*>(get_node("Explosion"));
	explosionNode->set_rotation(explosionNode->get_rotation() - get_rotation());
	static_cast<CanvasItem*>(get_node("explosive_bolt_sprite"))->set_visible(false);
	static_cast<AudioStreamPlayer*>(get_node("Explosion/ExplosionSound"))->play();
	static_cast<AnimatedSprite*>(get_node("Explosion/ExplosionAnimatedSprite"))->set_frame(0);
	static_cast<AnimatedSprite*>(get_node("Explosion/ExplosionAnimatedSprite"))->set_visible(true);
	static_cast<AnimatedSprite*>(get_node("Explosion/ExplosionAnimatedSprite"))->play("explosion");
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