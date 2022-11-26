#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/cdev.h>

#define DEVICE_NAME "driver"
#define CALL_PRINT 1

static dev_t device_dev;
static struct class *device_class;
static struct cdev device_cdev;

static char *buffer = NULL;

int device_open(struct inode *inode, struct file *filp);
int device_release(struct inode *inode, struct file *filp);
ssize_t device_read(struct file *filp, char *buf, size_t count, loff_t *f_ops);
ssize_t device_write(struct file *filp, const char *buf, size_t count, loff_t *f_ops);
long device_ioctl(struct file *filp, unsigned int cmd, unsigned long data);

static struct file_operations fops = {
	.open = device_open,
	.release = device_release,
	.read = device_read,
	.write = device_write,
	.unlocked_ioctl = device_ioctl
};

int device_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "device open\n");
	return 0;
}

int device_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "device close\n");
	return 0;
}

ssize_t device_read(struct file *filp, char *buf, size_t count, loff_t *f_ops)
{
	int msg;

	if(buffer == NULL)
		return -1;

	msg = copy_to_user(buf, buffer, count);
	printk("[%s] count = %ld, msg = %u\n", __func__, count, msg);
	return count - msg;
}

ssize_t device_write(struct file *filp, const char *buf, size_t count, loff_t *f_ops)
{
	int msg;

	if(buffer != NULL)
		kfree(buffer);

	if((buffer = kmalloc(count + 1, GFP_KERNEL)) == NULL)
		return -ENOMEM;

	msg = copy_from_user(buffer, buf, count);
	printk("[%s] count = %ld, msg = %u\n", __func__, count, msg);

	return count - msg;
}

long device_ioctl(struct file *filp, unsigned int cmd, unsigned long data)
{
	switch (cmd) {
		case CALL_PRINT:
			printk(KERN_INFO "[%s] CALL PRINT!", __func__);
			break;
		default:
			break;
	}

        return 0;
}

int __init device_init(void) 
{
	if(alloc_chrdev_region(&device_dev, 0, 1, DEVICE_NAME))
	{
		printk(KERN_ALERT "[&s] alloc_chrdev_region failed\n", __func__);	
		return -1;
	}
	
	cdev_init(&device_cdev, &fops);

	if(cdev_add(&device_cdev, device_dev, 1))
	{
		printk(KERN_ALERT "[%s] cdev_add failed\n", __func__);
		unregister_chrdev_region(device_dev, 1);
	}

	if((device_class = class_create(THIS_MODULE, DEVICE_NAME)) == NULL) 
	{
		printk(KERN_ALERT "[%s] class_add failed\n", __func__);
		unregister_chrdev_region(device_dev, 1);
	}

	if(device_create(device_class, NULL, device_dev, NULL, DEVICE_NAME) == NULL)
	{
		printk(KERN_ALERT "[%s] device_create failed\n", __func__);
		class_destroy(device_class);
	}

	printk(KERN_INFO "[%s] successfully created device: Major = %d, Minor = %d\n",
			__func__, MAJOR(device_dev), MINOR(device_dev));
	return 0;
	
}

void __exit device_exit(void)
{
	device_destroy(device_class, device_dev);	
	class_destroy(device_class);
	cdev_del(&device_cdev);
	unregister_chrdev_region(device_dev, 1);
	printk("KERN_INFO [%s] successfully unregistered.\n", __func__);
}

module_init(device_init);
module_exit(device_exit);

MODULE_AUTHOR("Test");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("character device driver");
