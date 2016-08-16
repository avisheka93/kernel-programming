#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

static dev_t first;
static struct class *cl;
static struct cdev c_dev;
static char c;

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

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Driver: read(), loff_t = %lu\n", *off);
	if(copy_to_user(buf, &c, 1) != 0)
		return -EFAULT;
	else if (*off == 0)
	{
		(*off)++;
//		copy_to_user(buf, "\n", 1);
		return 1;
	}
	else
		return 0;
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Driver: write()\n");
	if(copy_from_user(&c, buf + len - 1, 1) != 0)
		return -EFAULT;
	else
		return len;
}

static struct file_operations ssg_fops =
{
        .owner = THIS_MODULE,
        .open = my_open,
        .release = my_close,
        .read = my_read,
        .write = my_write
};

static int __init mfcd_init(void)
{
	printk(KERN_INFO "Mfcd regd!");
        if(alloc_chrdev_region(&first, 0, 1, "mycdev") < 0)
        {
                return -1;
        }
	if((cl = class_create(THIS_MODULE, "chardrvcls")) == NULL)
	{
		unregister_chrdev_region(first, 1);
		return -1;
	}
	if(device_create(cl, NULL, first, NULL, "nulldr") == NULL)
	{
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}
	cdev_init(&c_dev, &ssg_fops);
	if(cdev_add(&c_dev, first, 1) < 0)
	{
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}
	return 0;
}

static void __exit mfcd_exit(void)
{
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first,1);
	printk(KERN_INFO "Mfcd unregd!");
}

module_init(mfcd_init);
module_exit(mfcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SumanjitSG");
MODULE_DESCRIPTION("My First Character Driver");
