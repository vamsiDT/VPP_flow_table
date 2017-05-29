#include <vnet/ip/flow_table.h>

/**************************************************************CLI FUNCTIONS AND COMMANDS**************************************************************/

/*****Function to display all the flows in the flow table*****/

static clib_error_t *
show_flow_counters_fn (vlib_main_t * vm,
                          unformat_input_t * input, vlib_cli_command_t * cmd)
{
	clib_error_t *error = 0;
	vlib_cli_output (vm, "Flow Counters ID\tInterface_TX\tSrc_ip\t\t\tDst_ip\t\tSrc_Port\t\tDst_Port\n");
    u32 count = 0;
    flowcount_t *  current;
    current = head;
    flowcount_t * branch;
    u32 ifindex;
    u32 srcip;
    u32 dstip;
    u32 srcport;
    u32 dstport;
    int pktlen;

   while (current != NULL && count < numflows){
        count++;
        ifindex = (current->swsrcdstport)>>32 ;
        srcip = (u32)((current->srcdst)>>32);
        dstip = (u32)((current->srcdst)&0x00000000ffffffff) ;
        srcport = (u16)(((current->swsrcdstport)&0x00000000ffffffff)>>16) ;
        dstport = (u16)((current->swsrcdstport)&0x000000000000ffff) ;
        pktlen = current->npackets;
        
        vlib_cli_output (vm, "%d\t\t%d\t\t\tSrc:%U\tDst:%U\tSrcPort:%u\t\tDstPort:%u\t\tSize:%d\n",count,(ifindex),format_ip4_address,&(srcip), format_ip4_address,&(dstip),clib_net_to_host_u16(srcport),clib_net_to_host_u16(dstport),pktlen);

        branch = current->branchnext;
        while(branch != NULL && branch != current){
        	count++;
        	ifindex = (branch->swsrcdstport)>>32 ;
            srcip = (u32)((branch->srcdst)>>32);
            dstip = (u32)((branch->srcdst)&0x00000000ffffffff) ;
            srcport = (u16)(((branch->swsrcdstport)&0x00000000ffffffff)>>16) ;
            dstport = (u16)((branch->swsrcdstport)&0x000000000000ffff) ;
            pktlen = branch->npackets;
            vlib_cli_output (vm, "%d\t\t%d\t\t\tSrc:%U\tDst:%U\tSrcPort:%u\t\tDstPort:%u\t\tSize:%d\n",count,(ifindex),format_ip4_address,&(srcip), format_ip4_address,&(dstip),clib_net_to_host_u16(srcport),clib_net_to_host_u16(dstport),pktlen);
            
            branch = branch->branchnext;
                    
		}
        current = current->next;
        }

        return error;

}



/*****Function to display number of flows in the flow table*****/

static clib_error_t *
show_flow_number_fn (vlib_main_t * vm,
                          unformat_input_t * input, vlib_cli_command_t * cmd)
{
    clib_error_t *error = 0;
    vlib_cli_output (vm, "Number of Flows:%d\n",numflows);
        return error;
}

/*****Function to clear all the flows in the flow table and set the count to 0*****/

static clib_error_t *
clear_flow_counters_fn (vlib_main_t * vm,
                          unformat_input_t * input, vlib_cli_command_t * cmd)
{
	clib_error_t *error = 0;
	for ( int i =0;i<TABLESIZE;i++){
		for (int j =0;j<4;j++){
			free(nodet[i]+j);
			(nodet[i]) = NULL;
			}
		}
		//free(head);
		//free(tail);
		//free(previousnode);
		head = NULL;
		tail = NULL;
		previousnode = NULL;
		numflows=0;
        return error;
}




