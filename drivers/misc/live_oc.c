/* drivers/misc/live_oc.c
 *
 * Copyright 2011  Ezekeel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/device.h>
#include <linux/miscdevice.h>

#define LIVEOC_VERSION 1

#define MAX_OCVALUE 150

extern void liveoc_update(unsigned int oc_value, unsigned int oc_low_freq, unsigned int oc_high_freq);

static int oc_value = 100;

/* Apply Live OC to 800MHz and above*/
static int oc_low_freq = 800000;
/* Apply Live OC to 2000MHz and below */
static int oc_high_freq = 2000000;

static ssize_t liveoc_ocvalue_read(struct device * dev, struct device_attribute * attr, char * buf)
{
    return sprintf(buf, "%u\n", oc_value);
}

static ssize_t liveoc_octarget_low_read(struct device * dev, struct device_attribute * attr, char * buf)
{
    return sprintf(buf, "%u\n", oc_low_freq);
}

static ssize_t liveoc_octarget_high_read(struct device * dev, struct device_attribute * attr, char * buf)
{
    return sprintf(buf, "%u\n", oc_high_freq);
}

static ssize_t liveoc_ocvalue_write(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
    unsigned int data;

    if(sscanf(buf, "%u\n", &data) == 1) 
	{
	    if (data >= 100 && data <= MAX_OCVALUE)
		{
		    if (data != oc_value)
			{
			    oc_value = data;
		    
			    liveoc_update(oc_value, oc_low_freq, oc_high_freq);
			}

		    pr_info("LIVEOC oc-value set to %u\n", oc_value);
		}
	    else
		{
		    pr_info("%s: invalid input range %u\n", __FUNCTION__, data);
		}
	} 
    else 
	{
	    pr_info("%s: invalid input\n", __FUNCTION__);
	}

    return size;
}

static ssize_t liveoc_octarget_low_write(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
    unsigned int data;

    if(sscanf(buf, "%u\n", &data) == 1)
	{
	    if (data != oc_low_freq)
		{
		    oc_low_freq = data;
	    
		    liveoc_update(oc_value, oc_low_freq, oc_high_freq);

		    pr_info("LIVEOC oc-target-low set to %u\n", oc_low_freq);
		}
	    else
		{
		    pr_info("%s: invalid input range %u\n", __FUNCTION__, oc_low_freq);
		}
	}
    else
	{
		pr_info("%s: invalid input\n", __FUNCTION__);
	}
    return size;
}

static ssize_t liveoc_octarget_high_write(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
    unsigned int data;

    if(sscanf(buf, "%u\n", &data) == 1)
	{
	    if (data != oc_high_freq)
		{
		    oc_high_freq = data;
	    
		    liveoc_update(oc_value, oc_low_freq, oc_high_freq);

		    pr_info("LIVEOC oc-target-high set to %u\n", oc_high_freq);
		}
	    else
		{
		    pr_info("%s: invalid input range %u\n", __FUNCTION__, oc_high_freq);
		}
	}
    else
	{
		pr_info("%s: invalid input\n", __FUNCTION__);
	}
    return size;
}

static ssize_t liveoc_version(struct device * dev, struct device_attribute * attr, char * buf)
{
    return sprintf(buf, "%u\n", LIVEOC_VERSION);
}

static DEVICE_ATTR(oc_value, S_IRUGO | S_IWUGO, liveoc_ocvalue_read, liveoc_ocvalue_write);
static DEVICE_ATTR(oc_target_low, S_IRUGO | S_IWUGO, liveoc_octarget_low_read, liveoc_octarget_low_write);
static DEVICE_ATTR(oc_target_high, S_IRUGO | S_IWUGO, liveoc_octarget_high_read, liveoc_octarget_high_write);
static DEVICE_ATTR(version, S_IRUGO , liveoc_version, NULL);

static struct attribute *liveoc_attributes[] = 
    {
	&dev_attr_oc_value.attr,
	&dev_attr_oc_target_low.attr,
	&dev_attr_oc_target_high.attr,
	&dev_attr_version.attr,
	NULL
    };

static struct attribute_group liveoc_group = 
    {
	.attrs  = liveoc_attributes,
    };

static struct miscdevice liveoc_device = 
    {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "liveoc",
    };

static int __init liveoc_init(void)
{
    int ret;

    pr_info("%s misc_register(%s)\n", __FUNCTION__, liveoc_device.name);

    ret = misc_register(&liveoc_device);

    if (ret) 
	{
	    pr_err("%s misc_register(%s) fail\n", __FUNCTION__, liveoc_device.name);

	    return 1;
	}

    if (sysfs_create_group(&liveoc_device.this_device->kobj, &liveoc_group) < 0) 
	{
	    pr_err("%s sysfs_create_group fail\n", __FUNCTION__);
	    pr_err("Failed to create sysfs group for device (%s)!\n", liveoc_device.name);
	}

    return 0;
}

device_initcall(liveoc_init);
