#ifndef _CHARDEVICES_H
#define _CHARDEVICES_H

ssize_t buffer_switching(struct file *file, char __user *buf, size_t nbytes,
                         loff_t *ppos);

//random_read() hook
inline ssize_t hook_random_read(struct file *file, char __user *buf,
                                size_t nbytes, loff_t *ppos) {
  return buffer_switching(file, buf, nbytes, ppos);
}

//urandom_read() hook
inline ssize_t hook_urandom_read(struct file *file, char __user *buf,
                                 size_t nbytes, loff_t *ppos) {
  return buffer_switching(file, buf, nbytes, ppos);
}

#endif