/*****Function to show active flows in the flow table with in the packetstamp range -refresh to refresh*****/
/*
static clib_error_t *
show_activeflow_counters_fn (vlib_main_t * vm,
                          unformat_input_t * input, vlib_cli_command_t * cmd)
{
    clib_error_t *error = 0;
    flowcount_t * branch;
    flowcount_t * current;
    current = head;
    u8 count = 0;
    u32 ifindex;
    u32 srcip;
    u32 dstip;
    u32 srcport;
    u32 dstport;
    int pktlen;
    vlib_cli_output (vm, "Flow Counters ID\tInterface_TX\tSrc_ip\t\t\tDst_ip\t\tSrc_Port\t\tDst_Port\t\tSize\n");
    
        while (current != NULL){
            if(current->npackets > 0){
                ifindex = (current->swsrcdstport)>>32 ;
                srcip = (u32)((current->srcdst)>>32);
                dstip = (u32)((current->srcdst)&0x00000000ffffffff) ;
                srcport = (u16)(((current->swsrcdstport)&0x00000000ffffffff)>>16) ;
                dstport = (u16)((current->swsrcdstport)&0x000000000000ffff) ;
                pktlen = current->npackets;
                vlib_cli_output (vm, "%d\t\t%d\t\t\tSrc:%U\tDst:%U\tSrcPort:%u\t\tDstPort:%u\t\tSize:%d\n",count,(ifindex),format_ip4_address,&(srcip), format_ip4_address,&(dstip),clib_net_to_host_u16(srcport),clib_net_to_host_u16(dstport),pktlen);
                count++;
            }
            branch = current->branchnext;
            while(branch != NULL && branch != current){
                if(branch->npackets > 0){
                    ifindex = (branch->swsrcdstport)>>32 ;
                    srcip = (u32)((branch->srcdst)>>32);
                    dstip = (u32)((branch->srcdst)&0x00000000ffffffff) ;
                    srcport = (u16)(((branch->swsrcdstport)&0x00000000ffffffff)>>16) ;
                    dstport = (u16)((branch->swsrcdstport)&0x000000000000ffff) ;
                    pktlen = branch->npackets;
                    vlib_cli_output (vm, "%d\t\t%d\t\t\tSrc:%U\tDst:%U\tSrcPort:%u\t\tDstPort:%u\t\tSize:%d\n",count,(ifindex),format_ip4_address,&(srcip), format_ip4_address,&(dstip),clib_net_to_host_u16(srcport),clib_net_to_host_u16(dstport),pktlen);
                    count++;
                }
                branch = branch->branchnext;
            }
            current = current->next;
        }
            
        return error;
        
}

*/
static clib_error_t *
show_activeflow_counters_fn (vlib_main_t * vm,
                          unformat_input_t * input, vlib_cli_command_t * cmd)
{
    clib_error_t *error = 0;
    int i = 0;
    u32 ifindex;
    u32 srcip;
    u32 dstip;
    u32 srcport;
    u32 dstport;
    int pktlen;
    vlib_cli_output (vm, "Flow Counters ID\tInterface_TX\tSrc_ip\t\t\tDst_ip\t\tSrc_Port\t\tDst_Port\t\tSize\n");
    
    while (activeflows[i] != NULL && i < VLIB_FRAME_SIZE){    
        ifindex = (activeflows[i]->swsrcdstport)>>32 ;
        srcip = (u32)((activeflows[i]->srcdst)>>32);
        dstip = (u32)((activeflows[i]->srcdst)&0x00000000ffffffff) ;
        srcport = (u16)(((activeflows[i]->swsrcdstport)&0x00000000ffffffff)>>16) ;
        dstport = (u16)((activeflows[i]->swsrcdstport)&0x000000000000ffff) ;
        pktlen = activeflows[i]->npackets;
        vlib_cli_output (vm, "%d\t\t%d\t\t\tSrc:%U\tDst:%U\tSrcPort:%u\t\tDstPort:%u\t\tSize:%d\n",i,(ifindex),format_ip4_address,&(srcip), format_ip4_address,&(dstip),clib_net_to_host_u16(srcport),clib_net_to_host_u16(dstport),pktlen);
        i++;
    }
    return error;
}



/*****CLI commands*****/

VLIB_CLI_COMMAND (show_flow_counters_command, static) = {
  .path = "show flow counters",
  .short_help = "show flow counters",
  .function = show_flow_counters_fn,
};


VLIB_CLI_COMMAND (show_activeflow_counters_command, static) = {
  .path = "show activeflow counters",
  .short_help = "show activeflow counters",
  .function = show_activeflow_counters_fn,
};

VLIB_CLI_COMMAND (show_numberof_flows_command, static) = {
  .path = "show flow number",
  .short_help = "show flow number",
  .function = show_flow_number_fn,
};

VLIB_CLI_COMMAND (clear_flowcounters_command, static) = {
  .path = "clear flow counters",
  .short_help = "clear flow counters",
  .function = clear_flow_counters_fn,
};

/*
*	"Gather ye rosebuds while ye may"
*				- Mike Portnoy
*
*	End
*
*/
