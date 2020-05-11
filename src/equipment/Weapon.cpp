#include "Weapon.h"

#include <ResourceLoader.hpp>
#include <Sprite.hpp>
#include <Texture.hpp>
#include <Vector2.hpp>


using namespace godot;

void Weapon::_register_methods()
{
	register_method("_init", &Weapon::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Weapon::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &Weapon::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Weapon::_process, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Weapon::_init()
{
	
}

void Weapon::_ready()
{
	set_physics_process(false);
	set_z_index(3);
	set_z_as_relative(true);
}

void Weapon::setWeapon(WeaponType weaponType)
{
	ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
	Sprite* weaponSprite = static_cast<Sprite*>(get_node("weapon_sprite"));
	weaponType_ = weaponType;
	if (weaponType == WeaponType::FISTS)
	{
		//damage_ = 1;
	}
	else if (weaponType == WeaponType::SWORD)
	{
		weaponSprite->set_texture(resourceLoader->load("res://sprites/warrior_basic_weapon.png"));
		set_position(Vector2(22, 22));
		damage_ = 24;
	}
	else if (weaponType == WeaponType::AXE)
	{
		//damage_ = 65;
	}
	else if (weaponType == WeaponType::CLUB)
	{
		//damage_ = 35;
	}
	else if (weaponType == WeaponType::STICK)
	{
		weaponSprite->set_texture(resourceLoader->load("res://sprites/mage_basic_weapon.png"));
		set_position(Vector2(22, 22));
		//damage_ = 25;
	}
	else if (weaponType == WeaponType::CROSSBOW)
	{
		weaponSprite->set_texture(resourceLoader->load("res://sprites/crossbow.png"));
		set_position(Vector2(30, 6));
	}
}

void Weapon::_physics_process(float delta)
{

}

void Weapon::_process(float delta)
{

}


