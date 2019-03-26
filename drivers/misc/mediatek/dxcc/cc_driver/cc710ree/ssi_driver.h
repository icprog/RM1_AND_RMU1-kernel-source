/*****************************************************************************
* Copyright (C) 2015 ARM Limited or its affiliates.	                     *
* This program is free software; you can redistribute it and/or modify it    *
* under the terms of the GNU General Public License as published by the Free *
* Software Foundation; either version 2 of the License, or (at your option)  * 
* any later version.							     *
* This program is distributed in the hope that it will be useful, but 	     *
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY *
* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License   *
* for more details.							     *	
* You should have received a copy of the GNU General Public License along    *
* with this program; if not, write to the Free Software Foundation, 	     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.        *
******************************************************************************/

/* \file ssi_driver.h
   ARM CryptoCell Linux Crypto Driver
 */

#ifndef __SSI_DRIVER_H__
#define __SSI_DRIVER_H__

#include "ssi_config.h"
#ifdef COMP_IN_WQ
#include <linux/workqueue.h>
#else
#include <linux/interrupt.h>
#endif
#include <linux/dma-mapping.h>
#include <crypto/algapi.h>
#include <crypto/internal/skcipher.h>
#include <crypto/aes.h>
#include <crypto/sha.h>
#include <crypto/aead.h>
#include <crypto/authenc.h>
#include <crypto/hash.h>
#include <linux/version.h>

#include <linux/clk.h>
extern struct clk *dxcc_pub_clock;

#ifndef INT32_MAX /* Missing in Linux kernel */
#define INT32_MAX 0x7FFFFFFFL
#endif

/* Registers definitions from shared/hw/include */
#include "dx_reg_base_host.h"
#include "dx_host.h"
#define DX_CC_HOST_VIRT /* must be defined before including dx_cc_regs.h */
#include "hw_queue_defs.h"
#include "ssi_regs.h"
#include "dx_reg_common.h"
#include "ssi_hal.h"
#include "ssi_sram_mgr.h"
#define SEP_SUPPORT_SHA DX_DEV_SHA_MAX
#include "ssi_crypto_ctx.h"
#include "ssi_sysfs.h"
#include "hash_defs.h"
#include "ssi_fips_local.h"

#define DRV_MODULE_VERSION "3.0"

/* Supported CryptoCell products signature values */
/* Used to detect product ID at compile time */
#define SSI_CC641P_SIG    0xDCC63200 //0xDCC64100

/* Differentiate devices capabilities */
#if (DX_DEV_SIGNATURE == SSI_CC641P_SIG)
#define SSI_DEV_NAME_STR "cc710ree"
#define SSI_CC_HAS_ROM 1
#define SSI_CC_HAS_SEC_KEY 1
#define SSI_CC_HAS_AES_CCM 1
#define SSI_CC_HAS_AES_GCM 1
#define SSI_CC_HAS_AES_XTS 1
#define SSI_CC_HAS_AES_CTS 1
#define SSI_CC_HAS_MULTI2 1
#define SSI_CC_HAS_CMAC 1
#include "secure_key_defs.h"
#include "secure_key_int_defs.h"
#else
#error Unsupported DX_DEV_SIGNATURE value
#endif

#define SSI_AXI_IRQ_MASK ((1 << DX_AXIM_CFG1_BRESPMASK_BIT_SHIFT) | (1 << DX_AXIM_CFG1_RRESPMASK_BIT_SHIFT) |	\
			(1 << DX_AXIM_CFG1_INFLTMASK_BIT_SHIFT) | (1 << DX_AXIM_CFG1_COMPMASK_BIT_SHIFT))

#define SSI_AXI_ERR_IRQ_MASK (1 << DX_HOST_IRR_AXI_ERR_INT_BIT_SHIFT)

#define SSI_COMP_IRQ_MASK (1 << DX_HOST_IRR_AXIM_COMP_INT_BIT_SHIFT)

#define SSI_GPR7_IRQ_MASK (1 << DX_HOST_IRR_SEP_HOST_GPR7_INT_BIT_SHIFT)

#define SSI_CRA_PRIO 3000

#define MIN_HW_QUEUE_SIZE 50 /* Minimum size required for proper function */

#define MAX_REQUEST_QUEUE_SIZE 4096
#define MAX_MLLI_BUFF_SIZE 2080
#define MAX_ICV_NENTS_SUPPORTED 2

/* Definitions for HW descriptors DIN/DOUT fields */
#define NS_BIT 1
#define AXI_ID 0
/* AXI_ID is not actually the AXI ID of the transaction but the value of AXI_ID
   field in the HW descriptor. The DMA engine +8 that value. */

