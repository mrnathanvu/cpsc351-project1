#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "jiffies"
#define MESSAGE "Project 01 - jiffies\n"

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
        // creates the /proc/jiffies entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &proc_ops);

        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

        printk(KERN_INFO "In proc_init(), GOLDEN_RATIO_PRIME is: %lu\n", GOLDEN_RATIO_PRIME);
        printk(KERN_INFO "In proc_init(), Jiffies is: %lu and HZ is: %d\n\n", jiffies, HZ);

	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {
        // removes the /proc/jiffies entry
        remove_proc_entry(PROC_NAME, NULL);

        printk(KERN_INFO "In proc_exit(), the greatest common divisor (gcd) of 3,300 and 24 is: %lu\n", gcd(3300, 24));
        printk(KERN_INFO "In proc_exit(), Jiffies is: %lu\n", jiffies);

        printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/jiffies is read.
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

        rv = sprintf(buffer, "Jiffies in /proc/read is: %lu\n", jiffies);

        printk(KERN_INFO "In proc_read(), Jiffies is: %lu\n", jiffies);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Jiffies Module");
MODULE_AUTHOR("SGG");

// =============================================================
// Shell scripts for jiffies.ko
// =============================================================

// sudo -p "osc"
// sudo dmesg -c 
// clear 
// ​
// printf "beginning script...\n\n"
// sudo insmod jiffies.ko

// cat /proc/jiffies

// sudo rmmod jiffies 
// sudo dmesg 
// printf "\n...end of script\n\n"

// =============================================================
// Output from jiffies.ko
// =============================================================

// beginning script...

// Jiffies in /proc/read is: 4295726068
// [ 3335.429660] /proc/jiffies created
// [ 3335.429663] In proc_init(), GOLDEN_RATIO_PRIME is: 11400862456688148481
// [ 3335.429664] In proc_init(), Jiffies is: 4295726068 and HZ is: 250

// [ 3335.432318] In proc_read(), Jiffies is: 4295726068
// [ 3335.439074] In proc_exit(), the greatest common divisor (gcd) of 3,300 and 24 is: 12
// [ 3335.439078] In proc_exit(), Jiffies is: 4295726070
// [ 3335.439079] /proc/jiffies removed

// ...end of script

// osc@ubuntu:~/final-src-osc10e/ch2$ 
