#pragma once
#include <core/Godot.hpp>
#include <Node.hpp>

namespace godot
{
	class Network : public Node
	{
        GODOT_CLASS(Network, Node)

	public:
        Network() = default;
		~Network() = default;

		static void _register_methods();
		void _init();
        void _ready();

        void createServer(String playerName);

        void update_position(int64_t id, Vector2 position);

    private:
        Dictionary selfData_;
        Dictionary players_;


	};
}