float       spin;
integer     swing;
rotation    zerorot;
float       PI_BY_FOUR;
vector      use_vec;

vector SavePos = ZERO_VECTOR;
on_init()
{
    PI_BY_FOUR = PI / 4.0;
    swing = FALSE;
    spin = .45;

    llTargetOmega(<0.0,0.0,0.0>,spin,2.0);
    llSetTouchText("Swing");
    llSetTimerEvent(2.35);
}


default
{
    state_entry()
    {
        on_init();
    }
    
    on_rez(integer start)
    {
        on_init();
    }
    
    touch_end(integer num)
    {
        swing = !swing;
        if(swing)
        {
            SavePos = llGetPos();
            llSetTouchText("Stop swing");
            zerorot = llGetRot();
            use_vec = <0.0, 0.0, PI_BY_FOUR>;
            use_vec *= zerorot;
            llTargetOmega(use_vec,spin,2.0);
        }
        else
        {
            llSetRot(zerorot);
            on_init();
        }
    }       
    
    timer()
    {
        if(!swing)
            return;
        llSetPos(SavePos);
        llSetRot(llEuler2Rot(<0.0,0.0,PI_BY_FOUR*spin>) * zerorot);
        spin = -spin;
        llTargetOmega(use_vec,spin,2.0);
    }
}

