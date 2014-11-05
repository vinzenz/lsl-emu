string ShortScriptName  = "Data Storage I";
string VERSION_STRING   = "Escort Wall Server - Data Storage I 1.00";

integer CMD_RESTART     = 0x5642FF01;
integer CMD_GET_MEMORY  = 0x5642FF02;
integer CMD_GET_VERSION = 0x5642FF03;
integer CMD_DEBUG_ON    = 0x5642FF04;
integer CMD_DEBUG_OFF   = 0x5642FF05;
integer DEBUG_ON = FALSE;


integer CMD_ADD_DATA     = 0x5642A101;
integer CMD_GET_DATA     = 0x5642A102;
integer CMD_RPL_DATA     = 0x5642A103;
integer CMD_GET_ITEM_CNT = 0x5642A104;
integer CMD_RPL_ITEM_CNT = 0x5642A105;
integer CMD_SEND_DATA    = 0x5642A106;
integer CMD_FLUSH        = 0x5642A107;

list data = [];

TellMemory()
{
    llSay(0,ShortScriptName + " - " + (string)llGetListLength(data) + " items saved. " + (string)llGetFreeMemory() + " bytes memory free");    
}

TellVersion()
{
    llSay(0,VERSION_STRING);
}

SendData(string recipient)
{
    if(llGetListLength(data))
    {
        string tmp = (string)llGetListLength(data) + "|1\n";
        llEmail(recipient,"Escort Data",tmp + llDumpList2String(data,"\n"));
    }
}

default
{
    link_message(integer sender, integer ival, string sval, key kval)
    {
        if(ival == CMD_RESTART)
            llResetScript();
        else if(ival == CMD_ADD_DATA)
            data += [sval];
        else if(ival == CMD_GET_ITEM_CNT)
            llMessageLinked(LINK_SET,CMD_RPL_ITEM_CNT,(string)llGetListLength(data),NULL_KEY);
        else if(ival == CMD_GET_MEMORY)
            TellMemory();
        else if(ival == CMD_GET_VERSION)
            TellVersion();
        else if(ival == CMD_SEND_DATA)
            SendData(sval);
        else if(ival == CMD_FLUSH)
            data = [];
    }
}

