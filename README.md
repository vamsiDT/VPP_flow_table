# VPP_flow_table

* VPP (Vector Packet Processor) is a high-speed packet processing engine which runs in user-space and implements kernel bypass techniques to achieve high-speed packet processing.
* This is a modification within VPP src for flow classification,determining active flows and fair-dropping.
* Fair-drop is implemented at the input side of VPP forwarding graph. It is implemented in ip4-input node of VPP which is the first node after dpdk-input for ipv4 traffic.
* RSS-hash result from hardware which is stored in mbufs as hash.rss is used to classify flows.
* Currently the RSS key used is the default intel 82599 rss hash key provided in the datasheet. This key is also available by default in dpdk package.

----

## How to use this repository

   * $VPP_ROOT = /your/vpp/directory/ ;

   1. Copy flow_table.h, flow_table_var.h, ip4_input.c to $VPP_ROOT/src/vnet/ip/
   2. Header files to be added in ip4_input.c :
      i. #include <vnet/ip/flow_table.h>
      ii. #include <vnet/ip/flow_table_var.h>
      iii. #include <vnet/ip/flow_table_cli.h>
   3. Find the function " ip4_input_inline" in ip4_input.c
   4. Modify the function
      i. Please check the function in this repository for changes that are to be made.
      ii. look for the comments "start of extra code; end of extra code" to find the changes to be made.

#TODO

    1.Choose a good hash function for this application and update;
    2.Add an option to enable or disable the fair-drop scheduling.
    3.Also add option to enable or disable only on a specific interface.
    4.Extend this to multi-threading scenario with multiple tx-queues.
    5.Update README.md :P 
