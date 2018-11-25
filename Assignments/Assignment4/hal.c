#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dr Chandra");
MODULE_DESCRIPTION("Heuristically programmed ALgorithmic computer");

static int __init hal_init(void) {
  // printk(KERN_INFO "I honestly think you ought to calm down;\n");
  here:
    printk(KERN_INFO "I'm here %p;\n", &&here);
  return 0;
}

static void __exit hal_cleanup(void) {
  printk(KERN_INFO "What are you doing, Dave?\n");
}

module_init(hal_init);
module_exit(hal_cleanup);
