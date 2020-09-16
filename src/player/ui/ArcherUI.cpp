#include "ArcherUI.h"

#include "../Archer.h"

#include <SceneTree.hpp>
#include <TextureProgress.hpp>
#include <TextureRect.hpp>
#include <Texture.hpp>
#include <ResourceLoader.hpp>

using namespace godot;

void ArcherUI::_register_methods()
{
	register_method("_init", &ArcherUI::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_ready", &ArcherUI::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &ArcherUI::_process, GODOT_METHOD_RPC_MODE_DISABLED);
}

void ArcherUI::_init()
{
	set_process(false);
}

void ArcherUI::_ready()
{
	setOwner(static_cast<Player*>(get_node("/root/Game")->call("getSelfPlayer")));

	static_cast<TextureRect*>(get_node("Slot1/Icon"))->set_texture(getOwner()->resourceLoader_->load("res://sprites/icons/dagger_icon.png"));
	static_cast<TextureRect*>(get_node("Slot2/Icon"))->set_texture(getOwner()->resourceLoader_->load("res://sprites/icons/crossbow_icon.png"));
	static_cast<TextureRect*>(get_node("Slot3/Icon"))->set_texture(getOwner()->resourceLoader_->load("res://sprites/icons/trap_icon.png"));
	static_cast<TextureRect*>(get_node("Slot4/Icon"))->set_texture(getOwner()->resourceLoader_->load("res://sprites/icons/rapidfire_icon.png"));
	static_cast<CanvasItem*>(get_node("Slot2/Subslot1"))->set_visible(true);
	static_cast<CanvasItem*>(get_node("Slot2/Subslot2"))->set_visible(true);
	static_cast<TextureRect*>(get_node("Slot2/Highlight"))->set_visible(true);
	static_cast<TextureRect*>(get_node("Slot2/Subslot1/Highlight"))->set_visible(true);

	setFirstCooldownBar(static_cast<TextureProgress*>(get_node("Slot2/Subslot1/CooldownProgress")));
	setSecondCooldownBar(static_cast<TextureProgress*>(get_node("Slot2/Subslot2/CooldownProgress")));
	setThirdCooldownBar(static_cast<TextureProgress*>(get_node("Slot3/CooldownProgress")));
	setFourthCooldownBar(static_cast<TextureProgress*>(get_node("Slot4/CooldownProgress")));

	setFirstCooldownTimer(static_cast<Timer*>(static_cast<Crossbow*>(getOwner()->weapons_[1])->get_node("BoltCooldown")));
	setSecondCooldownTimer(static_cast<Timer*>(static_cast<Crossbow*>(getOwner()->weapons_[1])->get_node("ExplosiveBoltCooldown")));
	setThirdCooldownTimer(static_cast<Timer*>(getOwner()->get_node("ThirdAbilityCooldown")));
	setFourthCooldownTimer(static_cast<Timer*>(getOwner()->get_node("FourthAbilityCooldown")));
	
	set_process(true);
}

void ArcherUI::_process(float delta)
{
	updateCooldownBars();
}

void ArcherUI::updateCooldownBars()
{
	getFirstCooldownBar()->set_value(getNormalizedTimeLeft(getFirstCooldownTimer()));
	getSecondCooldownBar()->set_value(getNormalizedTimeLeft(getSecondCooldownTimer()));
	getThirdCooldownBar()->set_value(getNormalizedTimeLeft(getThirdCooldownTimer()));
	getFourthCooldownBar()->set_value(getNormalizedTimeLeft(getFourthCooldownTimer()));
}