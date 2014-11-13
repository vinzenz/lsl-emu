integer FIRST = TRUE;

default {
    state_entry(){
        llSay(0, "Default entry");
        if(FIRST) {
            llSay(0, "Yielded true");
            state foobar;
        }
        else {
            llSay(0, "Yielded false");
        }

        integer i = 0;
        for(i = 0; i < 10; ++i) {
            llSay(0, "Hello LSL");
        }
    }
}

state foobar {
    state_entry(){
        llSay(0, "foobar entry");
        FIRST = FALSE;
        state default;
    }
}
