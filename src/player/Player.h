#pragma once

#include <Godot.hpp>
#include <KinematicBody2D.hpp>

#include "../equipment/weapons/Weapon.h"
#include "../equipment/weapons/Crossbow.h"

#include "../camera/Camera.h"

#include "HealthBar.h"
#include <Sprite.hpp>
#include <Timer.hpp>

#include <memory>
using std::shared_ptr;
using std::make_shared;

namespace godot
{
	class Label;
	class AnimationPlayer;
	class ResourceLoader;
	class PackedScene;
	class Control;

    enum class MoveDirection : int64_t { LEFT, RIGHT, NONE };
	enum class MovementState : int64_t { NONE, JUMPED, FALLING, THROWBACK };
	enum class Team : int64_t { CELADON, CRIMSON };
	enum class RoleType : int64_t { WARRIOR, ARCHER };

    class Player : public KinematicBody2D
    {
		/* Friend classes */
		friend class Role;
		friend class Warrior;
		friend class Archer;

        GODOT_CLASS(Player, KinematicBody2D)

    public:
        static void _register_methods();

        Player() = default;
        ~Player() = default;

        void _init();
        void _ready();
        void init(int64_t chosenTeam, int64_t chosenRole);

		String getNickname() const { return nickname_; }
		Vector2 getVelocity() const { return velocity_; }
		int64_t getHealthPoints() const { return healthPoints_; }
		int64_t getNodeName() const { return nodeName_; }
		Vector2 getAimingDirection() const { return aimingDirection_; }

		void setNickname(String newNickname) { nickname_ = newNickname; }
		void setVelocity(Vector2 velocity) { velocity_ = velocity; }
		void addVelocity(Vector2 velocity) { velocity_ += velocity; }
		void setNodeName(int64_t nodeName) { nodeName_ = nodeName; }

		void inflictDamage(int64_t damage);
		void inflictSlow(int64_t newSpeed, int64_t slowTime);
		void immobilize(int64_t immobilizeTime);
		void applyThrowback(Vector2 direction, int64_t throwbackPower = DEFAULT_THROWBACK_POWER);

    private:
		void _physics_process(float delta);
		void _process(float delta);
		void _move(int64_t direction);
		void _die();
		void _on_SlowTimer_timeout();
		void _on_ImmobilizeTimer_timeout();
		void _on_RespawnTimer_timeout();
		void _on_FirstAbilityCooldown_timeout() { static_cast<Timer*>(get_node("FirstAbilityCooldown"))->stop(); }
		void _on_SecondAbilityCooldown_timeout() { static_cast<Timer*>(get_node("SecondAbilityCooldown"))->stop(); }
		void _on_ThirdAbilityCooldown_timeout() { static_cast<Timer*>(get_node("ThirdAbilityCooldown"))->stop(); }
		void _on_FourthAbilityCooldown_timeout() { static_cast<Timer*>(get_node("FourthAbilityCooldown"))->stop(); }
		void _on_FirstEffectTimer_timeout();
		void _on_SecondEffectTimer_timeout();

		void setTeam(int64_t team);
		void setRole(int64_t role);
		void setUI();
		void setSpawnPoint(Vector2 newSpawnPoint) { spawnPoint_ = newSpawnPoint; }

		void useFirstAbility();
		void useSecondAbility();
		void useThirdAbility();
		void useFourthAbility();
		void useAdditionalAbility();

		void showEntanglementEffect() { static_cast<Sprite*>(get_node("body_sprite/Entanglement"))->set_visible(true); }
		void hideEntanglementEffect() { static_cast<Sprite*>(get_node("body_sprite/Entanglement"))->set_visible(false); }
		void processInput();
		void updateSprite();
		void updateHealthPoints(int64_t newHealthPoints);
		void updateHealthBar();
		void updateMovementSpeed(int64_t newMovementSpeed);
		void setSlowTime(int64_t slowTime);
		void setImmobilizeTime(int64_t immobilizeTime);
		void setImmobilize(bool value) { immobilized_ = value; }
		void updateAimingDirection();
		void playBodyHitSound();

        static const int64_t DEFAULT_MOVEMENT_SPEED = 300;
        static const int64_t MAX_HP = 100;
		const float JUMP_POWER = 600.0f;
		const float GRAVITY_PULL = 20.0f;
		const static int64_t DEFAULT_THROWBACK_POWER = 400;
		const float HORIZONTAL_THROWBACK_DECAY = 30.0f;

		bool initialized_;

		Control* ui_;
		String nickname_;
		RoleType roleType_;

		shared_ptr<Role> role_;

		Camera* camera_;
		Label* nicknameLabel_;
		Vector2 velocity_;
		Vector2 facingDirection_;
		Vector2 aimingDirection_;
		Vector2 spawnPoint_;
		String animationNameSuffix_;
		Array weapons_;
		Weapon* currentWeapon_;
		int64_t currentMovementSpeed_;

		MoveDirection moveDirection_;
		MovementState movementState_;
        WeaponState weaponState_;

        Vector2 slavePosition;
        int64_t slaveMovement;
        int64_t slaveWeaponState;

        int64_t healthPoints_;
		HealthBar* healthBar_;
		float damageFactor_;
        int64_t nodeName_;

		Array alreadyAttackedPlayers_;
		Vector2 throwbackVelocity_;
		bool applyThrowback_;
		bool immobilized_;

		ResourceLoader* resourceLoader_;
    };
}
