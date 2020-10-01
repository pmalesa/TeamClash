#pragma once

#include <core/Godot.hpp>
#include <Node.hpp>
#include <AudioStreamPlayer.hpp>

namespace godot
{
	class MusicModule : public Node
	{
        GODOT_CLASS(MusicModule, Node)

	public:
		MusicModule() = default;
		~MusicModule() = default;

		static void _register_methods();
		void _init();
		void _ready();

		void playButtonClickSound() const { static_cast<AudioStreamPlayer*>(get_node("ButtonClickSound"))->play(); }
		void playMainMenuMusic() const { static_cast<AudioStreamPlayer*>(get_node("MenuSoundtrack"))->play(); }
		void playInGameMusic() const { static_cast<AudioStreamPlayer*>(get_node("InGameSoundtrack"))->play(); }

		void stopPlayingMusic();

    private:

	};
}