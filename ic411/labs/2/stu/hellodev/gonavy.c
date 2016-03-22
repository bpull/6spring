#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>

#include <asm/uaccess.h>

//general information about the module
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aviv");
MODULE_DESCRIPTION("Module to add /dev/gonavy");

//function declearations
static int gonavy_init(void);
static void gonavy_cleanup(void);
static ssize_t gonavy_read(struct file * file, char * buf, size_t len, loff_t *ppos);
static int gonavy_open(struct inode *inodep, struct file *filep);

//setting up device structures
static struct file_operations gonavy_fops = {
  .open = gonavy_open,
  .read = gonavy_read,
};
static struct miscdevice gonavy_dev = { MISC_DYNAMIC_MINOR,"gonavy",&gonavy_fops};

//globals
static char gonavy_str[]="Go Navy!\n";
static int gonavy_len=9;

//specify  our init and cleanup functions
module_init(gonavy_init);
module_exit(gonavy_cleanup);


//Initialize the module
static int __init gonavy_init(void){
  int ret;

  printk(KERN_INFO "gonavy: Initiallizing /dev/gonavy\n");
  if( (ret = misc_register(&gonavy_dev) ))
    printk(KERN_ERR "gonavy: Unable to register \"/dev/gonavy\" device\n");

  return ret;
}

//De-initialize the module
static void __exit gonavy_cleanup(void){
  printk(KERN_INFO "gonavy: Deinitillizing /dev/gonavy\n");
  misc_deregister(&gonavy_dev);
}

//Open the /dev/gonavy
static int gonavy_open(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "gonavy: /dev/gonavy opened\n");
   return 0;
}



//called whenever /dev/gonavy is read from
static ssize_t gonavy_read(struct file * filep, char * buf, size_t len, loff_t *ppos){

  //filep : structure of the file
  //buf   : userspace address to write what was read to
  //len   : request number of bytes to read
  //ppos  : offset into the file

  if ( len < gonavy_len)
    return -EINVAL; //support reading the whole string
  
  if ( *ppos != 0 ) 
    return 0; //read the string already, indicate EOF

  //write string to user space, returns 0 on success
  if(copy_to_user(buf,gonavy_str,gonavy_len)) 
    return -EINVAL; //error condition

  *ppos = gonavy_len; //indicate we've read this far into the file
  return gonavy_len;
}

