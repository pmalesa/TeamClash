#include "Player.h"
#include <KinematicCollision2D.hpp>
#include <CollisionShape2D.hpp>
#include <TextureProgress.hpp>
#include <Label.hpp>
#include <ResourceLoader.hpp>
#include <Texture.hpp>
#include <Timer.hpp>
#include <Array.hpp>
#include <Input.hpp>
#include <SceneTree.hpp>

using namespace godot;

void Player::_register_methods()
{
    register_method("_physics_process", &Player::_physics_process, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_ready", &Player::_ready, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_init", &Player::_init, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_on_RespawnTimer_timeout", &Player::_on_RespawnTimer_timeout, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_move", &Player::_move, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("damage", &Player::damage, GODOT_METHOD_RPC_MODE_DISABLED);
    register_method("_die", &Player::_die, GODOT_METHOD_RPC_MODE_SYNC);
    register_method("init", &Player::init, GODOT_METHOD_RPC_MODE_DISABLED);

    register_property<Player, Vector2>("slavePosition", &Player::slavePosition, Vector2(), GODOT_METHOD_RPC_MODE_PUPPET);
    register_property<Player, int64_t>("slaveMovement", &Player::slaveMovement, static_cast<int64_t>(MoveDirection::NONE), GODOT_METHOD_RPC_MODE_PUPPET);

    /*
    GODOT_METHOD_RPC_MODE_DISABLED,
    GODOT_METHOD_RPC_MODE_REMOTE,
    GODOT_METHOD_RPC_MODE_MASTER,
    GODOT_METHOD_RPC_MODE_PUPPET,
    GODOT_METHOD_RPC_MODE_SLAVE = GODOT_METHOD_RPC_MODE_PUPPET,
    GODOT_METHOD_RPC_MODE_REMOTESYNC,
    GODOT_METHOD_RPC_MODE_SYNC = GODOT_METHOD_RPC_MODE_REMOTESYNC,
    GODOT_METHOD_RPC_MODE_MASTERSYNC,
    GODOT_METHOD_RPC_MODE_PUPPETSYNC,
    */
    register_property<Player, int64_t>("nodeName", &Player::nodeName, 0, GODOT_METHOD_RPC_MODE_DISABLED);
}

Player::Player()
{

}

Player::~Player()
{

}

void Player::_init()
{
    slavePosition = Vector2();
    slaveMovement = static_cast<int64_t>(MoveDirection::NONE);
    nodeName = 0;
}

void Player::_ready()
{

}

void Player::_physics_process(float delta)
{
    MoveDirection direction = MoveDirection::NONE;
    Godot::print("TESTO");
    if (is_network_master())
    {
        Input* input = Input::get_singleton();
        if (input->is_action_pressed("left"))
        {
            direction = MoveDirection::LEFT;
        }
        else if (input->is_action_pressed("right"))
        {
            direction = MoveDirection::RIGHT;
        }
        else if (input->is_action_pressed("up"))
        {
            direction = MoveDirection::UP;
        }
        else if (input->is_action_pressed("down"))
        {
            direction = MoveDirection::DOWN;
        }

        rset_unreliable("slavePosition", get_position());
        rset("slaveMovement", static_cast<int64_t>(direction));
        _move(static_cast<int64_t>(direction));
    }
    else
    {
        _move(slaveMovement);
        set_position(slavePosition);
    }

    if (get_tree()->is_network_server())
    {
        get_node("/root/Network")->call("update_position", get_name().to_int(), get_position());
    }
}

// NOTE : Bundle homogeneous data -> send data for all players in a single packet (400-450 bytes)
// NOTE : Optimize your state data -> compress flags and the like into smaller data formats
// NOTE : Use prediction -> instead of interpolating recv_pos and recv_vel,
//                          interpolate to recv_pos + recv_vel * tick

/*
void Player::update_state(int64_t time, float tick, Dictionary state)
{ // NOTE : This would be set as RPC_PUPPET
    if(lastTime >= time)
    {
        return;
    }

    lastTime = time;

    get_node("Tween")->stop_all();
    get_node("Tween")->interpolate_method(this, "set_pos",
                                         get_pos(), state["position"], tick,
                                         Tween.TRANS_LINEAR, Tween.EASE_IN);
    get_node("Tween")->interpolate_method(this, "set_linear_velocity",
                                         get_linear_velocity(), state["velocity"], tick,
                                         Tween.TRANS_LINEAR, Tween.EASE_IN);
    get_node("Tween")->start();
}
*/

void Player::_move(int64_t direction)
{
    MoveDirection moveDirection = static_cast<MoveDirection>(direction);
    switch (moveDirection)
    {
    case MoveDirection::NONE:
        return;

    case MoveDirection::UP:
        move_and_collide(Vector2(0, -MOVE_SPEED));
        break;

    case MoveDirection::DOWN:
        move_and_collide(Vector2(0, MOVE_SPEED));
        break;

    case MoveDirection::LEFT:
        move_and_collide(Vector2(-MOVE_SPEED, 0));
        break;

    case MoveDirection::RIGHT:
        move_and_collide(Vector2(MOVE_SPEED, 0));
        break;
    }
}

void Player::damage(int64_t value)
{
    healthPoints -= value;
    std::cout << "Damage, new health: " << healthPoints << std::endl;
    if (healthPoints <= 0)
    {
        healthPoints = 0;
        rpc("_die");
    }
}

void Player::_die()
{
    static_cast<Timer*>(get_node("RespawnTimer"))->start();
    set_physics_process(false);

    for (unsigned int i = 0; i < get_child_count(); i++)
    {
        if (get_child(i)->has_method("hide"))
        {
            get_child(i)->call("hide");
        }
    }
    static_cast<CollisionShape2D*>(get_node("CollisionShape2D"))->set_disabled(true);
}

void Player::_on_RespawnTimer_timeout()
{
    set_physics_process(true);

    for (unsigned int i = 0; i < get_child_count(); i++)
    {
        if (get_child(i)->has_method("show"))
        {
            get_child(i)->call("show");
        }
    }
    static_cast<CollisionShape2D*>(get_node("CollisionShape2D"))->set_disabled(false);
    healthPoints = MAX_HP;
}

void Player::init(String nickname, Vector2 startPosition, bool isSlave)
{
    set_global_position(startPosition);
    //if (isSlave)
    //{
    //    ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
    //    static_cast<Sprite*>(get_node("Sprite"))->set_texture(resourceLoader->load("res://player/player-alt.png"));
    //}
}
