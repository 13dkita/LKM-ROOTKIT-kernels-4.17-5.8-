#ifndef _NETWORKING_H
#define _NETWORKING_H

int iterate(void);
int tcp_send(struct socket *sock, const char *buf, const size_t length, unsigned long flags);
int my_tcp_sock_init(void);
void send_message(const char *strFrom);

#endif
