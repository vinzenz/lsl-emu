string  ShortScriptName = "Controller";
string  VERSION_STRING  = "Escort Wall Server - Controller 1.00";

integer CMD_RESTART     = 0x5642FF01;
integer CMD_GET_MEMORY  = 0x5642FF02;
integer CMD_GET_VERSION = 0x5642FF03;
integer CMD_DEBUG_ON    = 0x5642FF04;
integer CMD_DEBUG_OFF   = 0x5642FF05;

integer DEBUG_ON = FALSE;


integer CMD_CHECK_MANAGER = 0x5642A507;
integer CMD_SHOW_MENU     = 0x5642A001;
integer CMD_CHECK_INVENTORY = 0x5642AA01;

integer MENU_CHANNEL = -1;
integer EOW_CHAN = 1;
integer REMOVE_ESCORT_CHAN = 2;
integer EOW_HANDLE = 0;
integer REMOVE_HANDLE = 0;

TellVersion()
{
    llSay(0,VERSION_STRING);
}

TellMemory()
{
    llSay(0,ShortScriptName + " - " + (string)llGetFreeMemory() + " bytes memory free.");
}

ShowMenu(key av)
{
    llDialog(av,"Please select action:", ["Tell ID","Add Escort","Remove Escort", "Set EOW", "Reset"], MENU_CHANNEL);
}

AddEscort(key av)
{
    llAllowInventoryDrop(TRUE);
    llDialog(av,"Please drop a setup notecard, a picture and a biography of the escort via CTRL + DRAG into the server. Press the 'Done' Button if you're finished.",["Done"],MENU_CHANNEL);
    llSetTimerEvent(120.);        
}

RemoveEscort(key av)
{
    if(REMOVE_HANDLE)
        llListenRemove(REMOVE_HANDLE);
    llInstantMessage(av,"Say the name of the Escort you want to remove in the Chat on channel " + (string)REMOVE_ESCORT_CHAN+ ":");
    REMOVE_HANDLE = llListen(REMOVE_ESCORT_CHAN,"",av,"");        
    llSetTimerEvent(120.);     
}

TellID(key av)
{
    llInstantMessage(av,"My ID is: " + (string)llGetKey());    
}

SetEOW(key av)
{
    if(EOW_HANDLE)
        llListenRemove(EOW_HANDLE);
    llInstantMessage(av,"Say the name of the Escort in the Chat on channel " +  (string)EOW_CHAN + ":");
    EOW_HANDLE = llListen(EOW_CHAN,"",av,"");
    llSetTimerEvent(120.);     
}

Reset()
{
    llMessageLinked(LINK_SET,CMD_RESTART,"",NULL_KEY);
    llResetScript();
}

default
{
    state_entry()
    {        
        MENU_CHANNEL = ((integer)llFrand(99999.) + 1000) * -1;
        llListen(MENU_CHANNEL,"",NULL_KEY,"");
    }
    
    on_rez(integer i)
    {
        Reset();
    }

    listen(integer channel, string name, key id, string cmd)
    {
        if(channel == MENU_CHANNEL)
        {
            if(cmd == "Set EOW")
                SetEOW(id);
            else if(cmd == "Tell ID")
                TellID(id);
            else if(cmd == "Add Escort")
                AddEscort(id);
            else if(cmd == "Remove Escort")
                RemoveEscort(id);
            else if(cmd == "Reset")
                Reset();
            else if(cmd == "Done") 
            {
                llSetTimerEvent(0.);
                llAllowInventoryDrop(FALSE);
                llMessageLinked(LINK_SET,CMD_CHECK_INVENTORY,"",NULL_KEY);
            }
        }
        else if(channel == EOW_CHAN)
        {
            llListenRemove(EOW_HANDLE);
            EOW_HANDLE = 0;
            llSetTimerEvent(0.);
            if(llGetInventoryType(cmd + " - Pic") == INVENTORY_TEXTURE)
            {
                llSetObjectDesc(cmd);
                llSay(0,"Escort of the week set to: '" + cmd + "'");
                llMessageLinked(LINK_SET,CMD_CHECK_INVENTORY,"",NULL_KEY);
            }
            else
            {
                llSay(0,"This escort cannot be found. Please check the exact spelling and try again");
            }
        }
        else if(channel == REMOVE_ESCORT_CHAN)
        {
            llListenRemove(REMOVE_HANDLE);
            REMOVE_HANDLE = 0;
            llSetTimerEvent(0.);
            if(llGetInventoryType(cmd + " - Pic") == INVENTORY_TEXTURE)
            {
                llRemoveInventory(cmd + " - Pic");
                llRemoveInventory(cmd + " - Bio");
                llRemoveInventory(cmd + " - Config");
                llSay(0,cmd + " was removed from the server.");
                llMessageLinked(LINK_SET,CMD_CHECK_INVENTORY,"",NULL_KEY);
            }
            else
            {
                llSay(0,"This escort cannot be found. Please check the exact spelling and try again");
            }
        }
    }
    
    timer()
    {
        if(EOW_HANDLE)
            llListenRemove(EOW_HANDLE);
        if(REMOVE_HANDLE)
            llListenRemove(REMOVE_HANDLE);
        llAllowInventoryDrop(FALSE);
        llSetTimerEvent(0.);
    }
    
    link_message(integer sender, integer ival, string sval, key kval)
    {
        if(ival == CMD_SHOW_MENU)
        {
            ShowMenu(kval);
        }
    }

    touch_start(integer i) 
    {
        key av = llDetectedKey(0);
        if(av == llGetOwner() || av == llGetInventoryCreator(llGetScriptName()))
        {
            ShowMenu(av);   
        }
        else
        {
            llMessageLinked(LINK_SET,CMD_CHECK_MANAGER,"",av);
        }
    }            
                                    
    on_rez(integer i)
    {
        llMessageLinked(LINK_SET,CMD_RESTART,"",NULL_KEY);
        llResetScript();
    }
}

