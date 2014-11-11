#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_HH_INCLUDED

namespace lsl {
namespace runtime {
namespace script {        
    
enum Constants {
    CHANGED_INVENTORY = 0x1,
    CHANGED_COLOR = 0x2,
    CHANGED_SHAPE = 0x4,
    CHANGED_SCALE = 0x8,
    CHANGED_TEXTURE = 0x10,
    CHANGED_LINK = 0x20,
    CHANGED_ALLOWED_DROP = 0x40,
    CHANGED_OWNER = 0x80,
    CHANGED_REGION = 0x100,
    CHANGED_TELEPORT = 0x200,
    CHANGED_REGION_START = 0x400,
    CHANGED_MEDIA = 0x800,

    CONTROL_FWD         = 0x00000001,
    CONTROL_BACK        = 0x00000002,
    CONTROL_LEFT        = 0x00000004,
    CONTROL_RIGHT       = 0x00000008,
    CONTROL_ROT_LEFT    = 0x00000100,
    CONTROL_ROT_RIGHT   = 0x00000200,
    CONTROL_UP          = 0x00000010,
    CONTROL_DOWN        = 0x00000020,
    CONTROL_LBUTTON     = 0x10000000,
    CONTROL_ML_LBUTTON  = 0x40000000,

    DATA_ONLINE         = 1,    // integer boolean
    DATA_NAME           = 2,    // string
    DATA_BORN           = 3,    // string
    DATA_RATING         = 4,    // deprecated
    DATA_PAYINFO        = 8,    // (integer) mask PAYMENT_INFO_*
    DATA_SIM_POS        = 5,    // (vector) Regions global position
    DATA_SIM_STATUS     = 6,    // string ("up", "down", "starting", "stopping", "crashed", "unknown")
    DATA_SIM_RATING     = 7,    // "PG", "MATURE", "ADULT", "UNKNOWN"

    OBJECT_NAME                     = 1,    // string
    OBJECT_DESC                     = 2,    // string
    OBJECT_POS                      = 3,    // vector
    OBJECT_ROT                      = 4,    // rotation
    OBJECT_VELOCITY                 = 5,    // vector
    OBJECT_OWNER                    = 6,    // key
    OBJECT_GROUP                    = 7,    // key
    OBJECT_CREATOR                  = 8,    // key
    OBJECT_RUNNING_SCRIPT_COUNT     = 9,    // integer
    OBJECT_TOTAL_SCRIPT_COUNT       = 10,   // integer
    OBJECT_SCRIPT_MEMORY            = 11,   // integer
    OBJECT_SCRIPT_TIME              = 12,   // float
    OBJECT_PRIM_EQUIVALENCE         = 13,   // integer
    OBJECT_SERVER_COST              = 14,   // float
    OBJECT_STREAMING_COST           = 15,   // float
    OBJECT_PHYSICS_COST             = 16,   // float
    OBJECT_CHARACTER_TIME           = 17,   // float
    OBJECT_ROOT                     = 18,   // key
    OBJECT_ATTACHED_POINT           = 19,   // integer
    OBJECT_PATHFINDING_TYPE         = 20,   // integer
    OBJECT_PHYSICS                  = 21,   // integer
    OBJECT_PHANTOM                  = 22,   // integer
    OBJECT_TEMP_ON_REZ              = 23,   // integer
    OBJECT_RENDER_WEIGHT            = 24,   // integer

    PAYMENT_INFO_ON_FILE    = 0x1,
    PAYMENT_INFO_USED       = 0x2,

    PU_SLOWDOWN_DISTANCE_REACHED            = 0x00,
    PU_GOAL_REACHED                         = 0x01,
    PU_FAILURE_INVALID_START                = 0x02,
    PU_FAILURE_INVALID_GOAL                 = 0x03,
    PU_FAILURE_UNREACHABLE                  = 0x04,
    PU_FAILURE_TARGET_GONE                  = 0x05,
    PU_FAILURE_NO_VALID_DESTINATION         = 0x06,
    PU_EVADE_HIDDEN                         = 0x07,
    PU_EVADE_SPOTTED                        = 0x08,
    PU_FAILURE_NO_NAVMESH                   = 0x09,
    PU_FAILURE_DYNAMIC_PATHFINDING_DISABLED = 0x0A,
    PU_FAILURE_PARCEL_UNREACHABLE           = 0x0B,
    PU_FAILURE_OTHER                        = 0xF420,

    DUMMY_CONSTANT______
};

    struct ScriptEventArguments {
        
    };

    struct ScriptEvent {
    
    };


    struct State {
        // attach( key id )
        // control( key id, integer level, integer edge )

        // experience_permissions( key agent_id )
        // experience_permissions_denied( key agent_id, integer reason )

        // http_request( key request_id, string method, string body )
        // http_response( key request_id, integer status, list metadata, string body )

        // land_collision( vector pos )
        // land_collision_end(vector pos)
        // land_collision_start(vector pos)

        // collision( integer num_detected )
        // collision_end(integer total_number)
        // collision_start(integer total_number)

        // at_rot_target(integer handle, rotation target_rot, rotation our_rot);
        // at_target( integer target_num, vector target_pos, vector our_pos )
        // not_at_rot_target( )
        // not_at_target( )

        // moving_end( )
        // moving_start( )

        // sensor(integer num_detected)
        // no_sensor( )
        
        // object_rez(key id)
        // on_rez( integer start_param )
        // path_update( integer type, list reserved )
        // remote_data( integer event_type, key channel, key message_id, string sender, integer data, string data )
        // run_time_permissions( integer perm )

        // money( key id, integer amount )
        // transaction_result( key id, integer success, string data )

        // touch( integer num_detected )
        // touch_end( integer num_detected )
        // touch_start( integer num_detected )

        // timer( )

        // link_message( integer sender_num, integer num, string str, key id )
        // listen( integer channel, string name, key id, string message )

        // changed( integer change )
        // dataserver( key query_id, string data )
        // email( string time, string address, string subject, string message, integer num_left )

        // state_entry( )
        // state_exit( )
    };
}

struct Script {

};

}
}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_HH_INCLUDED
