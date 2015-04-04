#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>

#define DRIVER_AUTHOR	"Nick Levesque <nick.levesque@gmail.com>
#define DRIVER_DESC	"Sets red, green and blue values for external LED"
#define DEVICE_NAME	"rgbled"

struct rgb_dev {
	struct mutex lock;
}

// Implementation of file operation methods

int rgb_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "rgb: opened device\n");
	return 0;
}

// Allow threads to read
int rgb_read(struct file *filp, char *buf, size_t buf_cnt, loff_t* offset)
{
	printk(KERN_INFO "rgb, read from device\n");
	return 0;
}

// Allow threads to write colors to LED
ssize_t rgb_write(struct file *filp, const char *src_buf, size_t buf_cnt, loff_t* offset)
{
	// Wait for lock
	if (mutex_lock_interruptible(&stintdev.lock) {
		
