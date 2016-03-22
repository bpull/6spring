
#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aviv");
MODULE_DESCRIPTION("A Simple Hello World module");


/* Initialize the LKM */
static int __init hello_init(void)
{
  printk("Hello, world - this is the kernel speaking\n");
  return 0;
}


/* Cleanup - undo whatever init_module did */
static void __exit hello_cleanup(void)
{
  printk(KERN_INFO "Cleaning up module.\n");
}


module_init(hello_init);
module_exit(hello_cleanup);
