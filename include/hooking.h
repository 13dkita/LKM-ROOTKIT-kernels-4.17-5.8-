#ifndef _HOOKING_H
#define _HOOKING_H
#define HOOK(_name, _hook, _orig)                                              \
  { .name = (_name), .function = (_hook), .original = (_orig), }
struct fhook {
  const char *name;
  void *function;
  void *original;
  unsigned long address;
  struct ftrace_ops ops;
};

int res_addr(struct fhook *hook);
void notrace ft_ops(unsigned long ip, unsigned long parent_ip,
                             struct ftrace_ops *ops, struct pt_regs *regs);
int hook_in(struct fhook *hook);
void hook_out(struct fhook *hook);
int hooks_in(struct fhook *hooks, size_t count);
void hooks_out(struct fhook *hooks, size_t count);
#endif
