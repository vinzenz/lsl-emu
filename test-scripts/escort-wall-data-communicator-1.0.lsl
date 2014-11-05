string  ShortScriptName = "Communicator";
string  VERSION_STRING  = "Escort Wall Server - Communicator 1.00";

integer CMD_RESTART     = 0x5642FF01;
integer CMD_GET_MEMORY  = 0x5642FF02;
integer CMD_GET_VERSION = 0x5642FF03;
integer CMD_DEBUG_ON    = 0x5642FF04;
integer CMD_DEBUG_OFF   = 0x5642FF05;
integer DEBUG_ON = FALSE;

integer CMD_ADD_CLIENT      = 0x5642B101;
integer CMD_ADD_EOW         = 0x5642B102;
integer CMD_INITIATE_UPDATE = 0x5642B103;

list CLIENTS = [];
list EOWS    = [];

integer CMD_SEND_DATA1    = 0x5642A106;
integer CMD_SEND_DATA2    = 0x5642A206;
integer CMD_SEND_DATA3    = 0x5642A306;
integer CMD_SEND_DATA4    = 0x5642A406;
integer CMD_SEND_DATA5    = 0x5642A506;

list EOW_DATA = [];
integer CUR_EOW = 0;
integer UPDATE = FALSE;

TellVersion()
{
    llSay(0,VERSION_STRING);
}

TellMemory()
{
    llSay(0,ShortScriptName + " - " + (string)llGetFreeMemory() + " bytes memory free.");
}

SendUpdate(string address)
{
    llMessageLinked(LINK_SET,CMD_SEND_DATA1,address,NULL_KEY);
    llMessageLinked(LINK_SET,CMD_SEND_DATA2,address,NULL_KEY);
    llMessageLinked(LINK_SET,CMD_SEND_DATA3,address,NULL_KEY);
    llMessageLinked(LINK_SET,CMD_SEND_DATA4,address,NULL_KEY);
    llMessageLinked(LINK_SET,CMD_SEND_DATA5,address,NULL_KEY);
}

SendEOW(string address)
{
    string name = llGetObjectDesc();
    if(llGetInventoryType(name + " - Pic") == INVENTORY_TEXTURE)
    {
        EOW_DATA = [];
        EOW_DATA += [address];                        
        EOW_DATA += [llGetInventoryKey(name + " - Pic")];
        EOW_DATA += [llGetNotecardLine(name + " - Config",0)];
        llMessageLinked(LINK_SET,CMD_SEND_DATA5,address,NULL_KEY);
    }
}

VendorBio(string data)
{
    llSay(0,"VendorBio(" + data + ")");
    list keys = llParseString2List(data,["|"],[]);
    key tex_key = (key)llList2String(keys,0);
    integer cnt = llGetInventoryNumber(INVENTORY_TEXTURE);
    integer i = 0;
    for(; i <cnt; ++i)
    {
        if(tex_key == llGetInventoryKey(llGetInventoryName(INVENTORY_TEXTURE,i)))
        {
            string name = llGetSubString(llGetInventoryName(INVENTORY_TEXTURE,i),0,-7);
            llGiveInventory((key)llList2String(keys,1),name + " - Bio");
        }
    }
}

SendUpdates()
{
    integer cnt = llGetListLength(CLIENTS);
    integer i = 0;
    for(; i < cnt; ++i)
        SendUpdate(llList2String(CLIENTS,i));
    cnt = llGetListLength(EOWS);
    if(cnt)
    {
        CUR_EOW = 0;
        UPDATE = TRUE;
        SendEOW(llList2String(EOWS,0));
    }
}

default
{
    state_entry()
    {
    }
    
    timer()
    {
        llGetNextEmail("","");
    }
    
    link_message(integer sender, integer ival, string sval, key kval)
    {
        if(ival == CMD_RESTART)
            llResetScript();
        else if(ival == CMD_GET_MEMORY)
            TellMemory();
        else if(ival == CMD_GET_VERSION)
            TellVersion();
        else if(ival == CMD_DEBUG_ON)
            DEBUG_ON = TRUE;
        else if(ival == CMD_DEBUG_OFF)
            DEBUG_ON = FALSE;
        else if(ival == CMD_ADD_CLIENT)
            CLIENTS += [sval + "@lsl.secondlife.com"];
        else if(ival == CMD_ADD_EOW)
            EOWS += [sval + "@lsl.secondlife.com"];
        else if(ival == CMD_INITIATE_UPDATE)
        {
            SendUpdates();
            llSetTimerEvent(30.);
        }
    }

    dataserver(key qid, string data)
    {
        if(llGetListLength(EOW_DATA))
        {
            if(llList2Key(EOW_DATA,2) == qid && data != EOF)
            {
                llEmail(llList2String(EOW_DATA,0),"EOW DATA",data + "|" + llList2String(EOW_DATA,1));
            }
            EOW_DATA = [];
            
            if(UPDATE)
            {
                ++CUR_EOW;
                if(CUR_EOW < llGetListLength(EOWS))
                {
                    SendEOW(llList2String(EOWS,CUR_EOW));
                }
            }
        }    
    }
            
    email(string ts, string address, string subj, string msg, integer left)
    {
        subj = llToUpper(subj);
        if(subj == "GET ESCORTS")
        {
            SendUpdate(address);
        }
        else if(subj == "DELIVER BIO")
        {
            VendorBio(llDeleteSubString(msg, 0, llSubStringIndex(msg, "\n\n") + 1));
        }
        else if(subj == "GET EOW")
        {
            SendEOW(address);
        }
        
        if(left)
        {
            llGetNextEmail("","");
        }
    }
}
