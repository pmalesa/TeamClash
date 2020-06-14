#include "Crossbow.h"

#include "../../player/Player.h"
#include "../projectiles/Bolt.h"
#include "../projectiles/ExplosiveBolt.h"

#include <PackedScene.hpp>
#include <ResourceLoader.hpp>
#include <AudioStreamPlayer.hpp>

using namespace godot;

void Crossbow::_register_methods()
{
	register_method("_init", &Crossbow::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Crossbow::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &Crossbow::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Crossbow::_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_BoltCooldown_timeout", &Crossbow::_on_BoltCooldown_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_ExplosiveBoltCooldown_timeout", &Crossbow::_on_ExplosiveBoltCooldown_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("shoot", &Crossbow::shoot, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("shootBolt", &Crossbow::shootBolt, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("shootExplosiveBolt", &Crossbow::shootExplosiveBolt, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Crossbow::_init()
{
	ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
	boltScene_ = resourceLoader->load("res://equipment/projectiles/Bolt.tscn");
	explosiveBoltScene_ = resourceLoader->load("res://equipment/projectiles/ExplosiveBolt.tscn");
	setWeaponType(WeaponType::CROSSBOW);
	setWeaponState(WeaponState::IDLE);
	currentAmmoType_ = ProjectileType::BOLT;
}

void Crossbow::_ready()
{
	setOwner(get_node("/root/Game")->call("getPlayer", int64_t(Variant(get_parent()->get_parent()->get_name()))));
	set_physics_process(false);
	set_z_index(3);
	set_z_as_relative(true);
}

void Crossbow::playDrawSound()
{
	if (is_network_master())
	{
		static_cast<AudioStreamPlayer*>(get_node("CrossbowDrawSound"))->play();
	}
}

void Crossbow::playAttackSound()
{
	if (currentAmmoType_ == ProjectileType::BOLT)
		static_cast<AudioStreamPlayer*>(get_node("BoltShootSound"))->play();
	else
		static_cast<AudioStreamPlayer*>(get_node("ExplosiveBoltShootSound"))->play();
}

void Crossbow::_physics_process(float delta)
{

}

void Crossbow::_process(float delta)
{
	if (getWeaponState() == WeaponState::SHOOTING) shoot();
}

void Crossbow::shoot()
{
	if (currentAmmoType_ == ProjectileType::BOLT) shootBolt();
	else if (currentAmmoType_ == ProjectileType::EXPLOSIVE_BOLT) shootExplosiveBolt();
}

void Crossbow::shootBolt()
{
	if (!boltOnCooldown())
	{
		Bolt* bolt = static_cast<Bolt*>(boltScene_->instance());
		Vector2 direction = getOwner()->getAimingDirection();
		Vector2 boltInitialPosition = getOwnerPosition() + 20 * direction;
		bolt->init(getOwner()->get_name(), boltInitialPosition, direction);
		playAttackSound();
		get_node("/root/Game/World")->add_child(bolt);
		static_cast<Timer*>(get_node("BoltCooldown"))->start();
	}
}

void Crossbow::shootExplosiveBolt()
{
	if (!explosiveBoltOnCooldown())
	{
		ExplosiveBolt* explosiveBolt = static_cast<ExplosiveBolt*>(explosiveBoltScene_->instance());
		Vector2 direction = getOwner()->getAimingDirection();
		Vector2 boltInitialPosition = getOwnerPosition() + 20 * direction;
		explosiveBolt->init(getOwner()->get_name(), boltInitialPosition, direction);
		playAttackSound();
		get_node("/root/Game/World")->add_child(explosiveBolt);
		static_cast<Timer*>(get_node("ExplosiveBoltCooldown"))->start();
	}
}