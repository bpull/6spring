#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>

#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aviv");
MODULE_DESCRIPTION("Module to add /dev/reverse");

//function declearations
static int reverse_init(void);
static void reverse_cleanup(void);
static ssize_t reverse_read(struct file * file, char * buf, size_t len, loff_t *ppos);
static ssize_t reverse_write(struct file *filep, const char *buf, size_t len, loff_t *offset);
static int reverse_open(struct inode *inodep, struct file *filep);

//setting up device structure
static struct file_operations reverse_fops = {
  .open = reverse_open,
  .read = reverse_read,
  .write = reverse_write,
};
static struct miscdevice reverse_dev = { MISC_DYNAMIC_MINOR,"reverse",&reverse_fops};

//globals
static char revbuffer[4096] = {0};
static short revbuffer_size = 0;

//specify  our init and cleanup functions
module_init(reverse_init);
module_exit(reverse_cleanup);


//Initialize the module
static int __init reverse_init(void)
{
  int ret;

  printk(KERN_INFO "reverse: Initiallizing /dev/reverse\n");
  if( (ret = misc_register(&reverse_dev) ))
    printk(KERN_ERR "reverse: Unable to register \"/dev/reverse\" device\n");

  return ret;

}

//De-initialize the module
static void __exit reverse_cleanup(void)
{
  printk(KERN_INFO "reverse: Deinitillizing /dev/reverse\n");
  misc_deregister(&reverse_dev);
}


//Open the /dev/reverse
static int reverse_open(struct inode *inodep, struct file *filep){
  printk(KERN_INFO "reverse: /dev/reverse opened\n");
  return 0;
}

//called whenever /dev/reverse is read from
static ssize_t reverse_read(struct file * filep, char * buf, size_t len, loff_t *ppos){

  //filep : structure of the file
  //buf   : userspace address to write what was read to
  //len   : request number of bytes to read
  //ppos  : offset into the file



  //leave debug print in
  printk(KERN_INFO "reverse: read: revbuffer=\"%s\" revbuffer_size=%d\n",revbuffer, revbuffer_size);


  //TODO: Complete the write() function
  //  (1) Only allow read if there is something to read
  //  (2) Indicate EOF (return 0) if asking for offset not 0 (can only read all of once)
  //  (3) Return EINVAL if len not long enough for buffer
  //  (4) Return number of bytes read



  return 0;
     

}

//called whenever /dev/reverse is written to
static ssize_t reverse_write(struct file *filep, const char *buf, size_t len, loff_t *ppos){

  //filep : structure of the file
  //buf   : userspace data to be written
  //len   : request number of bytes to write
  //ppos  : offset into the file

  //TODO: Complete the write() function
  //  (1) Only one write before the next read
  //  (2) Indicate EOF (return 0) if asking for offset not 0
  //  (3) Only allow for at most 4096 bytes to written
  //  (4) Return number of bytes written


  //leave debug print in
  printk(KERN_INFO "reverse: write: wrote %d bytes to revbuffer\n", revbuffer_size);

  return 0;
}
