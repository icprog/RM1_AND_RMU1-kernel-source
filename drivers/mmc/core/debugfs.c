/*
 * Debugfs support for hosts and cards
 *
 * Copyright (C) 2008 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/moduleparam.h>
#include <linux/export.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/fault-inject.h>

#include <linux/mmc/card.h>
#include <linux/mmc/host.h>

#include "core.h"
#include "mmc_ops.h"

#ifdef CONFIG_FAIL_MMC_REQUEST

static DECLARE_FAULT_ATTR(fail_default_attr);
static char *fail_request;
module_param(fail_request, charp, 0);

#endif /* CONFIG_FAIL_MMC_REQUEST */

/* The debugfs functions are optimized away when CONFIG_DEBUG_FS isn't set. */
static int mmc_ios_show(struct seq_file *s, void *data)
{
	static const char *vdd_str[] = {
		[8]	= "2.0",
		[9]	= "2.1",
		[10]	= "2.2",
		[11]	= "2.3",
		[12]	= "2.4",
		[13]	= "2.5",
		[14]	= "2.6",
		[15]	= "2.7",
		[16]	= "2.8",
		[17]	= "2.9",
		[18]	= "3.0",
		[19]	= "3.1",
		[20]	= "3.2",
		[21]	= "3.3",
		[22]	= "3.4",
		[23]	= "3.5",
		[24]	= "3.6",
	};
	struct mmc_host	*host = s->private;
	struct mmc_ios	*ios = &host->ios;
	const char *str;

	seq_printf(s, "clock:\t\t%u Hz\n", ios->clock);
	if (host->actual_clock)
		seq_printf(s, "actual clock:\t%u Hz\n", host->actual_clock);
	seq_printf(s, "vdd:\t\t%u ", ios->vdd);
	if ((1 << ios->vdd) & MMC_VDD_165_195)
		seq_printf(s, "(1.65 - 1.95 V)\n");
	else if (ios->vdd < (ARRAY_SIZE(vdd_str) - 1)
			&& vdd_str[ios->vdd] && vdd_str[ios->vdd + 1])
		seq_printf(s, "(%s ~ %s V)\n", vdd_str[ios->vdd],
				vdd_str[ios->vdd + 1]);
	else
		seq_printf(s, "(invalid)\n");

	switch (ios->bus_mode) {
	case MMC_BUSMODE_OPENDRAIN:
		str = "open drain";
		break;
	case MMC_BUSMODE_PUSHPULL:
		str = "push-pull";
		break;
	default:
		str = "invalid";
		break;
	}
	seq_printf(s, "bus mode:\t%u (%s)\n", ios->bus_mode, str);

	switch (ios->chip_select) {
	case MMC_CS_DONTCARE:
		str = "don't care";
		break;
	case MMC_CS_HIGH:
		str = "active high";
		break;
	case MMC_CS_LOW:
		str = "active low";
		break;
	default:
		str = "invalid";
		break;
	}
	seq_printf(s, "chip select:\t%u (%s)\n", ios->chip_select, str);

	switch (ios->power_mode) {
	case MMC_POWER_OFF:
		str = "off";
		break;
	case MMC_POWER_UP:
		str = "up";
		break;
	case MMC_POWER_ON:
		str = "on";
		break;
	default:
		str = "invalid";
		break;
	}
	seq_printf(s, "power mode:\t%u (%s)\n", ios->power_mode, str);
	seq_printf(s, "bus width:\t%u (%u bits)\n",
			ios->bus_width, 1 << ios->bus_width);

	switch (ios->timing) {
	case MMC_TIMING_LEGACY:
		str = "legacy";
		break;
	case MMC_TIMING_MMC_HS:
		str = "mmc high-speed";
		break;
	case MMC_TIMING_SD_HS:
		str = "sd high-speed";
		break;
	case MMC_TIMING_UHS_SDR12:
		str = "sd uhs SDR12";
		break;
	case MMC_TIMING_UHS_SDR25:
		str = "sd uhs SDR25";
		break;
	case MMC_TIMING_UHS_SDR50:
		str = "sd uhs SDR50";
		break;
	case MMC_TIMING_UHS_SDR104:
		str = "sd uhs SDR104";
		break;
	case MMC_TIMING_UHS_DDR50:
		str = "sd uhs DDR50";
		break;
	case MMC_TIMING_MMC_DDR52:
		str = "mmc DDR52";
		break;
	case MMC_TIMING_MMC_HS200:
		str = "mmc HS200";
		break;
	case MMC_TIMING_MMC_HS400:
		str = "mmc HS400";
		break;
	default:
		str = "invalid";
		break;
	}
	seq_printf(s, "timing spec:\t%u (%s)\n", ios->timing, str);

	switch (ios->signal_voltage) {
	case MMC_SIGNAL_VOLTAGE_330:
		str = "3.30 V";
		break;
	case MMC_SIGNAL_VOLTAGE_180:
		str = "1.80 V";
		break;
	case MMC_SIGNAL_VOLTAGE_120:
		str = "1.20 V";
		break;
	default:
		str = "invalid";
		break;
	}
	seq_printf(s, "signal voltage:\t%u (%s)\n", ios->chip_select, str);

	switch (ios->drv_type) {
	case MMC_SET_DRIVER_TYPE_A:
		str = "driver type A";
		break;
	case MMC_SET_DRIVER_TYPE_B:
		str = "driver type B";
		break;
	case MMC_SET_DRIVER_TYPE_C:
		str = "driver type C";
		break;
	case MMC_SET_DRIVER_TYPE_D:
		str = "driver type D";
		break;
	default:
		str = "invalid";
		break;
	}
	seq_printf(s, "driver type:\t%u (%s)\n", ios->drv_type, str);

	return 0;
}

