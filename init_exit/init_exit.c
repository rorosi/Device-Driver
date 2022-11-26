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

#define MINOR_BASE 0 /* starting number of minor number */
#define DEVICE_NAME "device" // 장치 고유 이름

static dev_t device_dev;
static struct class *device_class;
static struct cdev device_cdev;


/* overriding functions of Virtual File System,
 * used C99 feature
 */
static struct file_operations fops = {

};

int	__init device_init(void) {
	/* try allocating character device */
	if (alloc_chrdev_region(&device_dev, MINOR_BASE, 1, DEVICE_NAME)) {
		printk(KERN_ALERT "[%s] alloc_chrdev_region failed\n", __func__);
		goto err_return;
	}

	/* init cdev */
	cdev_init(&device_cdev, &fops);

	/* add cdev */
	if (cdev_add(&device_cdev, device_dev, 1)) {
		printk(KERN_ALERT "[%s] cdev_add failed\n", __func__);
		goto unreg_device;
	}

	if ((device_class = class_create(THIS_MODULE, DEVICE_NAME)) == NULL) {
		printk(KERN_ALERT "[%s] class_add failed\n", __func__);
		goto unreg_device;
	}

	if (device_create(device_class, NULL, device_dev, NULL, DEVICE_NAME) == NULL) {
		goto unreg_class;
	}

	printk(KERN_INFO "[%s] successfully created device: Major = %d, Minor = %d\n",
			__func__, MAJOR(device_dev), MINOR(device_dev));
	return 0;

unreg_class:
	class_destroy(device_class);

unreg_device:
	unregister_chrdev_region(device_dev, 1);

err_return:
	return -1;
}

void __exit	device_exit(void) {
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
