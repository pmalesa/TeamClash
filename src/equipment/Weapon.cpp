#include "Weapon.h"

#include <Texture.hpp>
#include <Vector2.hpp>
#include <CollisionPolygon2D.hpp>

using namespace godot;

void Weapon::_register_methods()
{
    register_method("_ready", &Weapon::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_init", &Weapon::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_physics_process", &Weapon::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
}

void Weapon::_init()
{
	
}

void Weapon::init(WeaponType weaponType)
{
	ResourceLoader* resourceLoader_ = ResourceLoader::get_singleton();
	godot::Sprite* weaponSprite = static_cast<godot::Sprite*>(get_node("weapon_sprite"));
	godot::CollisionPolygon2D* collisionPolygon = static_cast<godot::CollisionPolygon2D*>(get_node("CollisionPolygon2D"));
	if (weaponType == WeaponType::FISTS)
	{
		damage_ = 1;
	}
	else if (weaponType == WeaponType::SWORD)
	{
		weaponSprite->set_texture(resourceLoader_->load("res://sprites/warrior_basic_weapon.png"));
		weaponSprite->set_position(Vector2(weaponSprite->get_position().x + 14, weaponSprite->get_position().y));
		collisionPolygon->set_position(Vector2(weaponSprite->get_position().x + 14, weaponSprite->get_position().y));
		damage_ = 4;
	}
	else if (weaponType == WeaponType::AXE)
	{
		damage_ = 65;
	}
	else if (weaponType == WeaponType::CLUB)
	{
		damage_ = 35;
	}
	else if (weaponType == WeaponType::STICK)
	{
		weaponSprite->set_texture(resourceLoader_->load("res://sprites/mage_basic_weapon.png"));
		damage_ = 25;
	}
}

void Weapon::_ready()
{
	set_physics_process(false);
}

void Weapon::_physics_process(float delta)
{

}


