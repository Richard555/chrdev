#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/cdev.h>

static int major;
#define DEVICE_NAME "mychrdev"
#define MAX_LENGTH 60
static char device_buf[MAX_LENGTH];

static int init_device(void);
static void exit_device(void);

module_init(init_device);
module_exit(exit_device);

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *,
size_t, loff_t *);
static ssize_t device_write(struct file *, const char *,
size_t, loff_t *);
static loff_t device_lseek(struct file *, loff_t, int);

static struct file_operations fops =
{
.owner = THIS_MODULE,
.read = device_read,
.write = device_write,
.open = device_open,
.release = device_release,
.llseek = device_lseek
};

static int init_device(void)
{
	int i;
	major = register_chrdev(0, DEVICE_NAME, &fops);

	if (major < 0) {
	printk(KERN_ALERT "Register Fail\n");
	return major;
	}

	printk(KERN_ALERT "Register OK: %d\n", major);
	for (i = 0; i < MAX_LENGTH; i++)
	device_buf[i] = 0;
	return 0;
}

static void exit_device(void)
{
	unregister_chrdev(major, DEVICE_NAME);
	printk(KERN_ALERT "Module Removed\n");
}

static int device_open(struct inode *inode, struct file *file)
{
	static int count = 0;
	count++;
	printk(KERN_ALERT "Number of times open() was called: %d\n", count);
	printk(KERN_ALERT "PID: %d\n", current->pid);
	return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
	printk(KERN_ALERT "close() was called\n");
	return 0;
}

static ssize_t device_read(struct file *file, char __user *buf,
size_t lbuf, loff_t *ppos)
{
	int maxbytes, bytes_to_read, nbytes;

	printk(KERN_ALERT "device_read: user mode addr=0x%p size=%ld offset=%lld\n", buf, lbuf, *ppos);

	maxbytes = MAX_LENGTH - *ppos;

	if (maxbytes > lbuf)
	bytes_to_read = lbuf;
	else
	bytes_to_read = maxbytes;

	if (bytes_to_read == 0)
	{
	printk(KERN_ALERT "Reached end of device\n");
	return -ENOSPC;
	}

	nbytes = bytes_to_read -
	copy_to_user(buf, device_buf, bytes_to_read);

	*ppos += nbytes;
	return nbytes;
}

static ssize_t device_write(struct file *file, const char *buf,
size_t lbuf, loff_t *ppos)
{
	int maxbytes, bytes_to_write, nbytes;
	
	printk(KERN_ALERT "device_write user mode addr=0x%p size=%ld offset=%lld\n", buf, lbuf, *ppos);

	maxbytes = MAX_LENGTH - *ppos;

	if (maxbytes > lbuf)
	bytes_to_write = lbuf;
	else
	bytes_to_write = maxbytes;

	if (bytes_to_write == 0)
	{
	printk(KERN_ALERT "Reached end of device\n");
	return -ENOSPC;
	}

	nbytes = bytes_to_write -
	copy_from_user(device_buf + *ppos, buf, bytes_to_write);

	*ppos += nbytes;
	return nbytes;
}

static loff_t device_lseek(struct file *file, loff_t offset,
int whence)
{
	loff_t new_pos = 0;

	printk(KERN_ALERT "device_lseek: file=0x%p offset=%lld whence=%d\n", file, offset, whence);

	switch(whence)
	{
	case SEEK_SET:
	new_pos = offset;
	break;
	case SEEK_CUR:
	new_pos = file->f_pos + offset;
	break;
	case SEEK_END:
	new_pos = MAX_LENGTH + offset;
	break;
	}

	if (new_pos > MAX_LENGTH)
		new_pos = MAX_LENGTH;
	if (new_pos < 0)
		new_pos = 0;

	file->f_pos = new_pos;
return new_pos;
}
