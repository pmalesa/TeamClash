#include "WarriorUI.h"

#include "../Warrior.h"

#include <SceneTree.hpp>
#include <TextureProgress.hpp>
#include <TextureRect.hpp>
#include <Texture.hpp>
#include <ResourceLoader.hpp>

using namespace godot;

void WarriorUI::_register_methods()
{
	register_method("_init", &WarriorUI::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_ready", &WarriorUI::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &WarriorUI::_process, GODOT_METHOD_RPC_MODE_DISABLED);
}

void WarriorUI::_init()
{
	set_process(false);
}

void WarriorUI::_ready()
{
	setOwner(static_cast<Player*>(get_node("/root/Game")->call("getSelfPlayer")));

	static_cast<TextureRect*>(get_node("Slot1/Icon"))->set_texture(getOwner()->resourceLoader_->load("res://sprites/icons/axe_icon.png"));
	static_cast<TextureRect*>(get_node("Slot2/Icon"))->set_texture(getOwner()->resourceLoader_->load("res://sprites/icons/entangling_balls_icon.png"));
	static_cast<TextureRect*>(get_node("Slot3/Icon"))->set_texture(getOwner()->resourceLoader_->load("res://sprites/icons/charge_icon.png"));
	static_cast<TextureRect*>(get_node("Slot4/Icon"))->set_texture(getOwner()->resourceLoader_->load("res://sprites/icons/stoneskin_icon.png"));
	static_cast<TextureRect*>(get_node("Slot1/Highlight"))->set_visible(true);

	setSecondCooldownBar(static_cast<TextureProgress*>(get_node("Slot2/CooldownProgress")));
	setThirdCooldownBar(static_cast<TextureProgress*>(get_node("Slot3/CooldownProgress")));
	setFourthCooldownBar(static_cast<TextureProgress*>(get_node("Slot4/CooldownProgress")));

	setFirstCooldownTimer(nullptr);
	setSecondCooldownTimer(static_cast<Timer*>(getOwner()->get_node("SecondAbilityCooldown")));
	setThirdCooldownTimer(static_cast<Timer*>(getOwner()->get_node("ThirdAbilityCooldown")));
	setFourthCooldownTimer(static_cast<Timer*>(getOwner()->get_node("FourthAbilityCooldown")));

	set_process(true);
}

void WarriorUI::_process(float delta)
{
	updateCooldownBars();
}

void WarriorUI::updateCooldownBars()
{
	getSecondCooldownBar()->set_value(getNormalizedTimeLeft(getSecondCooldownTimer()));
	getThirdCooldownBar()->set_value(getNormalizedTimeLeft(getThirdCooldownTimer()));
	getFourthCooldownBar()->set_value(getNormalizedTimeLeft(getFourthCooldownTimer()));
}