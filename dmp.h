#include <linux/atomic.h>
#include <linux/device-mapper.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/sysfs.h>

static int dmp_ctr(struct dm_target* ti, unsigned int argc, char** argv);
static int dmp_map(struct dm_target* ti, struct bio* bio);
static void dmp_dtr(struct dm_target* ti);

static struct kset* dmp_kset;

struct dmp_info {
  atomic_t read_requests;
  atomic_t write_requests;
  atomic_t read_size;
  atomic_t write_size;
};

struct dmp_device {
  struct dm_dev* dev;
  struct dmp_info info;
  struct kobject kobj;
  char* name;
};

static ssize_t read_requests_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf) {
  struct dmp_device* dev = container_of(kobj, struct dmp_device, kobj);
  return sprintf(buf, "%d\n", atomic_read(&dev->info.read_requests));
}

static ssize_t write_requests_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf) {
  struct dmp_device* dev = container_of(kobj, struct dmp_device, kobj);
  return sprintf(buf, "%d\n", atomic_read(&dev->info.write_requests));
}

static ssize_t write_size_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf) {
  struct dmp_device* dev = container_of(kobj, struct dmp_device, kobj);
  return sprintf(buf, "%d\n", atomic_read(&dev->info.write_size));
}

static ssize_t read_size_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf) {
  struct dmp_device* dev = container_of(kobj, struct dmp_device, kobj);
  return sprintf(buf, "%d\n", atomic_read(&dev->info.read_size));
}

static struct kobj_attribute write_requests_attr = __ATTR_RO(write_requests);
static struct kobj_attribute read_requests_attr = __ATTR_RO(read_requests);
static struct kobj_attribute write_size_attr = __ATTR_RO(write_size);
static struct kobj_attribute read_size_attr = __ATTR_RO(read_size);

static struct attribute* dmp_attrs[] = {
    &write_requests_attr.attr,
    &read_requests_attr.attr,
    &write_size_attr.attr,
    &read_size_attr.attr,
    NULL,
};

static const struct attribute_group dmp_group = {
    .attrs = dmp_attrs,
};

static const struct attribute_group* dmp_groups[] = {
    &dmp_group,
    NULL,
};

static struct kobj_type dmp_ktype = {
    .sysfs_ops = &kobj_sysfs_ops,
    .default_groups = dmp_groups,
};

static struct target_type dmp_target = {
    .name = "dmp",
    .version = {1, 0, 0},
    .module = THIS_MODULE,
    .ctr = dmp_ctr,
    .dtr = dmp_dtr,
    .map = dmp_map,
};
