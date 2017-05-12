#include <vnet/ip/flow_table.h>

flowcount_t *  nodet[TABLESIZE][4] ;
flowcount_t *  head ;
flowcount_t *  previousnode;
flowcount_t *  tail;
int numflows;
