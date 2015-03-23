default {
    state_entry() {
        llOwnerSay("No Stride Integers    ASC: " + llDumpList2String(llListSort([9, 3, 2, 1, 8, 7, 4, 5, 6, 0], 1, TRUE), ", "));
        llOwnerSay("No Stride Integers   DESC: " + llDumpList2String(llListSort([9, 3, 2, 1, 8, 7, 4, 5, 6, 0], 1, FALSE), ", "));
        llOwnerSay("No Stride Vectors     ASC: " + llDumpList2String(llListSort([<1., 2, .3>, <2., 3., 4.>, <4., 5., 6.>], 1, TRUE), ", "));
        llOwnerSay("No Stride Vectors    DESC: " + llDumpList2String(llListSort([<1., 2, .3>, <2., 3., 4.>, <4., 5., 6.>], 1, FALSE), ", "));
        llOwnerSay("No Stride Rotations   ASC: " + llDumpList2String(llListSort([<1., 2, .3, 1.>, <2., 3., 4., 1.>, <4., 5., 6., 1.>], 1, TRUE), ", "));
        llOwnerSay("No Stride Rotations  DESC: " + llDumpList2String(llListSort([<1., 2, .3, 1.>, <2., 3., 4., 1.>, <4., 5., 6., 1.>], 1, FALSE), ", "));
        llOwnerSay("No Stride Strings     ASC: " + llDumpList2String(llListSort(["Hello", "to", "the", "virtual", "metaverse"], 1, TRUE), ", "));
        llOwnerSay("No Stride Strings    DESC: " + llDumpList2String(llListSort(["Hello", "to", "the", "virtual", "metaverse"], 1, FALSE), ", "));
        llOwnerSay("No Stride Mixed       ASC: " + llDumpList2String(llListSort(["Hello", 1, <1., 2., 3.>, .44444, <9., 3., 4444., .33232>], 1, TRUE), ", "));
        llOwnerSay("No Stride Mixed      DESC: " + llDumpList2String(llListSort(["Hello", 1, <1., 2., 3.>, .44444, <9., 3., 4444., .33232>], 1, FALSE), ", "));

        llOwnerSay("Stride 2 Integer    ASC: " + llDumpList2String(llListSort([2, "C", 0, "A", 3, "D", 1, "B"], 2, TRUE), ", "));
        llOwnerSay("Stride 2 Integer   DESC: " + llDumpList2String(llListSort([2, "C", 0, "A", 3, "D", 1, "B"], 2, FALSE), ", "));

        llOwnerSay("Stride 3 Integer    ASC: " + llDumpList2String(llListSort([2, "C", 4., 0, "A", 0., 3, "D", 9., 1, "B", 1.], 3, TRUE), ", "));
        llOwnerSay("Stride 3 Integer   DESC: " + llDumpList2String(llListSort([2, "C", 4., 0, "A", 0., 3, "D", 9., 1, "B", 1.], 3, FALSE), ", "));

        llOwnerSay("Unaligned Stride 3 Integer    ASC: " + llDumpList2String(llListSort([2, "C", 4., 0, "A", 0., 3, "D", 9., 1], 3, TRUE), ", "));
        llOwnerSay("Unaligned Stride 3 Integer   DESC: " + llDumpList2String(llListSort([2, "C", 4., 0, "A", 0., 3, "D", 9., 1], 3, FALSE), ", "));

        llOwnerSay("Unaligned Stride 4 Integer    ASC: " + llDumpList2String(llListSort([2, "C", 4., 0, "A", 0., 3, "D", 9., 1], 4, TRUE), ", "));
        llOwnerSay("Unaligned Stride 4 Integer   DESC: " + llDumpList2String(llListSort([2, "C", 4., 0, "A", 0., 3, "D", 9., 1], 4, FALSE), ", "));

        llOwnerSay("Stride 2 Mixed       ASC: " + llDumpList2String(llListSort(["Hello", 1, <1., 2., 3.>, .44444, <9., 3., 4444., .33232>, 33], 2, TRUE), ", "));
        llOwnerSay("Stride 2 Mixed      DESC: " + llDumpList2String(llListSort(["Hello", 1, <1., 2., 3.>, .44444, <9., 3., 4444., .33232>, 33], 2, FALSE), ", "));
    }

}
