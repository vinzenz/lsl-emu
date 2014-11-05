SetLocalRot(rotation rot)
{
    llSetRot(rot / ( ( ZERO_ROTATION / llGetLocalRot()) * llGetRot()));
}

float ROTATION_DELAY1 = 0.0;
float ROTATION_DELAY2 = 1.0;
rotation state1;
rotation state2;

integer TEXTURE_SIDE_STATE1     = 5;
integer TEXTURE_SIDE_STATE2     = 4;
integer current                 = TEXTURE_SIDE_STATE1;

integer CMD_ROTATE              = 0;
integer CMD_SET_TEXTURE         = 1;

default
{
    state_entry()
    {
        llSetRemoteScriptAccessPin(20021981);
        state1 = llEuler2Rot(<0.0,3 * PI_BY_TWO,PI>);
        state2 = llEuler2Rot(<0.0,PI_BY_TWO,0.0>);
        SetLocalRot(state1);
    }
            
    link_message(integer sender, integer num, string str, key id)
    {
        if(num == CMD_ROTATE)
        {
            if(current == TEXTURE_SIDE_STATE1)
            {
                if(ROTATION_DELAY1 > 0.0)
                    llSleep(ROTATION_DELAY1);
                current = TEXTURE_SIDE_STATE2;
                SetLocalRot(llGetLocalRot() / state2);
            }        
            else
            {
                if(ROTATION_DELAY2 > 0.0)
                    llSleep(ROTATION_DELAY2);
                current = TEXTURE_SIDE_STATE1;
                SetLocalRot(llGetLocalRot() * state2);
            }                
        }
        else if(num == CMD_SET_TEXTURE)
        {
            if(current == TEXTURE_SIDE_STATE1)
                llSetTexture(id,TEXTURE_SIDE_STATE1);
            else
                llSetTexture(id,TEXTURE_SIDE_STATE2);
        }
    }
}

