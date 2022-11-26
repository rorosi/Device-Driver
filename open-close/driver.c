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

static dev_t device_dev;
static struct class *device_class;
static struct cdev device_cdev;

int device_open(struct inode *inode, struct file *filp);
int device_release(struct inode *inode, struct file *filp);

static struct file_operations fops = {
	.open = device_open,
	.release = device_release
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
