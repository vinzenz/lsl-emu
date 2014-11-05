string  ShortScriptName = "Data Check";
string  VERSION_STRING  = "Escort Wall Server - Data Check 1.00";

integer CMD_RESTART     = 0x5642FF01;
integer CMD_GET_MEMORY  = 0x5642FF02;
integer CMD_GET_VERSION = 0x5642FF03;
integer CMD_DEBUG_ON    = 0x5642FF04;
integer CMD_DEBUG_OFF   = 0x5642FF05;
integer DEBUG_ON = FALSE;

integer CMD_CHECK_INVENTORY = 0x5642AA01;
integer CMD_UPDATE_DATA = 0x5642AB01;

TellVersion()
{
    llSay(0,VERSION_STRING);
}

TellMemory()
{
    llSay(0,ShortScriptName + " - " + (string)llGetFreeMemory() + " bytes memory free.");
}

integer IsValidInventory(string name)
{
    if(llGetSubString(name,0,0) == "#")
        return TRUE;
    if(llGetSubString(name,0,1) == "!!")
        return TRUE;
    if(llGetSubString(name,-6,-1) == " - Bio")
    {
        if(llGetInventoryType(name) != INVENTORY_NOTECARD)
        {
            llSay(0,"Invalid inventory item '" + name + "'. Item is not a notecard. Inventory item will be removed!");
            return FALSE;
        }
        string av_name = llGetSubString(name,0,-7);
        integer type = llGetInventoryType(av_name + " - Config");
        if(type == INVENTORY_NONE || type != INVENTORY_NOTECARD)
        {
            llSay(0,"Invalid inventory item '" + av_name + " - Config'. Item is not a notecard or missing. Inventory item will be removed!");
            return FALSE;
        }
        type = llGetInventoryType(av_name + " - Pic");
        if(type == INVENTORY_NONE || type != INVENTORY_TEXTURE)
        {
            llSay(0,"Invalid inventory item '" + av_name + " - Pic'. Item is not a texture or missing. Inventory item will be removed!");
            return FALSE;
        }
        return TRUE;
    }
    else if(llGetSubString(name,-6,-1) == " - Pic")
    {
        if(llGetInventoryType(name) != INVENTORY_TEXTURE)
        {
            llSay(0,"Invalid inventory item '" + name + "'. Item is not a texture. Inventory item will be removed!");
            return FALSE;
        }
        string av_name = llGetSubString(name,0,-7);
        integer type = llGetInventoryType(av_name + " - Config");
        if(type == INVENTORY_NONE || type != INVENTORY_NOTECARD)
        {
            llSay(0,"Invalid inventory item '" + av_name + " - Config'. Item is not a notecard or missing. Inventory item will be removed!");
            return FALSE;
        }
        type = llGetInventoryType(av_name + " - Bio");
        if(type == INVENTORY_NONE || type != INVENTORY_NOTECARD)
        {
            llSay(0,"Invalid inventory item '" + av_name + " - Bio'. Item is not a notecard or missing. Inventory item will be removed!");
            return FALSE;
        }
        return TRUE;
    }
    else if(llGetSubString(name,-9,-1) == " - Config")
    {
        if(llGetInventoryType(name) != INVENTORY_NOTECARD)
        {
            llSay(0,"Invalid inventory item '" + name + "'. Item is not a notecard. Inventory item will be removed!");
            return FALSE;
        }
        string av_name = llGetSubString(name,0,-10);
        integer type = llGetInventoryType(av_name + " - Bio");
        if(type == INVENTORY_NONE || type != INVENTORY_NOTECARD)
        {
            llSay(0,"Invalid inventory item '" + av_name + " - Bio'. Item is not a notecard or missing. Inventory item will be removed!");
            return FALSE;
        }
        type = llGetInventoryType(av_name + " - Pic");
        if(type == INVENTORY_NONE || type != INVENTORY_TEXTURE)
        {
            llSay(0,"Invalid inventory item '" + av_name + " - Pic'. Item is not a texture or missing. Inventory item will be removed!");
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}


CheckInventory()
{
    llSay(0,"Verifying inventory data...");
    integer cnt = llGetInventoryNumber(INVENTORY_ALL);
    integer i = 0;
    list RemInv = [];
    for(; i < cnt; ++i)
    {
        string name = llGetInventoryName(INVENTORY_ALL,i);
        if(llGetInventoryType(name) == INVENTORY_TEXTURE)
        {
            if(llGetInventoryKey(name) == NULL_KEY)
            {
                llSay(0,"Please check the rights for '" + name + "'. Item will be removed!");
                RemInv += [name];
            }
        }
        if(!IsValidInventory(name))
        {
            RemInv += [name];   
        }
    }
    cnt = llGetListLength(RemInv);
    for(i = 0; i < cnt; ++i)
    {
        llRemoveInventory(llList2String(RemInv,i));
    }
    llSay(0,"Inventory data verified.");    
}

integer NOTECARD_CUR = 0;
integer NOTECARD_CNT = 0;
key     REQ_ID       = NULL_KEY;
key     NAME_REQ_ID  = NULL_KEY;

CheckNextNotecard()
{
    if(NOTECARD_CUR < NOTECARD_CNT)
    {
        string name = llGetInventoryName(INVENTORY_NOTECARD,NOTECARD_CUR);
        ++NOTECARD_CUR;
        if(llGetSubString(name,-9,-1) == " - Config")
        {            
            REQ_ID = llGetNotecardLine(name,0);
        }
        else
        {
            CheckNextNotecard();   
        }
    }
    else
    {
        llSay(0,"Configuration files verified!");
        llMessageLinked(LINK_SET,CMD_UPDATE_DATA,"",NULL_KEY);
        if(TRUE)
        {
            state default;
        }
    }
}

CheckConfigurationItems()
{
    llSay(0,"Starting to verify configuration files...");
    NOTECARD_CNT = llGetInventoryNumber(INVENTORY_NOTECARD);
    NOTECARD_CUR = 0;
    CheckNextNotecard();
}

integer ONCE = TRUE;
default
{
    link_message(integer sender, integer ival, string sval, key kval)
    {
        if(ival == CMD_CHECK_INVENTORY)
            state check;
        else if(ival == CMD_RESTART)
            llResetScript();
        else if(ival == CMD_GET_MEMORY)
            TellMemory();
        else if(ival == CMD_GET_VERSION)
            TellVersion();    
    }
}

state check
{
    state_entry()
    {
        CheckInventory();
        CheckConfigurationItems();
    }    
    
    timer()
    {
        string name = llGetSubString(llGetInventoryName(INVENTORY_NOTECARD,NOTECARD_CUR - 1),0,-10);
        llSay(0,"Config Failure: Key or Avatar Name not correct! removing items for " + name);
        llRemoveInventory(name + " - Pic");
        llRemoveInventory(name + " - Config");
        llRemoveInventory(name + " - Bio");
        state restart;        
    }
                
    dataserver(key qid, string data)
    {
        if(REQ_ID == qid)
        {
            if(data == EOF)
            {
                string name = llGetSubString(llGetInventoryName(INVENTORY_NOTECARD,NOTECARD_CUR - 1),0,-10);
                llSay(0,"Config Failure removing items for " + name);
                llRemoveInventory(name + " - Pic");
                llRemoveInventory(name + " - Config");
                llRemoveInventory(name + " - Bio");
                state restart;
            }
            else
            {
                NAME_REQ_ID = llRequestAgentData((key)data,DATA_NAME);
                llSetTimerEvent(30.);
            }
        }
        else if(NAME_REQ_ID == qid)
        {
            llSetTimerEvent(0.);
            string name = llGetSubString(llGetInventoryName(INVENTORY_NOTECARD,NOTECARD_CUR - 1),0,-10);
            if(name == data)
            {
                CheckNextNotecard();          
            }
            else
            {
                llSay(0,"Config Failure: Key or Avatar Name not correct! removing items for " + name);
                llRemoveInventory(name + " - Pic");
                llRemoveInventory(name + " - Config");
                llRemoveInventory(name + " - Bio");
                state restart;
            }
        }
    }

    link_message(integer sender, integer ival, string sval, key kval)
    {
        if(ival == CMD_RESTART)
            llResetScript();
        else if(ival == CMD_GET_MEMORY)
            TellMemory();
        else if(ival == CMD_GET_VERSION)
            TellVersion();    
    }
}

state restart
{
    state_entry()
    {
        llSay(0,"Restarting check...");
        state check;
    }    
    
    link_message(integer sender, integer ival, string sval, key kval)
    {
        if(ival == CMD_RESTART)
            llResetScript();
        else if(ival == CMD_GET_MEMORY)
            TellMemory();
        else if(ival == CMD_GET_VERSION)
            TellVersion();    
    }
}

