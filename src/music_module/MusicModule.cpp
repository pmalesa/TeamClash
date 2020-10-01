#include "MusicModule.h"

using namespace godot;

void MusicModule::_register_methods()
{
    register_method("_init", &MusicModule::_init, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("_ready", &MusicModule::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("playButtonClickSound", &MusicModule::playButtonClickSound, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("playMainMenuMusic", &MusicModule::playMainMenuMusic, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("playInGameMusic", &MusicModule::playInGameMusic, GODOT_METHOD_RPC_MODE_DISABLED);
	register_method("stopPlayingMusic", &MusicModule::stopPlayingMusic, GODOT_METHOD_RPC_MODE_DISABLED);
}

void MusicModule::_init()
{
	Godot::print("[MUSIC_MODULE] Music module initialized.");
}

void MusicModule::_ready()
{
	get_node("/root/MusicModule")->call("playMainMenuMusic");
}

void MusicModule::stopPlayingMusic()
{
	if (static_cast<AudioStreamPlayer*>(get_node("MenuSoundtrack"))->is_playing())
		static_cast<AudioStreamPlayer*>(get_node("MenuSoundtrack"))->stop();
	if (static_cast<AudioStreamPlayer*>(get_node("InGameSoundtrack"))->is_playing())
		static_cast<AudioStreamPlayer*>(get_node("InGameSoundtrack"))->stop(); 
}
