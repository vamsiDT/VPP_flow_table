# VPP_flow_table

Modification within VPP src for flow classification and determining active flows

$VPP_ROOT = /your/vpp/directory/ ;

1. Copy the flow_table.h to $VPP_ROOT/src/vnet/ip/

2. "#include <vnet/ip/flow_table.h>"

   "#include <vnet/ip/flow_table_var.h>"

   "#include <vnet/ip/flow_table_cli.h>"
   
   //add these lines to ip4_forward.c in $VPP_ROOT/src/vnet/ip ;

3. Find the function " ip4_rewrite_inline" in ip4_forward.c

4. Modify the function
    1. Please check the function in this repository for changes that are to be made.
    2. look for the comments "start of extra code; end of extra code" to find what are the change to be made.

#CLI COMMANDS

    1.show flow counters - To show all the flows registered in the flow table.

    2.show flow number - To show the number of flows in the flow table.

    3.clear flow counters - To clear the flow table.

#Description

    1.Currently the table size is 1024*4;    
    
#TODO

    1.Choose a good hash function for this application and update;
    2.Add an option to enable or disable the classification .
    3.Also add option to enable or disable only on a specific interface.
    4.Extend this to multi-threading scenario with multiple tx-queues.
