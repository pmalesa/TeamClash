#include "Player.h"

#include "Role.h"
#include "Warrior.h"
#include "Archer.h"

#include "../equipment/weapons/Dagger.h"
#include "../equipment/weapons/Axe.h"
#include "../equipment/weapons/Crossbow.h"
#include "../equipment/utility/EntanglingBalls.h"

#include <SceneTree.hpp>
#include <KinematicCollision2D.hpp>
#include <CollisionShape2D.hpp>
#include <Label.hpp>
#include <Texture.hpp>
#include <TextureRect.hpp>
#include <Array.hpp>
#include <Input.hpp>
#include <AnimatedSprite.hpp>
#include <Sprite.hpp>
#include <AnimationPlayer.hpp>
#include <AudioStreamPlayer.hpp>
#include <Viewport.hpp>
#include <PackedScene.hpp>
#include <ResourceLoader.hpp>
#include <CanvasLayer.hpp>

using namespace godot;

void Player::_register_methods()
{
    register_method("_physics_process", &Player::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_process", &Player::_process, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Player::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_init", &Player::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setVelocity", &Player::setVelocity, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("_on_SlowTimer_timeout", &Player::_on_SlowTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_ImmobilizeTimer_timeout", &Player::_on_ImmobilizeTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_RespawnTimer_timeout", &Player::_on_RespawnTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_FirstAbilityCooldown_timeout", &Player::_on_FirstAbilityCooldown_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_SecondAbilityCooldown_timeout", &Player::_on_SecondAbilityCooldown_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_ThirdAbilityCooldown_timeout", &Player::_on_ThirdAbilityCooldown_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_FourthAbilityCooldown_timeout", &Player::_on_FourthAbilityCooldown_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_FirstEffectTimer_timeout", &Player::_on_FirstEffectTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_on_SecondEffectTimer_timeout", &Player::_on_SecondEffectTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setTeam", &Player::setTeam, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("setRole", &Player::setRole, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("setupUI", &Player::setupUI, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("setSpawnPoint", &Player::setSpawnPoint, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("useFirstAbility", &Player::useFirstAbility, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("useSecondAbility", &Player::useSecondAbility, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("useThirdAbility", &Player::useThirdAbility, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("useFourthAbility", &Player::useFourthAbility, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("useAdditionalAbility", &Player::useAdditionalAbility, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
    register_method("_move", &Player::_move, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("inflictDamage", &Player::inflictDamage, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("inflictSlow", &Player::inflictSlow, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("immobilize", &Player::immobilize, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("playBodyHitSound", &Player::playBodyHitSound, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("applyThrowback", &Player::applyThrowback, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("processInput", &Player::processInput, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("updateSprite", &Player::updateSprite, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_die", &Player::_die, GODOT_METHOD_RPC_MODE_REMOTESYNC);
    register_method("init", &Player::init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("updateHealthPoints", &Player::updateHealthPoints, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("updateMovementSpeed", &Player::updateMovementSpeed, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("setSlowTime", &Player::setSlowTime, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("setImmobilizeTime", &Player::setImmobilizeTime, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("setImmobilize", &Player::setImmobilize, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("throwback", &Player::throwback, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("updateHealthBar", &Player::updateHealthBar, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("showEntanglementEffect", &Player::showEntanglementEffect, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("hideEntanglementEffect", &Player::hideEntanglementEffect, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	
	register_property<Player, int64_t>("healthPoints_", &Player::healthPoints_, 0, GODOT_METHOD_RPC_MODE_DISABLED);
    register_property<Player, Vector2>("slavePosition", &Player::slavePosition, Vector2(), GODOT_METHOD_RPC_MODE_PUPPET);
    register_property<Player, int64_t>("slaveMovement", &Player::slaveMovement, static_cast<int64_t>(MoveDirection::NONE), GODOT_METHOD_RPC_MODE_PUPPET);
    register_property<Player, int64_t>("slaveWeaponState", &Player::slaveWeaponState, static_cast<int64_t>(WeaponState::IDLE), GODOT_METHOD_RPC_MODE_PUPPET);
	register_property<Player, int64_t>("nodeName_", &Player::nodeName_, 0, GODOT_METHOD_RPC_MODE_DISABLED);

	register_property<Player, Vector2>("facingDirection_", &Player::facingDirection_, Vector2(), GODOT_METHOD_RPC_MODE_DISABLED);
	register_property<Player, Vector2>("aimingDirection_", &Player::aimingDirection_, Vector2(), GODOT_METHOD_RPC_MODE_PUPPET);
	register_property<Player, Vector2>("throwbackVelocity_", &Player::throwbackVelocity_, Vector2(), GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_property<Player, bool>("applyThrowback_", &Player::applyThrowback_, false, GODOT_METHOD_RPC_MODE_REMOTESYNC);
}

void Player::_init()
{
	resourceLoader_ = ResourceLoader::get_singleton();
	healthPoints_ = MAX_HP;
	damageFactor_ = 1;
	velocity_ = Vector2(0, 0);
	facingDirection_ = Vector2(1, 0);
	aimingDirection_ = Vector2(0, 0);
	throwbackVelocity_ = Vector2(0, 0);
	applyThrowback_ = false;
	immobilized_ = false;
	moveDirection_ = MoveDirection::NONE;
	movementState_ = MovementState::NONE;
    slavePosition = Vector2();
    slaveMovement = static_cast<int64_t>(MoveDirection::NONE);
    nodeName_ = 0;
	animationNameSuffix_ = String();
	initialized_ = false;

	Godot::print("[PLAYER] Player variables initialized.");
}

void Player::_ready()
{
	nicknameLabel_ = static_cast<Label*>(get_node("NicknameBar/Nickname"));
	healthBar_ = static_cast<HealthBar*>(get_node("HealthBar/HealthBar"));
	currentMovementSpeed_ = DEFAULT_MOVEMENT_SPEED;
	nicknameLabel_->set_text(get_node("/root/Network")->call("getConnectedPlayerNickname", nodeName_));
	updateHealthBar();
	Godot::print("[PLAYER] Player ready.");
}

void Player::_physics_process(float delta)
{
	if (!initialized_)
		return;

	if (is_network_master())
	{
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

void Player::_process(float delta)
{
	if (!initialized_)
		return;

    if (is_network_master())
    {
        processInput();
		updateAimingDirection();
		rset("slaveWeaponState", static_cast<int64_t>(currentWeapon_->getWeaponState()));
		rset("aimingDirection_", aimingDirection_);
	}
    else
    {
        moveDirection_ = MoveDirection(slaveMovement);
        currentWeapon_->setWeaponState(WeaponState(slaveWeaponState));
    }
	updateSprite();
}

void Player::_move(int64_t direction)
{
	if (immobilized_)
		return;

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

void Player::inflictDamage(int64_t value, Player* attacker)
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

void Player::inflictSlow(int64_t newSpeed, int64_t slowTime)
{
	rpc("playBodyHitSound");
	rpc("updateMovementSpeed", newSpeed);
	rpc("setSlowTime", slowTime);
	rpc("showEntanglementEffect");
}

void Player::immobilize(int64_t immobilizeTime)
{
	rpc("setImmobilize", true);
	rpc("setImmobilizeTime", immobilizeTime);
	rset("applyThrowback_", false);
	rpc("throwback", Vector2(0, 0), 0);
	rpc("setVelocity", Vector2(0, 0));
}

void Player::playBodyHitSound()
{
	static_cast<AudioStreamPlayer*>(get_node("BodyHitSound"))->play();
}

void Player::applyThrowback(Vector2 direction, int64_t throwbackPower)
{
	if (!immobilized_)
		rpc("throwback", direction, throwbackPower);
}

void Player::processInput()
{
	if (healthPoints_ == 0)
		return;
	moveDirection_ = MoveDirection::NONE;
	Input* input = Input::get_singleton();
	AnimationPlayer* weaponAnimation = static_cast<AnimationPlayer*>(get_node("melee_weapon_node/Weapon/melee_weapon_animation"));
	if (input->is_action_pressed("left"))
	{
		moveDirection_ = MoveDirection::LEFT;
	}
	else if (input->is_action_pressed("right"))
	{
		moveDirection_ = MoveDirection::RIGHT;
	}

	if (input->is_action_just_pressed("space"))
	{
		if (is_on_floor() && !immobilized_) movementState_ = MovementState::JUMPED;
	}
	if (input->is_action_just_pressed("1"))
	{
		rpc("useFirstAbility");
	}
	if (input->is_action_just_pressed("2"))
	{
		rpc("useSecondAbility");
	}
	if (input->is_action_just_pressed("3"))
	{
		rpc("useThirdAbility");
	}
	if (input->is_action_just_pressed("4"))
	{
		rpc("useFourthAbility");
	}
	if (input->is_action_just_pressed("f"))
	{
		rpc("useAdditionalAbility");
	}

	if (currentWeapon_->isRanged())
	{
		if (input->is_action_pressed("basic_attack"))
		{
			currentWeapon_->setWeaponState(WeaponState::SHOOTING);
		}
		else
		{
			currentWeapon_->setWeaponState(WeaponState::IDLE);
		}
	}
	else
	{
		if (input->is_action_just_pressed("basic_attack"))
		{
			if (!weaponAnimation->is_playing()) currentWeapon_->setWeaponState(WeaponState::ATTACKING);
		}
		else
		{
			if (!weaponAnimation->is_playing()) currentWeapon_->setWeaponState(WeaponState::IDLE);
		}
	}
}

void Player::updateSprite()
{
	role_->updateSprite();
}

void Player::_die(Player* killer)
{
	if (get_tree()->is_network_server())
	{
		if (killer != nullptr)
		{
			if (killer->getTeam() == this->getTeam() || killer == this)
				get_node("/root/Game/ScoreboardLayer/Scoreboard")->call("decrementKillCount", killer->getNodeName());
			else
				get_node("/root/Game/ScoreboardLayer/Scoreboard")->call("incrementKillCount", killer->getNodeName());
		}
		get_node("/root/Game/ScoreboardLayer/Scoreboard")->call("incrementDeathCount", this->getNodeName());
	}
	static_cast<AudioStreamPlayer*>(get_node("DeathSound"))->play();
    static_cast<Timer*>(get_node("RespawnTimer"))->start();
	currentWeapon_->setWeaponState(WeaponState::IDLE);
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
	if (is_network_master())
		get_node("/root/Game")->call("showRespawnWindow");
	if (get_tree()->is_network_server())
	{
		// Update scoreboard's data and serve it across all peers
	}
}

void Player::_on_SlowTimer_timeout()
{
	hideEntanglementEffect();
	static_cast<Timer*>(get_node("SlowTimer"))->stop();
	rpc("updateMovementSpeed", DEFAULT_MOVEMENT_SPEED);
}

void Player::_on_ImmobilizeTimer_timeout()
{
	static_cast<Timer*>(get_node("ImmobilizeTimer"))->stop();
	rpc("setImmobilize", false);
}

void Player::_on_RespawnTimer_timeout()
{	
	static_cast<Timer*>(get_node("RespawnTimer"))->stop();
	set_position(spawnPoint_);
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
	if (currentWeapon_->isRanged())
		static_cast<Node2D*>(get_node("melee_weapon_node"))->set_visible(false);
	else
		static_cast<Node2D*>(get_node("ranged_weapon_node"))->set_visible(false);
	static_cast<CollisionShape2D*>(get_node("CollisionShape2D"))->set_disabled(false);
	set_physics_process(true);
	set_process(true);
	if (is_network_master())
		get_node("/root/Game")->call("hideRespawnWindow");
}

void Player::_on_FirstEffectTimer_timeout()
{
	static_cast<Timer*>(get_node("FirstEffectTimer"))->stop();
	role_->neutralizeFirstEffect();
}

void Player::_on_SecondEffectTimer_timeout()
{
	static_cast<Timer*>(get_node("SecondEffectTimer"))->stop();
	role_->neutralizeSecondEffect();
}

void Player::init(int64_t chosenTeam, int64_t chosenRole)
{
	setTeam(chosenTeam);
	setRole(chosenRole);
	if (is_network_master())
		setupUI();
	if (Team::CELADON == static_cast<Team>(chosenTeam))
		setSpawnPoint(get_node("/root/Game/World")->call("getCeladonTeamSpawnPoint"));
	else
		setSpawnPoint(get_node("/root/Game/World")->call("getCrimsonTeamSpawnPoint"));
	set_position(spawnPoint_);
}

void Player::setTeam(int64_t team)
{
	team_ = static_cast<Team>(team);
	if (Team::CELADON == static_cast<Team>(team))
	{
		add_to_group("Celadon");
		animationNameSuffix_ = "_celadon";
	}
	else
	{
		add_to_group("Crimson");
		animationNameSuffix_ = "_crimson";
	}
}

void Player::setRole(int64_t role)
{
	roleType_ = static_cast<RoleType>(role);
	if (RoleType::WARRIOR == roleType_)
		role_ = make_shared<Warrior>(this);
	else
		role_ = make_shared<Archer>(this);
	initialized_ = true;
}

void Player::setupUI()
{
	role_->setupUI();
}

void Player::useFirstAbility()
{ 
	role_->useFirstAbility(); 
}

void Player::useSecondAbility()
{ 
	role_->useSecondAbility();
}

void Player::useThirdAbility()
{
	role_->useThirdAbility();
}

void Player::useFourthAbility()
{
	role_->useFourthAbility();
}

void Player::useAdditionalAbility()
{
	role_->useAdditionalAbility();
}

void Player::updateHealthPoints(int64_t newHealthPoints)
{
	if (healthPoints_ != newHealthPoints)
	{
		healthPoints_ = newHealthPoints;
	}
}

void Player::updateHealthBar()
{
	healthBar_->setValue(healthPoints_);
}

void Player::updateMovementSpeed(int64_t newMovementSpeed)
{
	currentMovementSpeed_ = newMovementSpeed; 
}

void Player::setSlowTime(int64_t slowTime)
{
	static_cast<Timer*>(get_node("SlowTimer"))->set_wait_time(slowTime);
	static_cast<Timer*>(get_node("SlowTimer"))->start();
}

void Player::setImmobilizeTime(int64_t immobilizeTime)
{
	static_cast<Timer*>(get_node("ImmobilizeTimer"))->set_wait_time(immobilizeTime);
	static_cast<Timer*>(get_node("ImmobilizeTimer"))->start();
}

void Player::throwback(Vector2 direction, int64_t throwbackPower)
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

void Player::updateAimingDirection()
{
	Vector2 mousePosition = get_viewport()->get_mouse_position();
	Vector2 aimingDirection = Vector2(mousePosition.x - 960, mousePosition.y - 540);
	if (facingDirection_.x == 1 && aimingDirection.x < 0)
		aimingDirection.x = 0;
	else if (facingDirection_.x == -1 && aimingDirection.x > 0)
		aimingDirection.x = 0;

	aimingDirection_ = aimingDirection.normalized();
}