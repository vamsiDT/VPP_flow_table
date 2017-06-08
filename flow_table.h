/*
*   Flow classification in VPP
*
*         flow_table.h
*
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
#define ALPHA 0.9   // ALPHA = Output/Input
#define BETA 0.1    // BETA = Output/Input
#define BUFFER 1370000 // just assumed 250*1500 for testing purpose. Update the value with proper theoritical approach.
#define THRESHOLD 12800 //just a random number. Update the value with proper theoritical approach.

/*Node in the flow table. srcdst is 64 bit divided as |32bitsrcip|32bitdstip| ; swsrcdstport is divided as |32bit swifindex|16bit srcport|16bit dstport|*/
typedef struct flowcount{
    u64 srcdst;
    u64 swsrcdstport;
    int vqueue;
    struct flowcount * branchnext;
    struct flowcount * update;
}flowcount_t;

typedef struct activelist{
    struct flowcount * flow;
    struct activelist * next;
}activelist_t;

extern flowcount_t *  nodet[TABLESIZE];
extern activelist_t * head_af;
extern activelist_t * tail_af;
extern flowcount_t *  head ;
extern int numflows;
extern u32 r_qtotal;
extern u32 nbl;

/* Flow classification function */
always_inline flowcount_t *
flow_table_classify(u32 modulox, u64 hashx0, u64 hashx1, u16 pktlenx){

    flowcount_t * flow;

    if (PREDICT_FALSE(head == NULL)){
        numflows = 0;
        nbl = 0;
        nodet[modulox] = malloc(4*sizeof(flowcount_t));
        (nodet[modulox] + 0)->branchnext = NULL;
        (nodet[modulox] + 1)->branchnext = NULL;
        (nodet[modulox] + 2)->branchnext = NULL;
        (nodet[modulox] + 3)->branchnext = NULL;
        numflows++;
        (nodet[modulox] + 0)->srcdst = hashx0;
        (nodet[modulox] + 0)->swsrcdstport = hashx1;
        (nodet[modulox] + 0)->update = (nodet[modulox] + 0);
        head = nodet[modulox] + 0 ;
        flow = nodet[modulox] + 0;
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
        (nodet[modulox] + 0)->update = (nodet[modulox] + 0);
        flow = nodet[modulox] + 0;
    }

    else if  ((nodet[modulox] + 0)->branchnext == NULL)
    {
        if  ( ((nodet[modulox] + 0)->srcdst != hashx0) || ((nodet[modulox] + 0)->swsrcdstport != hashx1) ) 
        {
            numflows++;
            (nodet[modulox] + 1)->srcdst = hashx0;
            (nodet[modulox] + 1)->swsrcdstport = hashx1;
            (nodet[modulox] + 0)->branchnext = (nodet[modulox] + 1);
            flow = nodet[modulox] + 1;
        }
        else
        {
            flow = nodet[modulox] + 0;
        }
    }

    else if ( (nodet[modulox] + 1)->branchnext == NULL )
    {
        if ( ((nodet[modulox] + 0)->srcdst != hashx0) || ((nodet[modulox] + 0)->swsrcdstport != hashx1) ) {
            if ( ((nodet[modulox] + 1)->srcdst != hashx0) || ((nodet[modulox] + 1)->swsrcdstport != hashx1) ) {
                
                numflows++;
                (nodet[modulox] + 2)->srcdst = hashx0;
                (nodet[modulox] + 2)->swsrcdstport = hashx1;
                (nodet[modulox] + 1)->branchnext = nodet[modulox] + 2;
                flow = nodet[modulox] + 2;
            }
            else
            {
                flow = nodet[modulox] + 1;
            }
        }
        else
        {
            flow = nodet[modulox] + 0;
        }     
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
                    flow = nodet[modulox] + 3;
                }
                else
                {
                    flow = nodet[modulox] + 2;
                }
            }
            else
            {
                flow = nodet[modulox] + 1;
            }
        }
        else
        {
            flow = nodet[modulox] + 0;
        }
    }

    else
    {
        if ( ((nodet[modulox] + 0)->srcdst != hashx0) || ((nodet[modulox] + 0)->swsrcdstport != hashx1) ) {

            if ( ((nodet[modulox] + 1)->srcdst != hashx0) || ((nodet[modulox] + 1)->swsrcdstport != hashx1) ) {

                if ( ((nodet[modulox] + 2)->srcdst != hashx0) || ((nodet[modulox] + 2)->swsrcdstport != hashx1) ) {

                    if ( ((nodet[modulox] + 3)->srcdst != hashx0) || ((nodet[modulox] + 3)->swsrcdstport != hashx1) ) {

                        ((nodet[modulox] + 0)->update)->srcdst = hashx0;
                        ((nodet[modulox] + 0)->update)->swsrcdstport = hashx1;
                        flow = (nodet[modulox] + 0)->update;
                        (nodet[modulox] + 0)->update = ((nodet[modulox] + 0)->update)->branchnext ;
                    }
                    else
                    {
                        flow = nodet[modulox] + 3;
                    }
                }
                else
                {
                    flow = nodet[modulox] + 2;
                }
            }
            else
            {
                flow = nodet[modulox] + 1;
            }
        }
        else
        {
            flow = nodet[modulox] + 0;
        }
    }

    return flow;
}

