list DATA = [];

list CARDS = [
    0x00B, 0x012, 0x023, 0x034, 0x045, 0x056, 0x067, 0x078, 0x089, 0x09A, 0x0AA, 0x0BA, 0x0CA,
    0x10B, 0x112, 0x123, 0x134, 0x145, 0x156, 0x167, 0x178, 0x189, 0x19A, 0x1AA, 0x1BA, 0x1CA,
    0x20B, 0x212, 0x223, 0x234, 0x245, 0x256, 0x267, 0x278, 0x289, 0x29A, 0x2AA, 0x2BA, 0x2CA,
    0x30B, 0x312, 0x323, 0x334, 0x345, 0x356, 0x367, 0x378, 0x389, 0x39A, 0x3AA, 0x3BA, 0x3CA
];

list STACK = [];

new_round() {
    STACK = CARDS;
}

integer deal_card() {
    integer card = (integer)llFrand((float)llGetListLength(STACK));
    integer res = llList2Integer(STACK, card);
    STACK = llDeleteSubList(STACK, card, card);
    return res;
}

list PLAYERCARDS1 = [];
list PLAYERCARDS2 = [];
list DEALERCARDS = [];

integer calc_value(list v) {
    integer c = llGetListLength(v);
    integer i = 0;
    integer s = 0;
    integer c11 = 0;
    for(;i < c; ++i) {
        integer e = llList2Integer(v, i);
        if(e == 11) c11 += 1;
        s += (e & 0xF);
    }
    if(s > 21 && c11 > 0) {
        integer ns = s;
        while(ns > 21 && c11 > 0) {
            ns -= 10;
            --c11;
        }
        return ns;
    }
    return s;
}

default {
    state_entry(){
        new_round();
        PLAYERCARDS1 = [deal_card(), deal_card(), deal_card()];
        DEALERCARDS = [deal_card(), deal_card(), deal_card()];
        llOwnerSay(llDumpList2String(["Player has: ", calc_value(PLAYERCARDS1)], " "));
        llOwnerSay(llDumpList2String(["Dealer has: ", calc_value(DEALERCARDS)], " "));
    }
}
