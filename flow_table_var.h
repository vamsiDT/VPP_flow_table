#include <vnet/ip/flow_table.h>

flowcount_t *  nodet[TABLESIZE] ;
flowcount_t *  head ;
flowcount_t *  previousnode;
flowcount_t *  tail;
int numflows;
int active_index;