/* function to insert the flow in blacklogged flows list. The flow is inserted at the end of the list i.e tail.*/
void flowin(flowcount_t * flow){
    activelist_t * temp;
    temp = malloc(sizeof(activelist_t));
    temp->flow = flow;
    temp->next = NULL;
    if (head_af == NULL){
        head_af = temp;
        tail_af = temp;
    }
    else{
        tail_af->next = temp;
        tail_af = temp;
    }
}

/* function to extract the flow from the blacklogged flows list. The flow is taken from the head of the list. */
flowcount_t * flowout(){
    flowcount_t * temp;
    temp = head_af->flow;
    head_af = head_af->next;
    return temp;
}


/* vstate algorithm */
always_inline void vstate(flowcount_t * flow, u16 pktlenx,u8 update){
    flowcount_t * j;
    u32 served,credit;
    int oldnbl=nbl+1;
    credit = BUFFER/*This is just a temporary variable. Determine the capacity to be shared*/;
    if(PREDICT_FALSE(update == 1)){
        while (oldnbl>nbl && nbl > 0){
            oldnbl = nbl;
            served = credit/nbl;
            credit = 0; 
            for (int k=0;k<oldnbl;k++){
                j = flowout(); 
                if(j->vqueue > served){
                    j->vqueue -= served;
                    flowin(j);
                }
                else{
                    credit += served - j->vqueue;
                    j->vqueue = 0;
                    nbl--;
                }
            }
        }
    }

    if (flow != NULL){
        if (flow->vqueue == 0){
            nbl++;
            flowin(flow);
        }
        flow->vqueue += pktlenx; 
    }
}

/* arrival function for each packet */
always_inline void arrival(flowcount_t * flow, u16 pktlenx,vlib_buffer_t *b0){

    if(flow->vqueue <= THRESHOLD /*&& r_qtotal < BUFFER*/){
        vstate(flow,pktlenx,0); 
        //r_qtotal += pktlenx;
    }
    else {
        vnet_buffer (b0)->sw_if_index[VLIB_TX] = vnet_buffer (b0)->sw_if_index[VLIB_RX];
        //update vstate is only after a vector. So no update before dropping a packet here.
        //code for dropping the packet.
    }
}


always_inline void fq (u32 modulox, u64 hashx0, u64 hashx1, u16 pktlenx,vlib_buffer_t *b0){
    flowcount_t * i;
    i = flow_table_classify(modulox,hashx0,hashx1,pktlenx);
    arrival(i,pktlenx,b0);
}

always_inline void departure (){
	vstate(NULL,0,1);
}
#endif /*FLOW_TABLE_H*/

/*
*   "Gather ye rosebuds while ye may"
*                  - Mike Portnoy
*
*   End
*
*/
