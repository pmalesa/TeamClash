#pragma once
#include "../Player.h"

#include <Timer.hpp>
#include <Godot.hpp>
#include <CanvasLayer.hpp>


namespace godot
{
	class TextureProgress;

    class UI : public CanvasLayer
    {
        GODOT_CLASS(UI, CanvasLayer)

    public:
        static void _register_methods();

		UI() = default;
        virtual ~UI() = default;

		virtual void _init() {}
		virtual void _ready() {}

	protected:
		void setOwner(Player* newOwner) { owner_ = newOwner; }

		Player* getOwner() const { return owner_; }

		void setFirstCooldownBar(TextureProgress* newCooldownBar) { firstCooldownBar_ = newCooldownBar; }
		void setSecondCooldownBar(TextureProgress* newCooldownBar) { secondCooldownBar_ = newCooldownBar; }
		void setThirdCooldownBar(TextureProgress* newCooldownBar) { thirdCooldownBar_ = newCooldownBar; }
		void setFourthCooldownBar(TextureProgress* newCooldownBar) { fourthCooldownBar_ = newCooldownBar; }

		void setFirstCooldownTimer(Timer* newCooldownTimer) { firstCooldownTimer_ = newCooldownTimer; }
		void setSecondCooldownTimer(Timer* newCooldownTimer) { secondCooldownTimer_ = newCooldownTimer; }
		void setThirdCooldownTimer(Timer* newCooldownTimer) { thirdCooldownTimer_ = newCooldownTimer; }
		void setFourthCooldownTimer(Timer* newCooldownTimer) { fourthCooldownTimer_ = newCooldownTimer; }

		TextureProgress* getFirstCooldownBar() const { return firstCooldownBar_; }
		TextureProgress* getSecondCooldownBar() const { return secondCooldownBar_; }
		TextureProgress* getThirdCooldownBar() const { return thirdCooldownBar_; }
		TextureProgress* getFourthCooldownBar() const { return fourthCooldownBar_; }

		Timer* getFirstCooldownTimer() const { return firstCooldownTimer_; }
		Timer* getSecondCooldownTimer() const { return secondCooldownTimer_; }
		Timer* getThirdCooldownTimer() const { return thirdCooldownTimer_; }
		Timer* getFourthCooldownTimer() const { return fourthCooldownTimer_; }

		real_t getNormalizedTimeLeft(Timer* timer) { return 100.0 * (timer->get_time_left() / timer->get_wait_time()); }

	private:
		virtual void _process(float delta) {}
		virtual void updateCooldownBars() {}

		Player* owner_;
		int64_t role_;
		TextureProgress* firstCooldownBar_;
		TextureProgress* secondCooldownBar_;
		TextureProgress* thirdCooldownBar_;
		TextureProgress* fourthCooldownBar_;

		Timer* firstCooldownTimer_;
		Timer* secondCooldownTimer_;
		Timer* thirdCooldownTimer_;
		Timer* fourthCooldownTimer_;
	};
}
