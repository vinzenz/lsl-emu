string version_string = "Virtuosic Bytes Advanced Rotating Billboard 2.0 (no copy/trans)";
integer update_channel = -4711;
integer script_pin = 20021981;

list scripts_to_update = ["#Adboard Controller 3.01", 1,
                          "#Update Check", 1,
                          "Ad Board Turn1 3.0",2,
                          "Ad Board Turn2 3.0",3,
                          "Ad Board Turn3 3.0",4,
                          "Ad Board Turn4 3.0",5,
                          "Ad Board Turn5 3.0",6,
                          "Ad Board Turn6 3.0",7,
                          "Ad Board Turn7 3.0",8,
                          "Ad Board Turn8 3.0",9,
                          "Ad Board Turn9 3.0",10,
                          "Ad Board Turn10 3.0",11];

integer pdone;
integer step;

update_step(integer i, list data)
{
    llSleep(1);  
    pdone += step;
    llSetText("Update in progress\n" + (string)pdone + "% completed...",<1.,0.,0.>,1.); 
    llRemoteLoadScriptPin(llList2Key(data,llList2Integer(scripts_to_update,i*2+1)),llList2String(scripts_to_update,i*2),script_pin,TRUE,0);
}

update_progress(list data)
{
    pdone = 0;
    integer i = 0; 
    integer steps = llGetListLength(scripts_to_update)/2;
    step = 100/steps;
    
    llSetText("Starting update...",<1.,1.,0.>,1.);
    for(;i < steps; ++i)
    {
        update_step(i,data);
    }        
    llSetText("Update done. Waiting for the next board...",<0.,1.,0.>,1.);
}

default
{
    state_entry()
    {
        llSetText("Waiting for the next board...",<0.,1.,0.>,1.);
        llListen(update_channel,"",NULL_KEY,"");
        llShout(update_channel,"VB_UPDATE");
    }
    
    on_rez(integer i)
    {
        llResetScript();
    }
    
    listen(integer channel, string name, key id, string data)
    {
        if(channel == update_channel)
        {
            list data_list = llCSV2List(data);
            if(llList2String(data_list,0) == version_string)
                update_progress(data_list);
        }
    }
}

