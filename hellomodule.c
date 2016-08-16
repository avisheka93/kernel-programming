#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/version.h>

static int __init hello_init(void)
{
	printk(KERN_INFO "Hello World!");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodbye World!");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SumanjitSG");
MODULE_DESCRIPTION("HelloWorldProgram.");
