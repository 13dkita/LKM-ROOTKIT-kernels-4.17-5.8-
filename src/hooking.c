#include <linux/ftrace.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
#define KPROB 1
#include <linux/kprobes.h>
struct kprobe kp = {.symbol_name = "kallsyms_lookup_name"};
#endif

#define USEOFF 0
#if !USEOFF
#pragma GCC optimize("-fno-optimize-sibling-calls")
#endif

struct fhook {
	const char *name;
	void *function;
	void *original;
	unsigned long address;
	struct ftrace_ops ops;
};

//function helping with finding addres 
int res_addr(struct fhook *hook)
{
#ifdef KPROB
	typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
	kallsyms_lookup_name_t kallsyms_lookup_name = NULL;
//registering kprobe, getting kallsyms and unregistering kprobe	
	register_kprobe(&kp);
	kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
	unregister_kprobe(&kp);
#endif
	hook->address = kallsyms_lookup_name(hook->name);
	if (!hook->address)
		return -ENOENT;

#if USEOFF
	*((unsigned long*) hook->original) = hook->address + MCOUNT_INSN_SIZE;
#else
	*((unsigned long*) hook->original) = hook->address;
#endif
	return 0;
}

void notrace ft_ops(unsigned long ip, unsigned long parent_ip, struct ftrace_ops *ops, struct pt_regs *regs)
{
	struct fhook *hook = container_of(ops, struct fhook, ops);

#if USEOFF
	regs->ip = (unsigned long) hook->function;
#else
	if(!within_module(parent_ip, THIS_MODULE))
		regs->ip = (unsigned long) hook->function;
#endif
}

//function to adding hook
int hook_in(struct fhook *hook)
{
	int err = 0;
	
	err = res_addr(hook);
	if(err)
		return err;

//flags required to safe installing hooks
	hook->ops.func = ft_ops;
	hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS
			| FTRACE_OPS_FL_RECURSION_SAFE
			| FTRACE_OPS_FL_IPMODIFY;

	err = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0);
	if(err)
		return err;
//installing hook
	err = register_ftrace_function(&hook->ops);
	if(err)
		return err;
	
	return 0;
}

int hooks_in(struct fhook *hooks, size_t count)
{
	void hook_out(struct fhook *hook);
	int err = 0;
	size_t i = 0;

	for (i = 0 ; i < count ; i++)
	{
		err = hook_in(&hooks[i]);
		if(err)
			goto hookingfailed;
	}
	return 0;
	
hookingfailed:
	while (i != 0)
	{
		hook_out(&hooks[--i]);
	}
	return err;
}
//function to removing hook
void hook_out(struct fhook *hook)
{
	int err = 0;
	err = unregister_ftrace_function(&hook->ops);
	err = ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
}

void hooks_out(struct fhook *hooks, size_t count)
{
	size_t i = 0;
	for (i = 0 ; i < count ; i++)
		hook_out(&hooks[i]);
}
