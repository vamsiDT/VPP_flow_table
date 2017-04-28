/*
*	Flow classification in VPP
*	
*	      flow_table.h
*
*     
*/
#include <vlib/vlib.h>
#include <vnet/ip/ip.h>
#include <vnet/vnet.h>
#include <stdlib.h>
#include <math.h>
#define TABLESIZE 1024

/*Node in the flow table. srcdst is 64 bit divided as |32bitsrcip|32bitdstip| ; swsrcdstport is divided as |32bit swifindex|16bit srcport|16bit dstport|*/
typedef struct flowcount{
	struct flowcount * prev;
	u64 srcdst;
	u64 swsrcdstport;
	int packetstamp;
	int vqueue;
	struct flowcount * branchnext;
    struct flowcount * next;
    struct flowcount * update;
}flowcount_t;

extern flowcount_t *  nodet[TABLESIZE][4] ;
extern flowcount_t *  head ;
extern flowcount_t *  previousnode;
extern flowcount_t *  tail;
extern int numflows;
extern int packets;
extern int refresh;

flowcount_t *  nodet[TABLESIZE][4] ;
flowcount_t *  head ;
flowcount_t *  previousnode;
flowcount_t *  tail;
int numflows;
int packets;
int refresh = 15000000; 

/* Flow classification function */
always_inline void
flow_table_classify(u32 modulox, u64 hashx0, u64 hashx1){
if (head == NULL){
	numflows = 0;
	packets = 0;
    nodet[modulox][0] = malloc(sizeof(flowcount_t));
    numflows++;
    (nodet[modulox][0])->srcdst = hashx0;
    (nodet[modulox][0])->swsrcdstport = hashx1;
    (nodet[modulox][0])->update = (nodet[modulox][0]);
    (nodet[modulox][0])->packetstamp = packets;
    head = nodet[modulox][0] ;
    previousnode = head ;
	tail = head;
    }
    else if ( nodet[modulox][0] == NULL ){
    nodet[modulox][0] = malloc(sizeof(flowcount_t));
    numflows++;
    (nodet[modulox][0])->srcdst = hashx0;
    (nodet[modulox][0])->swsrcdstport = hashx1;
    (nodet[modulox][0])->prev = previousnode;
    previousnode->next = nodet[modulox][0];
    previousnode = nodet[modulox][0];
    tail = nodet[modulox][0] ;
    (nodet[modulox][0])->update = (nodet[modulox][0]);
    (nodet[modulox][0])->packetstamp = packets;
    }
    else if  ((nodet[modulox][1]) == NULL)
    {
        if  ( ((nodet[modulox][0])->srcdst != hashx0) || ((nodet[modulox][0])->swsrcdstport != hashx1) ) 
        {
    nodet[modulox][1] = malloc(sizeof(flowcount_t));
    numflows++;
    (nodet[modulox][1])->srcdst = hashx0;
    (nodet[modulox][1])->swsrcdstport = hashx1;
    (nodet[modulox][0])->branchnext = (nodet[modulox][1]);
    (nodet[modulox][1])->packetstamp = packets;
        }
        else
        (nodet[modulox][0])->packetstamp = packets;
    }
    else if ( nodet[modulox][2] == NULL )
    {
        if ( ((nodet[modulox][0])->srcdst != hashx0) || ((nodet[modulox][0])->swsrcdstport != hashx1) ) {
    		if ( ((nodet[modulox][1])->srcdst != hashx0) || ((nodet[modulox][1])->swsrcdstport != hashx1) ) {
    			nodet[modulox][2] = malloc(sizeof(flowcount_t));
    			numflows++;
    			(nodet[modulox][2])->srcdst = hashx0;
    			(nodet[modulox][2])->swsrcdstport = hashx1;
    			(nodet[modulox][1])->branchnext = nodet[modulox][2];
    			(nodet[modulox][2])->packetstamp = packets;
    		}
    		else
    		(nodet[modulox][1])->packetstamp = packets;
        }
        else
        (nodet[modulox][0])->packetstamp = packets;
     }
    else if ( nodet[modulox][3] == NULL ){
        if ( ((nodet[modulox][0])->srcdst != hashx0) || ((nodet[modulox][0])->swsrcdstport != hashx1) ) {
    		if ( ((nodet[modulox][1])->srcdst != hashx0) || ((nodet[modulox][1])->swsrcdstport != hashx1) ) {
    			if ( ((nodet[modulox][2])->srcdst != hashx0) || ((nodet[modulox][2])->swsrcdstport != hashx1) ) {
    				(nodet[modulox][3]) = malloc(sizeof(flowcount_t));
    				numflows++;
    				(nodet[modulox][3])->srcdst = hashx0;
    				(nodet[modulox][3])->swsrcdstport = hashx1;
    				(nodet[modulox][2])->branchnext = nodet[modulox][3];
    				(nodet[modulox][3])->branchnext = nodet[modulox][0];
    				(nodet[modulox][3])->packetstamp = packets;
    			}
    			else
    			(nodet[modulox][2])->packetstamp = packets;
        	}
        	else
    		(nodet[modulox][1])->packetstamp = packets;
		}
		else
		(nodet[modulox][0])->packetstamp = packets;
	}


    else
    {
    	if ( ((nodet[modulox][0])->srcdst != hashx0) || ((nodet[modulox][0])->swsrcdstport != hashx1) ) {

    		if ( ((nodet[modulox][1])->srcdst != hashx0) || ((nodet[modulox][1])->swsrcdstport != hashx1) ) {

    			if ( ((nodet[modulox][2])->srcdst != hashx0) || ((nodet[modulox][2])->swsrcdstport != hashx1) ) {

    				if ( ((nodet[modulox][3])->srcdst != hashx0) || ((nodet[modulox][3])->swsrcdstport != hashx1) ) {

    					((nodet[modulox][0])->update)->srcdst = hashx0;
    					((nodet[modulox][0])->update)->swsrcdstport = hashx1;
    					((nodet[modulox][0])->update)->packetstamp = packets;
    					(nodet[modulox][0])->update = ((nodet[modulox][0])->update)->branchnext ;
    				}
    				else
    				(nodet[modulox][3])->packetstamp = packets;
    			}
    			else
    			(nodet[modulox][2])->packetstamp = packets;
    		}
    		else
    		(nodet[modulox][1])->packetstamp = packets;
    	}
    	else
    	(nodet[modulox][0])->packetstamp = packets;
    }
    packets++;

    if (packets > refresh){

        packets = 0;
        flowcount_t * tmpnode;
        flowcount_t * branch;
        tmpnode = tail;
        u32 count = 0;
        while (tmpnode != NULL && count < numflows){
        count++;
        if(tmpnode->packetstamp <= (-refresh))
                        tmpnode->packetstamp = (-refresh);
        else
        tmpnode->packetstamp = (tmpnode->packetstamp)-(refresh);

        branch = tmpnode->branchnext;
    if (branch != NULL && count < numflows){
        count++;
        if(branch->packetstamp <= (-refresh))
                        branch->packetstamp = (-refresh);
        else
        branch->packetstamp = (branch->packetstamp)-(refresh);

        branch = tmpnode->branchnext->branchnext;

        if((branch != NULL && count < numflows)){
            count++;
            if(branch->packetstamp <= (-refresh))
                        branch->packetstamp = (-refresh);
            else
            branch->packetstamp = (branch->packetstamp)-(refresh);
            branch = tmpnode->branchnext->branchnext->branchnext;

            if((branch != NULL) && count < numflows){
            count++;
            if(branch->packetstamp <= (-refresh))
                        branch->packetstamp = (-refresh);
            else
            branch->packetstamp = (branch->packetstamp)-(refresh);
            
            }
        }
        }
        tmpnode = tmpnode->prev;
        }

	}

}

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
    current = tail;
    flowcount_t * branch;
    u32 ifindex;
    u32 srcip;
    u32 dstip;
    u32 srcport;
    u32 dstport;
   while (current != NULL && count < numflows){
        count++;
        ifindex = (current->swsrcdstport)>>32 ;
        srcip = (u32)((current->srcdst)>>32);
        dstip = (u32)((current->srcdst)&0x00000000ffffffff) ;
        srcport = (u16)(((current->swsrcdstport)&0x00000000ffffffff)>>16) ;
        dstport = (u16)((current->swsrcdstport)&0x000000000000ffff) ;
        vlib_cli_output (vm, "%d\t\t%d\t\t\tSrc:%U\tDst:%U\tSrcPort:%u\t\tDstPort:%u\n",count,(ifindex),format_ip4_address,&(srcip), format_ip4_address,&(dstip),clib_net_to_host_u16(srcport),clib_net_to_host_u16(dstport));

        branch = current->branchnext;
    if (branch != NULL && count < numflows){
        count++;
        ifindex = (branch->swsrcdstport)>>32 ;
        srcip = (u32)((branch->srcdst)>>32);
        dstip = (u32)((branch->srcdst)&0x00000000ffffffff) ;
        srcport = (u16)(((branch->swsrcdstport)&0x00000000ffffffff)>>16) ;
        dstport = (u16)((branch->swsrcdstport)&0x000000000000ffff) ;
        vlib_cli_output (vm, "%d\t\t%d\t\t\tSrc:%U\tDst:%U\tSrcPort:%u\t\tDstPort:%u\n",count,(ifindex),format_ip4_address,&(srcip), format_ip4_address,&(dstip),clib_net_to_host_u16(srcport),clib_net_to_host_u16(dstport));

        branch = current->branchnext->branchnext;

        if((branch != NULL && count < numflows)){
            count++;
            ifindex = (branch->swsrcdstport)>>32 ;
        	srcip = (u32)((branch->srcdst)>>32);
        	dstip = (u32)((branch->srcdst)&0x00000000ffffffff) ;
        	srcport = (u16)(((branch->swsrcdstport)&0x00000000ffffffff)>>16) ;
        	dstport = (u16)((branch->swsrcdstport)&0x000000000000ffff) ;
            vlib_cli_output (vm, "%d\t\t%d\t\t\tSrc:%U\tDst:%U\tSrcPort:%u\t\tDstPort:%u\n",count,(ifindex),format_ip4_address,&(srcip), format_ip4_address,&(dstip),clib_net_to_host_u16(srcport),clib_net_to_host_u16(dstport));

            branch = current->branchnext->branchnext->branchnext;

            if((branch != NULL) && count < numflows){
                count++;
                ifindex = (branch->swsrcdstport)>>32 ;
        		srcip = (u32)((branch->srcdst)>>32);
        		dstip = (u32)((branch->srcdst)&0x00000000ffffffff) ;
        		srcport = (u16)(((branch->swsrcdstport)&0x00000000ffffffff)>>16) ;
        		dstport = (u16)((branch->swsrcdstport)&0x000000000000ffff) ;
                vlib_cli_output (vm, "%d\t\t%d\t\t\tSrc:%U\tDst:%U\tSrcPort:%u\t\tDstPort:%u\n",count,(ifindex),format_ip4_address,&(srcip), format_ip4_address,&(dstip),clib_net_to_host_u16(srcport),clib_net_to_host_u16(dstport));
                }
            }
        }
        current = current->prev;
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
			free(nodet[i][j]);
			nodet[i][j] = NULL;
			}
		}
		free(head);
		free(tail);
		free(previousnode);
		head = NULL;
		tail = NULL;
		previousnode = NULL;
		numflows=0;
        return error;
}

