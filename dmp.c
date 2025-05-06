#include "dmp.h"

int init_module(void) {
  printk(KERN_INFO "dmp init\n");
  dmp_kset = kset_create_and_add("dmp", NULL, kernel_kobj);
  if (!dmp_kset)
    return -ENOMEM;

  return dm_register_target(&dmp_target);
}

static int dmp_ctr(struct dm_target* ti, unsigned int argc, char** argv) {
  struct dmp_device* ddev;
  int err_code = 0;

  ddev = kzalloc(sizeof(struct dmp_device), GFP_KERNEL);
  if (!ddev)
    return -ENOMEM;

  err_code = dm_get_device(ti, argv[0], dm_table_get_mode(ti->table), &ddev->dev);
  if (err_code)
    goto error;

  atomic_set(&ddev->info.read_requests, 0);
  atomic_set(&ddev->info.write_requests, 0);
  atomic_set(&ddev->info.write_size, 0);
  atomic_set(&ddev->info.read_size, 0);

  kobject_init(&ddev->kobj, &dmp_ktype);
  err_code = kobject_add(&ddev->kobj, &dmp_kset->kobj, "%s", ti->type->name);
  if (err_code)
    goto error;

  ti->private = ddev;
  return 0;

error:
  if (ddev->dev)
    dm_put_device(ti, ddev->dev);
  kfree(ddev);
  return err_code;
}

static int dmp_map(struct dm_target* ti, struct bio* bio) {
  // TODO:
  return 0;
}

void cleanup_module(void) {
  printk(KERN_INFO "dmp drop\n");
}

MODULE_LICENSE("GPL");
