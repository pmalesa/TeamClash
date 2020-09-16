#include "Crossbow.h"

#include "../../player/Player.h"
#include "../projectiles/Bolt.h"
#include "../projectiles/ExplosiveBolt.h"

#include <SceneTree.hpp>
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
	register_method("processRangedAttack", &Crossbow::processRangedAttack, GODOT_METHOD_RPC_MODE_REMOTE);

	register_method("activateBolt", &Crossbow::activateBolt, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("activateExplosiveBolt", &Crossbow::activateExplosiveBolt, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("shootBolt", &Crossbow::shootBolt, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("shootExplosiveBolt", &Crossbow::shootExplosiveBolt, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("playAttackSound", &Crossbow::playAttackSound, GODOT_METHOD_RPC_MODE_DISABLED);
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
	if (is_network_master())
	{
		if (get_tree()->is_network_server())
			processRangedAttack();
 		else
			rpc_id(1, "processRangedAttack");
	}
}

void Crossbow::processRangedAttack()
{
	if (getWeaponState() == WeaponState::SHOOTING)
	{
		if (currentAmmoType_ == ProjectileType::BOLT && !boltOnCooldown())
			shootBolt();
		else if (currentAmmoType_ == ProjectileType::EXPLOSIVE_BOLT && !explosiveBoltOnCooldown())
			shootExplosiveBolt();
	}
}

void Crossbow::activateBolt(String boltNodeName, String shooterNodeName, Vector2 initialPosition, Vector2 initialDirection)
{
	Variant var = boltNodeName;
	int64_t id = static_cast<int64_t>(var);
	Bolt* bolt = static_cast<Bolt*>(get_node("/root/Game")->call("getBolt", id));
	if (!bolt)
		return;
	bolt->activate(shooterNodeName, initialPosition, initialDirection);
}

void Crossbow::activateExplosiveBolt(String explosiveBoltNodeName, String shooterNodeName, Vector2 initialPosition, Vector2 initialDirection)
{
	Variant var = explosiveBoltNodeName;
	int64_t id = static_cast<int64_t>(var);
	ExplosiveBolt* explosiveBolt = static_cast<ExplosiveBolt*>(get_node("/root/Game")->call("getExplosiveBolt", id));
	if (!explosiveBolt)
		return;
	explosiveBolt->activate(shooterNodeName, initialPosition, initialDirection);
}

void Crossbow::shootBolt()
{
	Bolt* bolt = static_cast<Bolt*>(get_node("/root/Game")->call("takeBoltFromStack"));
	if (!bolt)
		return;
	Vector2 direction = getOwner()->getAimingDirection();
	Vector2 initialPosition = getOwnerPosition() + 20 * direction;
	rpc("activateBolt", bolt->get_name(), getOwner()->get_name(), initialPosition, direction);
	static_cast<Timer*>(get_node("BoltCooldown"))->start();
}

void Crossbow::shootExplosiveBolt()
{
	ExplosiveBolt* explosiveBolt = static_cast<ExplosiveBolt*>(get_node("/root/Game")->call("takeExplosiveBoltFromStack"));
	if (!explosiveBolt)
		return;
	Vector2 direction = getOwner()->getAimingDirection();
	Vector2 initialPosition = getOwnerPosition() + 20 * direction;
	rpc("activateExplosiveBolt", explosiveBolt->get_name(), getOwner()->get_name(), initialPosition, direction);
	static_cast<Timer*>(get_node("ExplosiveBoltCooldown"))->start();
}