/*****Function to show active flows in the flow table with in the packetstamp range -refresh to refresh*****/
static clib_error_t *
show_activeflow_counters_fn (vlib_main_t * vm,
                          unformat_input_t * input, vlib_cli_command_t * cmd)
{
	clib_error_t *error = 0;
	
	vlib_cli_output (vm, "Flow Counters ID\tInterface_TX\tSrc_ip\t\t\tDst_ip\t\tSrc_Port\t\tDst_Port\t\tFlow-Pktstamp/Current-pktstamp\n");
    u32 count = 0;
    flowcount_t *  current;
    current = tail;
    flowcount_t * branch;
    u32 ifindex;
    u32 srcip;
    u32 dstip;
    u32 srcport;
    u32 dstport;
    int pktstamp;
   while (current != NULL && count < numflows){
   		count++;
   		if(current->packetstamp > (-refresh)){
        
        ifindex = (current->swsrcdstport)>>32 ;
        srcip = (u32)((current->srcdst)>>32);
        dstip = (u32)((current->srcdst)&0x00000000ffffffff) ;
        srcport = (u16)(((current->swsrcdstport)&0x00000000ffffffff)>>16) ;
        dstport = (u16)((current->swsrcdstport)&0x000000000000ffff) ;
        pktstamp = (u32)(current->packetstamp);
        vlib_cli_output (vm, "%d\t\t%d\t\t\tSrc:%U\tDst:%U\tSrcPort:%u\t\tDstPort:%u\tFlow-Pktstamp/Current-pktstamp:%d/%d\n",count,(ifindex),format_ip4_address,&(srcip), format_ip4_address,&(dstip),clib_net_to_host_u16(srcport),clib_net_to_host_u16(dstport),pktstamp,packets);
		}
        branch = current->branchnext;
    if (branch != NULL && count < numflows){
    	count++;
    	if(branch->packetstamp > (-refresh)){
        
        ifindex = (branch->swsrcdstport)>>32 ;
        srcip = (u32)((branch->srcdst)>>32);
        dstip = (u32)((branch->srcdst)&0x00000000ffffffff) ;
        srcport = (u16)(((branch->swsrcdstport)&0x00000000ffffffff)>>16) ;
        dstport = (u16)((branch->swsrcdstport)&0x000000000000ffff) ;
        pktstamp = (u32)(branch->packetstamp);
        vlib_cli_output (vm, "%d\t\t%d\t\t\tSrc:%U\tDst:%U\tSrcPort:%u\t\tDstPort:%u\tFlow-Pktstamp/Current-pktstamp:%d/%d\n",count,(ifindex),format_ip4_address,&(srcip), format_ip4_address,&(dstip),clib_net_to_host_u16(srcport),clib_net_to_host_u16(dstport),pktstamp,packets);
		}
        branch = current->branchnext->branchnext;

        if((branch != NULL && count < numflows)){
        	count++;
        	if(branch->packetstamp > (-refresh)){
            
            ifindex = (branch->swsrcdstport)>>32 ;
        	srcip = (u32)((branch->srcdst)>>32);
        	dstip = (u32)((branch->srcdst)&0x00000000ffffffff) ;
        	srcport = (u16)(((branch->swsrcdstport)&0x00000000ffffffff)>>16) ;
        	dstport = (u16)((branch->swsrcdstport)&0x000000000000ffff) ;
        	pktstamp = (u32)(branch->packetstamp);
            vlib_cli_output (vm, "%d\t\t%d\t\t\tSrc:%U\tDst:%U\tSrcPort:%u\t\tDstPort:%u\tFlow-Pktstamp/Current-pktstamp:%d/%d\n",count,(ifindex),format_ip4_address,&(srcip), format_ip4_address,&(dstip),clib_net_to_host_u16(srcport),clib_net_to_host_u16(dstport),pktstamp,packets);
			}
            branch = current->branchnext->branchnext->branchnext;

            if((branch != NULL) && count < numflows){
            	count++;
            	if(branch->packetstamp > (-refresh)){
                
                ifindex = (branch->swsrcdstport)>>32 ;
        		srcip = (u32)((branch->srcdst)>>32);
        		dstip = (u32)((branch->srcdst)&0x00000000ffffffff) ;
        		srcport = (u16)(((branch->swsrcdstport)&0x00000000ffffffff)>>16) ;
        		dstport = (u16)((branch->swsrcdstport)&0x000000000000ffff) ;
        		pktstamp = (u32)(branch->packetstamp);
                vlib_cli_output (vm, "%d\t\t%d\t\t\tSrc:%U\tDst:%U\tSrcPort:%u\t\tDstPort:%u\tFlow-Pktstamp/Current-pktstamp:%d/%d\n",count,(ifindex),format_ip4_address,&(srcip), format_ip4_address,&(dstip),clib_net_to_host_u16(srcport),clib_net_to_host_u16(dstport),pktstamp,packets);
                }
                }
            }
        }
        current = current->prev;
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
