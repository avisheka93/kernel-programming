#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/slab.h>

#define MEM_SIZE 20

static void __iomem *virtual_base;
static dev_t first;
static struct cdev c_dev;
static struct class *cl;


static int myopen(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: open()\n");
	return 0;
}

static int myclose(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}

static ssize_t myread(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	int i;
	u8 byte;

	printk(KERN_INFO "Driver: read(): %u\n", len);
	
	if (*off >= MEM_SIZE)
		return 0;
	if (*off + len > MEM_SIZE)
		len = MEM_SIZE - *off;
	for (i = 0; i < len; i++)
	{
		byte = ioread8((u8 *)virtual_base + *off + i);
		if (copy_to_user(buf + i, &byte, 1))
			return -EFAULT;
	}
	*off += len;
	printk(KERN_INFO "%u\n", len);

	return len;
}

static ssize_t mywrite(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	int i;
	u8 byte;
	
	printk(KERN_INFO "Driver: write(): %u\n", len);

	if (*off >= MEM_SIZE)
		return 0;
	if (*off + len > MEM_SIZE)
		len = MEM_SIZE - *off;
	for (i = 0; i < len; i++)
	{
		if (copy_from_user(&byte, buf + i, 1))
			return -EFAULT;
		iowrite8(byte, (u8 *)virtual_base + *off + i);
	}
	*off += len;
	printk(KERN_INFO "%u\n", len);
	return len;
}


static struct file_operations ha_ops =
{
	.owner = THIS_MODULE,
	.open = myopen,
	.release = myclose,
	.read = myread,
	.write = mywrite
};


static int __init ha_init(void)
{
	printk(KERN_INFO "HA regd()\n");
	
	if ((virtual_base = (void *)kmalloc(MEM_SIZE, GFP_KERNEL)) == NULL)
	{
		printk(KERN_ERR "Kmalloc failed\n");
		return -1;
	}
	
	if (alloc_chrdev_region(&first, 0, 1, "haccessdev_t") < 0)
	{
		kfree(virtual_base);
		return -1;
	}
	
	if ((cl = class_create(THIS_MODULE, "haccessclass")) == NULL)
	{
		unregister_chrdev_region(first, 1);
		kfree(virtual_base);
		return -1;
	}
	
	if (device_create(cl, NULL, first, NULL, "haccessdevice") == NULL)
	{
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		kfree(virtual_base);
		return -1;
	}
	
	cdev_init(&c_dev, &ha_ops);
	
	if (cdev_add(&c_dev, first, 1) < 0)
	{
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		kfree(virtual_base);
		return -1;
	}
	
	return 0;
}

static void __exit ha_exit(void)
{
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);
	kfree(virtual_base);
	
	printk(KERN_INFO "HA unregd()\n");
}


module_init(ha_init);
module_exit(ha_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sumanjit SenGupta");
MODULE_DESCRIPTION("Hardware Interfacing Driver");
