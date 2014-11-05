string SETUP_NOTECARD   = "Billboard Ads Setup";
integer gSetupCurrent   = 0;
integer gAdCount        = 0;
integer gIsData         = FALSE;

list TEXTURES       = [];
list DELIVER        = [];
list TEXTURE_KEYS   = [];

integer LAST_IDX                = 0;
integer AD_COUNT                = 0;
float   ROTATE_INTERVALL        = 10.0;
integer CMD_ROTATE              = 0;
integer CMD_SET_TEXTURE         = 1;
integer DIALOG_CHANNEL          = 0;
integer DELIVER_CHANNEL         = 0;
integer INTERVALL_CHANNEL       = 0;
key     BLANK_TEXTURE           = "5748decc-f629-461c-9a36-a35a221fe21f";
vector CUR_VEC = ZERO_VECTOR;
error(string msg)
{
    llInstantMessage(llGetOwner(),"ERROR: " + msg);
}

init()
{
    LAST_IDX        = 0;
    AD_COUNT        = 0;
    TEXTURES        = [];
    DELIVER         = [];
    TEXTURE_KEYS    = [];
    
    gSetupCurrent   = 0;
    gAdCount        = 0;
    gIsData         = FALSE;
    
    DIALOG_CHANNEL = ((integer)llFrand(99999999)) + 1000;
    INTERVALL_CHANNEL = ((integer)llFrand(99999999)) + 1000;
    DELIVER_CHANNEL = -456789;
    llListen(DIALOG_CHANNEL, "", NULL_KEY, "");
    llListen(INTERVALL_CHANNEL, "", NULL_KEY, "");
    llListen(DELIVER_CHANNEL, "", NULL_KEY, "");
    llGetNotecardLine(SETUP_NOTECARD,0);
}

show_main_dialog()
{
    llDialog(llGetOwner(), "Please select action:", ["Set Pos","Reload ads", "Touch", "Set Intervall"], DIALOG_CHANNEL);
}

show_intervall_menu()
{
    llDialog(llGetOwner(), "Please select Intervall (Values in seconds):", ["40", "50", "60","10", "20", "30"], INTERVALL_CHANNEL);
}

default
{
    state_entry()
    {        
        llSetTimerEvent(0.0);
        init();
    }
    
    on_rez(integer i)
    {
        llSetTimerEvent(0.0);
        init();
    }
    
    listen(integer chan, string name, key id, string data)
    {
        if(chan == DIALOG_CHANNEL)
        {
            if(data == "Reload ads")
            {
                init();
            }
            else if(data == "Set Intervall")
            {
                show_intervall_menu();
            }
            else if(data == "Touch")
            {
                llGiveInventory(id,llList2String(DELIVER,LAST_IDX - 1));
            }
            else if(data == "Set Pos")
            {
                CUR_VEC = llGetPos();
            }
        }
        else if(chan == INTERVALL_CHANNEL)
        {
            ROTATE_INTERVALL = ((integer)data);
            llSetTimerEvent(ROTATE_INTERVALL);
        }
        else if(chan == DELIVER_CHANNEL)
        {
            if(data == llGetOwner())
            {
                show_main_dialog();
            }
            else
            {
                llGiveInventory(data,llList2String(DELIVER,LAST_IDX - 1));
            }        
        }
    }    
    
    dataserver(key qid, string data)
    {
        if(data != EOF)
        {
            if(data == ".:[BEGIN DATA]:.")
            {
                gIsData = TRUE;
            }
            else if(data == ".:[END DATA]:.")
            {
                gIsData = FALSE;   
            }
            else if(gIsData)
            {
                list tmp = llCSV2List(data);
                if(llGetListLength(tmp) == 2)
                {
                    string tex = llList2String(tmp,0);
                    TEXTURES += [tex];
                    DELIVER += [llList2String(tmp,1)];
                    TEXTURE_KEYS += [llGetInventoryKey(tex)];
                    ++AD_COUNT;
                }
                else
                {
                    error("Broken Setup File at line: " + (string)gSetupCurrent);
                }
            }            
            ++gSetupCurrent;            
            llGetNotecardLine(SETUP_NOTECARD,gSetupCurrent);
        }   
        else
        {
            if(AD_COUNT > 0)
            {
                llMessageLinked(LINK_SET,CMD_SET_TEXTURE,"",(key)llList2Key(TEXTURE_KEYS,0));
                llSetTimerEvent(ROTATE_INTERVALL);
            }    
        }
    }
    
    touch_start(integer i)
    {
        if(llDetectedKey(0) == llGetOwner())
        {
            show_main_dialog();
        }
        else
        {
            llGiveInventory(llDetectedKey(0),llList2String(DELIVER,LAST_IDX - 1));
        }
    }
    
    timer()
    {
        if(CUR_VEC != ZERO_VECTOR)
            llSetPos(CUR_VEC);
        llMessageLinked(LINK_SET,CMD_ROTATE,"",NULL_KEY);
        if(LAST_IDX + 1 < AD_COUNT)
            ++LAST_IDX;
        else
            LAST_IDX = 0;
        llMessageLinked(LINK_SET,CMD_SET_TEXTURE,"",(key)llList2Key(TEXTURE_KEYS,LAST_IDX));        
    }
}