static int mmc_ios_open(struct inode *inode, struct file *file)
{
	return single_open(file, mmc_ios_show, inode->i_private);
}

static const struct file_operations mmc_ios_fops = {
	.open		= mmc_ios_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int mmc_clock_opt_get(void *data, u64 *val)
{
	struct mmc_host *host = data;

	*val = host->ios.clock;

	return 0;
}

static int mmc_clock_opt_set(void *data, u64 val)
{
	struct mmc_host *host = data;

	/* We need this check due to input value is u64 */
	if (val > host->f_max)
		return -EINVAL;

	mmc_claim_host(host);
	mmc_set_clock(host, (unsigned int) val);
	mmc_release_host(host);

	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(mmc_clock_fops, mmc_clock_opt_get, mmc_clock_opt_set,
	"%llu\n");

void mmc_add_host_debugfs(struct mmc_host *host)
{
	struct dentry *root;

	root = debugfs_create_dir(mmc_hostname(host), NULL);
	if (IS_ERR(root))
		/* Don't complain -- debugfs just isn't enabled */
		return;
	if (!root)
		/* Complain -- debugfs is enabled, but it failed to
		 * create the directory. */
		goto err_root;

	host->debugfs_root = root;

	if (!debugfs_create_file("ios", S_IRUSR, root, host, &mmc_ios_fops))
		goto err_node;

	if (!debugfs_create_file("clock", S_IRUSR | S_IWUSR, root, host,
			&mmc_clock_fops))
		goto err_node;

#ifdef CONFIG_FAIL_MMC_REQUEST
	if (fail_request)
		setup_fault_attr(&fail_default_attr, fail_request);
	host->fail_mmc_request = fail_default_attr;
	if (IS_ERR(fault_create_debugfs_attr("fail_mmc_request",
					     root,
					     &host->fail_mmc_request)))
		goto err_node;
#endif
	return;

err_node:
	debugfs_remove_recursive(root);
	host->debugfs_root = NULL;
err_root:
	dev_err(&host->class_dev, "failed to initialize debugfs\n");
}

void mmc_remove_host_debugfs(struct mmc_host *host)
{
	debugfs_remove_recursive(host->debugfs_root);
}

static int mmc_dbg_card_status_get(void *data, u64 *val)
{
	struct mmc_card	*card = data;
	u32		status;
	int		ret;

	mmc_get_card(card);

	ret = mmc_send_status(data, &status);
	if (!ret)
		*val = status;

	mmc_put_card(card);

	return ret;
}
DEFINE_SIMPLE_ATTRIBUTE(mmc_dbg_card_status_fops, mmc_dbg_card_status_get,
		NULL, "%08llx\n");

#define EXT_CSD_STR_LEN 1025

static int mmc_ext_csd_open(struct inode *inode, struct file *filp)
{
	struct mmc_card *card = inode->i_private;
	char *buf;
	ssize_t n = 0;
	u8 *ext_csd;
	int err, i;

	buf = kmalloc(EXT_CSD_STR_LEN + 1, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	mmc_get_card(card);
	err = mmc_get_ext_csd(card, &ext_csd);
	mmc_put_card(card);
	if (err)
		goto out_free;

	for (i = 0; i < 512; i++)
		n += sprintf(buf + n, "%02x", ext_csd[i]);
	n += sprintf(buf + n, "\n");
	BUG_ON(n != EXT_CSD_STR_LEN);

	filp->private_data = buf;
	kfree(ext_csd);
	return 0;

out_free:
	kfree(buf);
	return err;
}

static ssize_t mmc_ext_csd_read(struct file *filp, char __user *ubuf,
				size_t cnt, loff_t *ppos)
{
	char *buf = filp->private_data;

	return simple_read_from_buffer(ubuf, cnt, ppos,
				       buf, EXT_CSD_STR_LEN);
}

static int mmc_ext_csd_release(struct inode *inode, struct file *file)
{
	kfree(file->private_data);
	return 0;
}

static const struct file_operations mmc_dbg_ext_csd_fops = {
	.open		= mmc_ext_csd_open,
	.read		= mmc_ext_csd_read,
	.release	= mmc_ext_csd_release,
	.llseek		= default_llseek,
};

#ifdef VENDOR_EDIT
//xiaohua.tian@Prd6.BaseDrv.Sensor,2016/10/31,add for emmc life&size display
#define SECTOR_COUNT_BUF_LEN 16

static int mmc_sector_count_open(struct inode *inode, struct file *filp)
{
	struct mmc_card *card = inode->i_private;
	char *buf;
	ssize_t n = 0;
	u8 *ext_csd;
	int err;
	unsigned int sector_count = 0;

	buf = kmalloc(SECTOR_COUNT_BUF_LEN, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	memset(buf,0,SECTOR_COUNT_BUF_LEN);
#if 0
	ext_csd = kmalloc(512, GFP_KERNEL);
	if (!ext_csd) {
		err = -ENOMEM;
		goto out_free;
	}

	memset(ext_csd,0,512);
#endif
	mmc_claim_host(card->host);
#if 0
	err = mmc_send_ext_csd(card, ext_csd);
#else
	err = mmc_get_ext_csd(card, &ext_csd);
#endif
	mmc_release_host(card->host);
	if (err)
		goto out_free;
	sector_count = (ext_csd[215]<<24) |(ext_csd[214]<<16)|
			(ext_csd[213]<<8)|(ext_csd[212]);
	n = sprintf(buf, "0x%08x", sector_count);

	BUG_ON(n > SECTOR_COUNT_BUF_LEN);

	filp->private_data = buf;
	kfree(ext_csd);
	return 0;
out_free:
	kfree(buf);
#if 0
	kfree(ext_csd);
#endif
	return err;
}

static ssize_t mmc_sector_count_read(struct file *filp, char __user *ubuf,
	size_t cnt, loff_t *ppos)
{
	char *buf = filp->private_data;
	int len = strlen(buf);
	return simple_read_from_buffer(ubuf, cnt, ppos,
		buf, len);
}

static int mmc_sector_count_release(struct inode *inode, struct file *file)
{
	kfree(file->private_data);
	return 0;
}

static const struct file_operations mmc_dbg_sector_count_fops = {
	.open		= mmc_sector_count_open,
	.read		= mmc_sector_count_read,
	.release	= mmc_sector_count_release,
	.llseek		= default_llseek,
};


#define LIFE_TIME_BUF_LEN 256
static char* life_time_table[]={
	"Not defined",
	"0%-10% device life time used",
	"10%-20% device life time used",
	"20%-30% device life time used",
	"30%-40% device life time used",
	"30%-40% device life time used",
	"40%-50% device life time used",
	"60%-70% device life time used",
	"80%-90% device life time used",
	"90%-100% device life time used",
	"Exceeded its maximum estimated device life time",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
};

static int mmc_life_time_open(struct inode *inode, struct file *filp)
{
	struct mmc_card *card = inode->i_private;
	char *buf;
	ssize_t n = 0;
	u8 *ext_csd;
	int err;
	unsigned char life_time_A = 0;
	unsigned char life_time_B = 0;

	buf = kmalloc(LIFE_TIME_BUF_LEN, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	memset(buf,0,LIFE_TIME_BUF_LEN);
#if 0
	ext_csd = kmalloc(512, GFP_KERNEL);
	if (!ext_csd) {
		err = -ENOMEM;
		goto out_free;
	}

	memset(ext_csd,0,512);
#endif
	mmc_claim_host(card->host);
#if 0
	err = mmc_send_ext_csd(card, ext_csd);
#else
	err = mmc_get_ext_csd(card, &ext_csd);
#endif
	mmc_release_host(card->host);
	if (err)
		goto out_free;
	life_time_A = ext_csd[268];
	life_time_B = ext_csd[269];
	n = sprintf(buf, "type A:%s\n\rtype B:%s\n\r",
		life_time_table[life_time_A],life_time_table[life_time_B]);

	BUG_ON(n > LIFE_TIME_BUF_LEN);

	filp->private_data = buf;
	kfree(ext_csd);
	return 0;
out_free:
	kfree(buf);
#if 0
	kfree(ext_csd);
#endif
	return err;
}

static ssize_t mmc_life_time_read(struct file *filp, char __user *ubuf,
	size_t cnt, loff_t *ppos)
{
	char *buf = filp->private_data;
	int len = strlen(buf);

	return simple_read_from_buffer(ubuf, cnt, ppos,
		buf, len);
	}

static int mmc_life_time_release(struct inode *inode, struct file *file)
{
	kfree(file->private_data);
	return 0;
}

static const struct file_operations mmc_dbg_life_time_fops = {
	.open		= mmc_life_time_open,
	.read		= mmc_life_time_read,
	.release	= mmc_life_time_release,
	.llseek		= default_llseek,
};
#endif /*VENDOR_EDIT*/

#ifdef MTK_BKOPS_IDLE_MAYA
static int mmc_bkops_stats_open(struct inode *inode, struct file *filp)
{
	struct mmc_card *card = inode->i_private;

	filp->private_data = card;

	card->bkops_info.bkops_stats.print_stats = 1;
	return 0;
}

static ssize_t mmc_bkops_stats_read(struct file *filp, char __user *ubuf,
	size_t cnt, loff_t *ppos)
{
	struct mmc_card *card = filp->private_data;
	struct mmc_bkops_stats *bkops_stats;
	int i, ret;
	unsigned long page = get_zeroed_page(GFP_KERNEL);
	char *temp_buf = (char *) page;

	if (!card)
		return cnt;

	bkops_stats = &card->bkops_info.bkops_stats;
	if (!bkops_stats->print_stats)
		return 0;

	if (!bkops_stats->enabled) {
		pr_err("%s: bkops statistics are disabled\n",
			mmc_hostname(card->host));
		goto exit;
	}

	spin_lock(&bkops_stats->lock);
	temp_buf += sprintf(temp_buf, "%s: bkops statistics:\n", mmc_hostname(card->host));

	for (i = 0; i < BKOPS_NUM_OF_SEVERITY_LEVELS; ++i) {
		temp_buf += sprintf(temp_buf, "%s: BKOPS: due to level %d: %u\n",
				mmc_hostname(card->host), i, bkops_stats->bkops_level[i]);
	}
	temp_buf += sprintf(temp_buf, "%s: BKOPS: stopped due to HPI: %u\n",
				mmc_hostname(card->host), bkops_stats->hpi);
	temp_buf += sprintf(temp_buf, "%s: BKOPS: how many time host was suspended: %u\n",
				mmc_hostname(card->host), bkops_stats->suspend);
	spin_unlock(&bkops_stats->lock);
	ret = simple_read_from_buffer(ubuf, cnt, ppos, (char *) page, (unsigned long) temp_buf - page);
	free_page(page);
exit:
	if (bkops_stats->print_stats == 1) {
		bkops_stats->print_stats = 0;
		return strnlen(ubuf, cnt);
	}
	return ret;
}

static ssize_t mmc_bkops_stats_write(struct file *filp,
	const char __user *ubuf, size_t cnt, loff_t *ppos)
{
	struct mmc_card *card = filp->private_data;
	char value;
	struct mmc_bkops_stats *bkops_stats;
	int cnt;

	if (!card)
		return cnt;

	bkops_stats = &card->bkops_info.bkops_stats;

	cnt = sscanf(ubuf, "%s", &value);
	if (cnt != 1)
		return -1;
	if (value) {
		mmc_blk_init_bkops_statistics(card);
	} else {
		pr_err("enter into mmc_bkops_stats_write else bkops_stats->enabled = false\n");
		spin_lock(&bkops_stats->lock);
		bkops_stats->enabled = false;
		spin_unlock(&bkops_stats->lock);
	}

	return cnt;
}

static const struct file_operations mmc_dbg_bkops_stats_fops = {
	.open = mmc_bkops_stats_open,
	.read = mmc_bkops_stats_read,
	.write = mmc_bkops_stats_write
};
#endif

void mmc_add_card_debugfs(struct mmc_card *card)
{
	struct mmc_host	*host = card->host;
	struct dentry	*root;

	if (!host->debugfs_root)
		return;

	root = debugfs_create_dir(mmc_card_id(card), host->debugfs_root);
	if (IS_ERR(root))
		/* Don't complain -- debugfs just isn't enabled */
		return;
	if (!root)
		/* Complain -- debugfs is enabled, but it failed to
		 * create the directory. */
		goto err;

	card->debugfs_root = root;

	if (!debugfs_create_x32("state", S_IRUSR, root, &card->state))
		goto err;

	if (mmc_card_mmc(card) || mmc_card_sd(card))
		if (!debugfs_create_file("status", S_IRUSR, root, card,
					&mmc_dbg_card_status_fops))
			goto err;

	if (mmc_card_mmc(card))
		if (!debugfs_create_file("ext_csd", S_IRUSR, root, card,
					&mmc_dbg_ext_csd_fops))
			goto err;
#ifdef VENDOR_EDIT
//xiaohua.tian@Prd6.BaseDrv.Sensor,2016/10/31,add for emmc life&size display
	if (mmc_card_mmc(card))
		if (!debugfs_create_file("sector_count", S_IRUSR, root, card,
						&mmc_dbg_sector_count_fops))
			goto err;

	if (mmc_card_mmc(card))
		if (!debugfs_create_file("life_time", S_IRUSR, root, card,
						&mmc_dbg_life_time_fops))
			goto err;
#endif

	return;

err:
	debugfs_remove_recursive(root);
	card->debugfs_root = NULL;
	dev_err(&card->dev, "failed to initialize debugfs\n");
}

void mmc_remove_card_debugfs(struct mmc_card *card)
{
	debugfs_remove_recursive(card->debugfs_root);
}