/* Logging macros */
#define SSI_LOG(level, format, ...) \
	printk(level "cc710ree::%s: " format , __func__, ##__VA_ARGS__)
#define SSI_LOG_ERR(format, ...) SSI_LOG(KERN_ERR, format, ##__VA_ARGS__)
#define SSI_LOG_WARNING(format, ...) SSI_LOG(KERN_WARNING, format, ##__VA_ARGS__)
#define SSI_LOG_NOTICE(format, ...) SSI_LOG(KERN_NOTICE, format, ##__VA_ARGS__)
#define SSI_LOG_INFO(format, ...) SSI_LOG(KERN_INFO, format, ##__VA_ARGS__)
#ifdef DX_DEBUG
#define SSI_LOG_DEBUG(format, ...) SSI_LOG(KERN_DEBUG, format, ##__VA_ARGS__)
#else /* Debug log messages are removed at compile time for non-DEBUG config. */
#define SSI_LOG_DEBUG(format, ...) do {} while (0)
#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define SSI_MAX_IVGEN_DMA_ADDRESSES 	3
struct ssi_crypto_req {
	void (*user_cb)(struct device *dev, void *req, void __iomem *cc_base);
	void *user_arg;
	dma_addr_t ivgen_dma_addr[SSI_MAX_IVGEN_DMA_ADDRESSES]; /* For the first 'ivgen_dma_addr_len' addresses of this array,
					 generated IV would be placed in it by send_request().
					 Same generated IV for all addresses! */
	unsigned int ivgen_dma_addr_len; /* Amount of 'ivgen_dma_addr' elements to be filled. */
	unsigned int ivgen_size; /* The generated IV size required, 8/16 B allowed. */
	struct completion seq_compl; /* request completion */
#ifdef ENABLE_CYCLE_COUNT
	enum stat_op op_type;
	cycles_t submit_cycle;
	bool is_monitored_p;
#endif
};

/**
 * struct ssi_drvdata - SeP driver private data context
 * @mem_start:	phys. address of the control registers
 * @mem_end:	phys. address of the control registers
 * @mem_size:	Control registers memory range size (mem_end - mem_start)
 * @cc_base:	virt address of the CC registers
 * @irq:	device IRQ number
 * @irq_mask:	Interrupt mask shadow (1 for masked interrupts)
 * @rom_ver:	SeP ROM version
 * @fw_ver:	SeP loaded firmware version
 * @devt_base:	Allocated char.dev. major/minor (with alloc_chrdev_region)
 * @dev:	Device context
 */
struct ssi_drvdata {
	struct resource *res_mem;
	struct resource *res_irq;
	resource_size_t mem_start;
	resource_size_t mem_end;
	resource_size_t mem_size;
	void __iomem *cc_base;
#ifdef DX_BASE_ENV_REGS
	void __iomem *env_base; /* ARM CryptoCell development FPGAs only */
#endif
	unsigned int irq;
	uint32_t irq_mask;
#if SSI_CC_HAS_ROM
	uint32_t rom_ver;
#endif
	uint32_t fw_ver;
	/* Calibration time of start/stop
	*  monitor descriptors */
	uint32_t monitor_null_cycles;
	dev_t devt_base;
	struct platform_device *plat_dev;
	ssi_sram_addr_t mlli_sram_addr;
	struct completion icache_setup_completion;
	void *buff_mgr_handle;
	void *hash_handle;
	void *aead_handle;
	void *blkcipher_handle;
	void *request_mgr_handle;
	void *ivgen_handle;
	void *sram_mgr_handle;
	void *power_mgr_handle;

#ifdef ENABLE_CYCLE_COUNT
	cycles_t isr_exit_cycles; /* Save for isr-to-tasklet latency */
#endif
	uint32_t inflight_counter;

};

struct ssi_crypto_alg {
	struct list_head entry;
	int cipher_mode;
	int flow_mode; /* Note: currently, refers to the cipher mode only. */
	int auth_mode;
	int is_secure_key;
	struct ssi_drvdata *drvdata;
	struct crypto_alg crypto_alg;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,3,0)
	struct aead_alg aead_alg;
#endif
};

struct ssi_alg_template {
	char name[CRYPTO_MAX_ALG_NAME];
	char driver_name[CRYPTO_MAX_ALG_NAME];
	unsigned int blocksize;
	u32 type;
	union {
		struct ablkcipher_alg ablkcipher;
		struct aead_alg aead;
		struct blkcipher_alg blkcipher;
		struct cipher_alg cipher;
		struct compress_alg compress;
	} template_u;
	int cipher_mode;
	int flow_mode; /* Note: currently, refers to the cipher mode only. */
	int auth_mode;
	int is_secure_key;
	bool synchronous;
	struct ssi_drvdata *drvdata;
};

struct async_gen_req_ctx {
	dma_addr_t iv_dma_addr;
	enum sep_crypto_direction op_type;
};

#ifdef DX_DUMP_BYTES
void dump_byte_array(const char *name, const uint8_t *the_array, unsigned long size);
#else
#define dump_byte_array(name, array, size) do {	\
} while (0);
#endif

#ifdef ENABLE_CYCLE_COUNT
#define DECL_CYCLE_COUNT_RESOURCES cycles_t _last_cycles_read
#define START_CYCLE_COUNT() do { _last_cycles_read = get_cycles(); } while (0)
#define END_CYCLE_COUNT(_stat_op_type, _stat_phase) update_host_stat(_stat_op_type, _stat_phase, get_cycles() - _last_cycles_read)
#define GET_START_CYCLE_COUNT() _last_cycles_read
#define START_CYCLE_COUNT_AT(_var) do { _var = get_cycles(); } while(0)
#define END_CYCLE_COUNT_AT(_var, _stat_op_type, _stat_phase) update_host_stat(_stat_op_type, _stat_phase, get_cycles() - _var)
#else
#define DECL_CYCLE_COUNT_RESOURCES 
#define START_CYCLE_COUNT() do { } while (0)
#define END_CYCLE_COUNT(_stat_op_type, _stat_phase) do { } while (0)
#define GET_START_CYCLE_COUNT() 0
#define START_CYCLE_COUNT_AT(_var) do { } while (0)
#define END_CYCLE_COUNT_AT(_var, _stat_op_type, _stat_phase) do { } while (0)
#endif /*ENABLE_CYCLE_COUNT*/

void init_cc_gpr7_interrupt(struct ssi_drvdata *drvdata);
int init_cc_regs(struct ssi_drvdata *drvdata, bool is_probe);
void fini_cc_regs(struct ssi_drvdata *drvdata);

#endif /*__SSI_DRIVER_H__*/

