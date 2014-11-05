string version_string = "Virtuosic Bytes Advanced Rotating Billboard 2.0 (no copy/trans)";
integer update_channel = -4711;

list rotnames = ["Rot1","Rot2","Rot3","Rot4","Rot5","Rot6","Rot7","Rot8","Rot9","Rot10"];


send_check()
{
    llHTTPRequest("http://s15235104.onlinehome-server.info/updates/product_update.py?version=" + llEscapeURL(version_string),[],"");
}

default
{
    state_entry()
    {
        llSetRemoteScriptAccessPin(20021981);
        llListen(update_channel,"",NULL_KEY,"VB_UPDATE");
        send_check();
        llSetTimerEvent(168.0*3600.0);
    }
    
    on_rez(integer i)
    {
        send_check();
        llSetTimerEvent(72.0*3600.0);
    }
    
    timer()
    {
        send_check();
    }
    
    listen(integer channel, string name, key id, string cmd)
    {
        if(channel == update_channel && cmd == "VB_UPDATE")
        {
            list keys = [NULL_KEY,NULL_KEY,NULL_KEY,NULL_KEY,NULL_KEY,NULL_KEY,NULL_KEY,NULL_KEY,NULL_KEY,NULL_KEY];
            integer cnt = llGetNumberOfPrims()+1;
            integer i = 1;
            for(;i < cnt; ++i)
            {
                string name = llGetLinkName(i);
                integer child_cnt = llGetListLength(rotnames);
                integer j = 0;
                for(;j< child_cnt; ++j)
                {
                    if(name == llList2String(rotnames,j))
                    {
                        keys = llListReplaceList(keys,[llGetLinkKey(i)],j,j);
                    }
                }
            }
            keys = [version_string,llGetKey()] + keys;            
            llShout(update_channel,llList2CSV(keys));
        }
    }
}

