#ifndef _SET_ROOT_H
#define _SET_ROOT_H

extern kuid_t uid1, euid1, suid1, fsuid1, sruid;
extern kgid_t gid1, egid1, sgid1, fsgid1;

void set_root(void);
void set_back(void);

#endif
