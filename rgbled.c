#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>

#define DRIVER_AUTHOR	"Nick Levesque <nick.levesque@gmail.com>
#define DRIVER_DESC	"Sets red, green and blue values for external LED"
#define DEVICE_NAME	"rgbled"

struct rgbled_dev {
	int ret;
	dev_t dev_num;
	struct cdev *cdev;
	int major_num;
} rgbleddev = {
	.major_num = 0;
	.ret = 0;
	.dev_num = 0;
};

// Implementation of file operation methods

int rgbled_open(struct inode *inode, struct file *filp)
{
	#ifdef DEBUG
	printk(KERN_INFO "rgbled: opened device\n");
	#endif
	return 0;
}

// Allow threads to read
int rgbled_read(struct file *filp, char *buf, size_t buf_cnt, loff_t* offset)
{
	#ifdef DEBUG
	printk(KERN_INFO "rgbled read from device\n");
	#endif
	return 0;
}

// Allow threads to write colors to LED
ssize_t rgbled_write(struct file *filp, const char *src_buf, size_t buf_cnt, loff_t* offset)
{
	#ifdef DEBUG
	printk(KERN_INFO "rgbled: write to device\n");
	#endif
	return 0;
}

struct file_operations fops = {
	.owner =	THIS_MODULE,
	.open = 	rbgled_open,
	.read = 	rgbled_read,
	.write =	rgbled_write,
	.release =	rgbled_close,
	.ioctl =	rgbled_ioctl
};

static int __init rgbled_init(void)
{
	rgbled.ret = alloc_chrdev_region(&rgbleddev.dev_num, 0, 1, DEVICE_NAME);
	if (rgbleddev.ret < 0) {
		prink(KERN_ALERT "rgbled: allocating major number failed\n");
		return rgbleddev.ret;
	}
	rgbleddev.major_num = MAJOTR(rgbleddev.dev_num);
	#ifdef DEBUG
	printk(KERN_INFO "rgbled: major num = %d\n", rgbleddev.major_num);
	#endif

	rgbleddev.cdev = cdev_alloc();
	rgbleddev.cdev->ops = &fops;
	rgbleddev.cdev->owner = THIS_MODULE;
	rgbleddev.ret = cdev_add(rgbleddev.cdev, rgbleddev.dev_num, 1);
	if (rgbleddev.ret < 0) {
		printk(KERN_ALERT "rgbled: failed to add cdev\n");
		return rgbleddev.ret;
	}
	// lock init
	rgbleddev.dev_num = MKDEV(rgbleddev.major_num, 0);
	return 0;
}
