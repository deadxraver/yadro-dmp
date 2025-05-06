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
  struct dmp_device* ddev = ti->private;

  if (bio_op(bio) == REQ_OP_READ) {
    atomic_inc(&ddev->info.read_requests);
    atomic_add(bio->bi_iter.bi_size, &ddev->info.read_size);
  } else if (bio_op(bio) == REQ_OP_WRITE) {
    atomic_inc(&ddev->info.write_requests);
    atomic_add(bio->bi_iter.bi_size, &ddev->info.write_size);
  }

  bio_set_dev(bio, ddev->dev->bdev);
  submit_bio_noacct(bio);
  return DM_MAPIO_SUBMITTED;
}

static void dmp_dtr(struct dm_target* ti) {
  struct dmp_device* ddev = ti->private;
  dm_put_device(ti, ddev->dev);
  kobject_put(&ddev->kobj);
  kfree(ddev);
}

void cleanup_module(void) {
  dm_unregister_target(&dmp_target);
  kset_unregister(dmp_kset);
  printk(KERN_INFO "dmp drop\n");
}

MODULE_LICENSE("GPL");
