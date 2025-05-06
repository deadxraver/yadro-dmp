#include <linux/kernel.h>
#include <linux/module.h>

int init_module(void) {
  int* test;
  printk(KERN_INFO "Hello lol\n");
  return 0;
}

void cleanup_module(void) {
  pr_info("bye-bye\n");
}

MODULE_LICENSE("GPL");
