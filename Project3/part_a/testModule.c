/*
 * Hello World module
 * @author: Fuad Aghazada
 * @date: 21.11.2018
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

/* Initializing the module */
static int __init hello(void)
{
	printk(KERN_INFO "Hello, World!\n");

	return 0;		// Module exits successfully
}

/* Cleaning the module */
static void __exit goodbye(void)
{
	printk(KERN_INFO "Module is destroyed!\n");
}

module_init(hello);
module_exit(goodbye);
