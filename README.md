# VPP_flow_table
Modification within VPP src for flow classification and determining active flows
$VPP_ROOT = /your/vpp/directory/ ;
1. Copy the flow_table.h to $VPP_ROOT/src/vnet/ip/
2. "#include <vnet/ip/flow_table.h>" //add this line to ip4_forward.c in $VPP_ROOT/src/vnet/ip ;
3. Find the function " ip4_rewrite_inline in ip4_forward.c"
4. Modify the function
    1. Please check the function in this repository for changes that are to be made.
    2. look for the comments "start of extra code; end of extra code" to find what are the change to be made.

#CLI COMMANDS

1.show flow counters - To show all the flows registered in the flow table.

2.show activeflow counters - To show only the flows which appeared in the last 15million packets.

3.show flow number - To show the number of flows in the flow table.

4.clear flow counters - To clear the flow table.
