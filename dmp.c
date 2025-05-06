#include <linux/atomic.h>
#include <linux/kernel.h>
#include <linux/module.h>

struct dmp_info {
  atomic_t read_requests;
  atomic_t write_requests;
  atomic_t read_size;
  atomic_t write_size
};

int init_module(void) {
  printk(KERN_INFO "dmp init\n");
  return 0;
}

void cleanup_module(void) {
  printk(KERN_INFO "dmp drop\n");
}

MODULE_LICENSE("GPL");
