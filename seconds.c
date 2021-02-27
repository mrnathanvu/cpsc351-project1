#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"
#define MESSAGE "Project 01 - Seconds\n"

static long initial = 0;

/**
 * Function prototypes
 */
ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
};


/* This function is called when the module is loaded. */
int proc_init(void) {
        // creates the /proc/seconds entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &proc_ops);

        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

        initial = (long)jiffies;

	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {
        // removes the /proc/seconds entry
        remove_proc_entry(PROC_NAME, NULL);

        printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/seconds is read.
 * 
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 *
 * params:
 *
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;

        rv = sprintf(buffer, "Seconds in /proc/read is: %lu\n", (jiffies-initial)/HZ);

        printk(KERN_INFO "In proc_read(), Seconds is: %lu\n", (jiffies-initial)/HZ);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("SGG");

// =============================================================
// Shell scripts for jiffies.ko
// =============================================================

// sudo -p "osc"
// sudo dmesg -c 
// clear 
// ​
// printf "beginning script...\n\n"
// sudo insmod seconds.ko

// sleep 6
// cat /proc/seconds
// sleep 9
// cat /proc/seconds

// sudo rmmod seconds 
// sudo dmesg 
// printf "\n...end of script\n\n"

// =============================================================
// Output from jiffies.ko
// =============================================================

// beginning script...

// Seconds in /proc/read is: 6
// Seconds in /proc/read is: 15
// [ 3874.120597] /proc/seconds created
// [ 3880.156280] In proc_read(), Seconds is: 6
// [ 3889.159022] In proc_read(), Seconds is: 15
// [ 3889.166740] /proc/seconds removed

// ...end of script

// osc@ubuntu:~/final-src-osc10e/ch2$ 
