#include "ExplosiveBolt.h"

#include "../../player/Player.h"

#include <Area2D.hpp>
#include <Timer.hpp>
#include <Node2D.hpp>
#include <CollisionPolygon2D.hpp>
#include <CanvasItem.hpp>
#include <AnimatedSprite.hpp>
#include <AudioStreamPlayer.hpp>

#include <iostream>
using std::cout;
using std::endl;

using namespace godot;

void ExplosiveBolt::_register_methods()
{
	register_method("_init", &ExplosiveBolt::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &ExplosiveBolt::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("init", &ExplosiveBolt::init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &ExplosiveBolt::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &ExplosiveBolt::_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processImpact", &ExplosiveBolt::processImpact, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processBoltMovement", &ExplosiveBolt::processBoltMovement, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_ExplosiveBoltMaximumLifeTimer_timeout", &ExplosiveBolt::_on_ExplosiveBoltMaximumLifeTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_ExplosiveBoltAfterExplosionLifeTime_timeout", &ExplosiveBolt::_on_ExplosiveBoltAfterExplosionLifeTime_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("collisionDetected", &ExplosiveBolt::collisionDetected, GODOT_METHOD_RPC_MODE_DISABLED);

	register_property<ExplosiveBolt, Vector2>("velocity_", &ExplosiveBolt::velocity_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<ExplosiveBolt, String>("shooterNodeName_", &ExplosiveBolt::shooterNodeName_, String(), GODOT_METHOD_RPC_MODE_DISABLED);
}

void ExplosiveBolt::_init()
{
	objectHit_ = false;
	Godot::print("[BOLT] Initialized.");
}

void ExplosiveBolt::_ready()
{
	static_cast<CanvasItem*>(get_node("Explosion/ExplosionAnimatedSprite"))->set_visible(false);
	set_physics_process(true);
	static_cast<Timer*>(get_node("ExplosiveBoltMaximumLifeTimer"))->start();
	static_cast<AudioStreamPlayer*>(get_node("ExplosiveBoltShootSound"))->play();
	Godot::print("[BOLT] Ready and fired.");
}

void ExplosiveBolt::init(String shooterNodeName, Vector2 initialPosition, Vector2 initialDirection)
{
	set_position(initialPosition);
	set_rotation(-initialDirection.angle_to(Vector2(1, 0)));
	set("velocity_", Vector2(initialDirection.x * INITIAL_PROJECTILE_SPEED, initialDirection.y * INITIAL_PROJECTILE_SPEED));
	set("shooterNodeName_", shooterNodeName);
	set("initialDirection_", initialDirection);
}

void ExplosiveBolt::_physics_process(float delta)
{
	if (!collisionDetected())
		processBoltMovement();
}

void ExplosiveBolt::_process(float delta)
{
	if (!objectHit_ && collisionDetected())
		processImpact();
}


void ExplosiveBolt::processImpact()
{
	Godot::print("[BOLT] Collision detected / object hit.");
	velocity_ = Vector2(0, 0);
	Node2D* explosionNode = static_cast<Node2D*>(get_node("Explosion"));
	explosionNode->set_rotation(explosionNode->get_rotation() - get_rotation());
	static_cast<CanvasItem*>(get_node("explosive_bolt_sprite"))->set_visible(false);
	static_cast<CanvasItem*>(get_node("Explosion/ExplosionAnimatedSprite"))->set_visible(true);
	static_cast<AudioStreamPlayer*>(get_node("Explosion/ExplosionSound"))->play();
	static_cast<AnimatedSprite*>(get_node("Explosion/ExplosionAnimatedSprite"))->play("explosion");
	static_cast<Timer*>(get_node("ExplosiveBoltAfterExplosionLifeTime"))->start();
	
	Array explosionOverlapingBodies = static_cast<Area2D*>(get_node("Explosion"))->get_overlapping_bodies();
	for (unsigned int i = 0; i < explosionOverlapingBodies.size(); ++i)
	{
		Node* overlappedNode = static_cast<Node*>(explosionOverlapingBodies[i]);
		if (overlappedNode->is_in_group("Player"))
		{
			Player* shotPlayer = static_cast<Player*>(explosionOverlapingBodies[i]);
			shotPlayer->inflictDamage(damage_);
			shotPlayer->applyThrowback(shotPlayer->get_position() - get_position(), EXPLOSION_THROWBACK);
		}
	}
	objectHit_ = true;
}

void ExplosiveBolt::processBoltMovement()
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
	queue_free();
}

void ExplosiveBolt::_on_ExplosiveBoltAfterExplosionLifeTime_timeout()
{
	static_cast<Timer*>(get_node("ExplosiveBoltAfterExplosionLifeTime"))->stop();
	queue_free();
}

bool ExplosiveBolt::collisionDetected()
{
	return !static_cast<Area2D*>(get_node("ExplosiveBoltArea"))->get_overlapping_bodies().empty();
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