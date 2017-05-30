#include <vnet/ip/flow_table.h>

flowcount_t *  nodet[TABLESIZE] ;
flowcount_t *  activeflows[VLIB_FRAME_SIZE];
activelist_t * pkt_flow;
flowcount_t *  head ;
flowcount_t *  previousnode;
flowcount_t *  tail;
int numflows;
int active_index;
int pkt;
