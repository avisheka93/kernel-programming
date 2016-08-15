#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/errno.h>

#include "ioctlheader.h"

static dev_t first;
static struct class *cl;
static struct cdev c_dev;
static int status =1, dignity = 3, ego = 5;

static int my_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: open()\n");
	return 0;
}

static int my_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	query_arg_t q;
	
	switch(cmd)
	{
		
	
	}
	
	return 0;

}

static struct file_operations id_ops =
{
	.owner = THIS_MODULE,
	.open = myopen,
	.release = my_close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
	.ioctl = my_ioctl
#else
	.unlocked_ioctl = my_ioctl
#endif
};

static int __init ioctl_init(void)
{
	int ret;
	struct device *device_ret;
	
	printk{KERN_INFO "Driver: init()\n");
	
	if(IS_ERR(ret = alloc_chrdev_region(&first, 0, 1, "ioctldrdev_t")))
	{
		printk(KERN_ERR "Failed: alloc_chrdev_region()\n");
		return PTR_ERR(ret);
	}
	
	if(IS_ERR(cl = class_create(THIS_MODULE, "ioctldrclass")))
	{
		printk(KERN_ERR "Failed: class_create()\n");
		unregister_chrdev_region(first, 1);
		return PTR_ERR(cl);
	}
	
	if(IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "ioctldrdevice")
	{
		printk(KERN_ERR "Failed: device_create()\n");
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return PTR_ERR(dev_ret);
	}
	
	cdev_init(&c_dev, &id_ops);

	if(IS_ERR(ret = cdev_add(&c_dev, first, 1)))
	{
		printk(KERN_ERR "Failed: cdev_add()\n");
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return PTR_ERR(ret);
	}
	
	return 0;
}

static void __exit ioctl_exit(void)
{
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);
	
	printk(KERN_INFO "Driver: exit()\n")
}


module_init(ioctl_init);
module_exit(ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sumanjit SenGupta");
MODULE_DESCRIPTION("Query driver data structures ioctl driver");
