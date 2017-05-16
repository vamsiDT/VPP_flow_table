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
#ifndef FLOW_TABLE_H
#define FLOW_TABLE_H
#define TABLESIZE 1024

/*Node in the flow table. srcdst is 64 bit divided as |32bitsrcip|32bitdstip| ; swsrcdstport is divided as |32bit swifindex|16bit srcport|16bit dstport|*/
typedef struct flowcount{
	struct flowcount * prev;
	u64 srcdst;
	u64 swsrcdstport;
	int npackets;
	int vqueue;
	struct flowcount * branchnext;
    struct flowcount * next;
    struct flowcount * update;
}flowcount_t;

extern flowcount_t *  nodet[TABLESIZE];
extern flowcount_t *  head ;
extern flowcount_t *  previousnode;
extern flowcount_t *  tail;
extern int numflows;


/* Flow classification function */
always_inline void
flow_table_classify(u32 modulox, u64 hashx0, u64 hashx1, u16 pktlenx ){

if (PREDICT_FALSE(head == NULL)){
	numflows = 0;
    nodet[modulox] = malloc(4*sizeof(flowcount_t));
    (nodet[modulox] + 0)->branchnext = NULL;
    (nodet[modulox] + 1)->branchnext = NULL;
    (nodet[modulox] + 2)->branchnext = NULL;
    (nodet[modulox] + 3)->branchnext = NULL;
    numflows++;
    (nodet[modulox] + 0)->srcdst = hashx0;
    (nodet[modulox] + 0)->swsrcdstport = hashx1;
    (nodet[modulox] + 0)->update = (nodet[modulox] + 0);
    (nodet[modulox] + 0)->npackets=(nodet[modulox] + 0)->npackets + pktlenx;
    head = nodet[modulox] + 0 ;
    previousnode = head ;
	tail = head;
    }
    else if ( (nodet[modulox] + 0) == NULL ){
    nodet[modulox] = malloc(4*sizeof(flowcount_t));
    (nodet[modulox] + 0)->branchnext = NULL;
    (nodet[modulox] + 1)->branchnext = NULL;
    (nodet[modulox] + 2)->branchnext = NULL;
    (nodet[modulox] + 3)->branchnext = NULL;
    numflows++;
    (nodet[modulox] + 0)->srcdst = hashx0;
    (nodet[modulox] + 0)->swsrcdstport = hashx1;
    (nodet[modulox] + 0)->prev = previousnode;
    previousnode->next = nodet[modulox] + 0;
    previousnode = nodet[modulox] + 0;
    tail = nodet[modulox] + 0 ;
    (nodet[modulox] + 0)->update = (nodet[modulox] + 0);
    (nodet[modulox] + 0)->npackets=(nodet[modulox] + 0)->npackets + pktlenx;
    }
    else if  ((nodet[modulox] + 0)->branchnext == NULL)
    {
        if  ( ((nodet[modulox] + 0)->srcdst != hashx0) || ((nodet[modulox] + 0)->swsrcdstport != hashx1) ) 
        {
    numflows++;
    (nodet[modulox] + 1)->srcdst = hashx0;
    (nodet[modulox] + 1)->swsrcdstport = hashx1;
    (nodet[modulox] + 0)->branchnext = (nodet[modulox] + 1);
    (nodet[modulox] + 1)->npackets=(nodet[modulox] + 1)->npackets + pktlenx;
        }
        else
        (nodet[modulox] + 0)->npackets=(nodet[modulox] + 0)->npackets + pktlenx;
    }
    else if ( (nodet[modulox] + 1)->branchnext == NULL )
    {
        if ( ((nodet[modulox] + 0)->srcdst != hashx0) || ((nodet[modulox] + 0)->swsrcdstport != hashx1) ) {
    		if ( ((nodet[modulox] + 1)->srcdst != hashx0) || ((nodet[modulox] + 1)->swsrcdstport != hashx1) ) {
    			
    			numflows++;
    			(nodet[modulox] + 2)->srcdst = hashx0;
    			(nodet[modulox] + 2)->swsrcdstport = hashx1;
    			(nodet[modulox] + 1)->branchnext = nodet[modulox] + 2;
    			(nodet[modulox] + 2)->npackets=(nodet[modulox] + 2)->npackets + pktlenx;
    		}
    		else
    		(nodet[modulox] + 1)->npackets=(nodet[modulox] + 1)->npackets + pktlenx;
        }
        else
        (nodet[modulox] + 0)->npackets=(nodet[modulox] + 0)->npackets + pktlenx;
     }
    else if ( (nodet[modulox] + 2)->branchnext == NULL ){
        if ( ((nodet[modulox] + 0)->srcdst != hashx0) || ((nodet[modulox] + 0)->swsrcdstport != hashx1) ) {
    		if ( ((nodet[modulox] + 1)->srcdst != hashx0) || ((nodet[modulox] + 1)->swsrcdstport != hashx1) ) {
    			if ( ((nodet[modulox] + 2)->srcdst != hashx0) || ((nodet[modulox] + 2)->swsrcdstport != hashx1) ) {
    				
    				numflows++;
    				(nodet[modulox] + 3)->srcdst = hashx0;
    				(nodet[modulox] + 3)->swsrcdstport = hashx1;
    				(nodet[modulox] + 2)->branchnext = nodet[modulox] + 3;
    				(nodet[modulox] + 3)->branchnext = nodet[modulox] + 0;
    				(nodet[modulox] + 3)->npackets++;
    			}
    			else
    			(nodet[modulox] + 2)->npackets=(nodet[modulox] + 2)->npackets + pktlenx;
        	}
        	else
    		(nodet[modulox] + 1)->npackets=(nodet[modulox] + 1)->npackets + pktlenx;
		}
		else
		(nodet[modulox] + 0)->npackets=(nodet[modulox] + 0)->npackets + pktlenx;
	}


    else
    {
    	if ( ((nodet[modulox] + 0)->srcdst != hashx0) || ((nodet[modulox] + 0)->swsrcdstport != hashx1) ) {

    		if ( ((nodet[modulox] + 1)->srcdst != hashx0) || ((nodet[modulox] + 1)->swsrcdstport != hashx1) ) {

    			if ( ((nodet[modulox] + 2)->srcdst != hashx0) || ((nodet[modulox] + 2)->swsrcdstport != hashx1) ) {

    				if ( ((nodet[modulox] + 3)->srcdst != hashx0) || ((nodet[modulox] + 3)->swsrcdstport != hashx1) ) {

    					((nodet[modulox] + 0)->update)->srcdst = hashx0;
    					((nodet[modulox] + 0)->update)->swsrcdstport = hashx1;
    					((nodet[modulox] + 0)->update)->npackets=((nodet[modulox] + 0)->update)->npackets + pktlenx;
    					(nodet[modulox] + 0)->update = ((nodet[modulox] + 0)->update)->branchnext ;
    				}
    				else
    				(nodet[modulox] + 3)->npackets=(nodet[modulox] + 3)->npackets + pktlenx;
    			}
    			else
    			(nodet[modulox] + 2)->npackets=(nodet[modulox] + 2)->npackets + pktlenx;
    		}
    		else
    		(nodet[modulox] + 1)->npackets=(nodet[modulox] + 1)->npackets + pktlenx;
    	}
    	else
    	(nodet[modulox] + 0)->npackets=(nodet[modulox] + 0)->npackets + pktlenx;
    }


}

#endif /*FLOW_TABLE_H*/

/*
*	"Gather ye rosebuds while ye may"
*				- Mike Portnoy
*
*	End
*
*/
