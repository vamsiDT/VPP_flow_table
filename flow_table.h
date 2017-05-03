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


/* Flow classification function */
always_inline void
flow_table_classify(u32 modulox, u64 hashx0, u64 hashx1){
if (PREDICT_FALSE(head == NULL)){
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

    if (PREDICT_FALSE(packets > refresh)){

        packets = 0;
        flowcount_t * tmpnode;
        flowcount_t * branch;
        tmpnode = tail;
        u32 count = 0;
        while (PREDICT_TRUE(tmpnode != NULL && count < numflows)){
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

#endif /*FLOW_TABLE_H*/

/*
*	"Gather ye rosebuds while ye may"
*				- Mike Portnoy
*
*	End
*
*/
