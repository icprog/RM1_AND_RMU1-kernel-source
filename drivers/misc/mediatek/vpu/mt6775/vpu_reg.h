/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef _MT6769_VPU_REG_H_
#define _MT6769_VPU_REG_H_

#include <sync_write.h>

#define CTRL_BASE_OFFSET 0x80000
#define DEBUG_BASE_OFFSET 0x90000

/* common macro definitions */
#define F_REG(base, offset)     (*((unsigned int *) ((uintptr_t)base + offset)))
#define F_VAL(val, msb, lsb)    ((msb - lsb > 30) ? val : (((val)&((1<<(msb-lsb+1))-1))<<lsb))
#define F_MSK(msb, lsb)         F_VAL(0xffffffff, msb, lsb)
#define F_BIT_SET(bit)          (1<<(bit))
#define F_BIT_VAL(val, bit)     ((!!(val))<<(bit))

#define VPU_SET_BIT(reg, bit)   ((*(unsigned int *)((uintptr_t)reg)) |= (unsigned int)(1 << (bit)))
#define VPU_CLR_BIT(reg, bit)   ((*(unsigned int *)((uintptr_t)reg)) &= ~((unsigned int)(1 << (bit))))

static inline unsigned int vpu_read_reg32(unsigned long vpu_base, unsigned int offset)
{
	return ioread32((void *) (vpu_base + offset));
}

static inline void vpu_write_reg32(unsigned long vpu_base, unsigned int offset, unsigned int val)
{
	mt_reg_sync_writel(val, (void *) (vpu_base + offset));
}


/* Spare Register - Enum */
enum {
	VPU_CMD_DO_EXIT         = 0x00,
	VPU_CMD_DO_LOADER       = 0x01,
	VPU_CMD_DO_DECRYPT      = 0x02,
	VPU_CMD_DO_PASS1_DL     = 0x10,
	VPU_CMD_DO_PASS2_DL     = 0x11,
	VPU_CMD_DO_D2D          = 0x22,
	VPU_CMD_SET_DEBUG       = 0x40,
	VPU_CMD_GET_SWVER       = 0x81,
	VPU_CMD_GET_ALGO        = 0x82,

	/* Extend for test */
	VPU_CMD_EXT_BUSY        = 0xF0
};

enum {
	VPU_STATE_NOT_READY     = 0x00,
	VPU_STATE_READY         = 0x01,
	VPU_STATE_IDLE          = 0x02,
	VPU_STATE_BUSY          = 0x04,
	VPU_STATE_ERROR         = 0x08,
	VPU_STATE_TERMINATED    = 0x10
};


enum vpu_reg {
	/* module vpu */
	REG_CG_CON,
	REG_CG_SET,
	REG_CG_CLR,
	REG_SW_RST,
	REG_MBIST_MODE,
	REG_MBIST_CTL,
	REG_RP_OK0,
	REG_RP_OK1,
	/*REG_RP_OK2,*/
	REG_RP_FAIL0,
	REG_RP_FAIL1,
	/*REG_RP_FAIL2,*/
	REG_MBIST_FAIL0,
	REG_MBIST_FAIL1,
	/*REG_MBIST_FAIL2,*/
	REG_MBIST_DONE,
	REG_SRAM_DELSEL0,
	REG_SRAM_DELSEL1,
	REG_RP_RST,
	REG_RP_CON,
	REG_RP_PRE_FUSE,
	REG_SLEEP_SRAM_CTL,
	REG_SPARE0,
	REG_SPARE1,
	REG_SPARE2,
	REG_SPARE3,
	REG_EVENT_TRIG,
	REG_DONE_ST,
	REG_CTRL,
	REG_XTENSA_INT,
	REG_CTL_XTENSA_INT,
	REG_CTL_XTENSA_INT_CLR,
	REG_INT_MASK,
	REG_AXI_DEFAULT0,
	REG_AXI_DEFAULT1,
	REG_AXI_DEFAULT2,
	REG_AXI_DEFAULT3,
	REG_CABGEN_CTL,
	REG_XTENSA_INFO00,
	REG_XTENSA_INFO01,
	REG_XTENSA_INFO02,
	REG_XTENSA_INFO03,
	REG_XTENSA_INFO04,
	REG_XTENSA_INFO05,
	REG_XTENSA_INFO06,
	REG_XTENSA_INFO07,
	REG_XTENSA_INFO08,
	REG_XTENSA_INFO09,
	REG_XTENSA_INFO10,
	REG_XTENSA_INFO11,
	REG_XTENSA_INFO12,
	REG_XTENSA_INFO13,
	REG_XTENSA_INFO14,
	REG_XTENSA_INFO15,
	REG_XTENSA_INFO16,
	REG_XTENSA_INFO17,
	REG_XTENSA_INFO18,
	REG_XTENSA_INFO19,
	REG_XTENSA_INFO20,
	REG_XTENSA_INFO21,
	REG_XTENSA_INFO22,
	REG_XTENSA_INFO23,
	REG_XTENSA_INFO24,
	REG_XTENSA_INFO25,
	REG_XTENSA_INFO26,
	REG_XTENSA_INFO27,
	REG_XTENSA_INFO28,
	REG_XTENSA_INFO29,
	REG_XTENSA_INFO30,
	REG_XTENSA_INFO31,
	REG_DEBUG_INFO00,
	REG_DEBUG_INFO01,
	REG_DEBUG_INFO02,
	REG_DEBUG_INFO03,
	REG_DEBUG_INFO04,
	REG_DEBUG_INFO05,
	REG_DEBUG_INFO06,
	REG_DEBUG_INFO07,
	REG_XTENSA_ALTRESETVEC,
	REG_CAM_INT,
	REG_CAM_INT_CLR,
	VPU_NUM_REGS
};

enum vpu_reg_field {
	/* module vpu */
	FLD_IPU_CG,
	FLD_AXI_M_CG,
	FLD_JTAG_CG,
	FLD_IPU_CG_SET,
	FLD_AXI_M_CG_SET,
	FLD_JTAG_CG_SET,
	FLD_IPU_CG_CLR,
	FLD_AXI_M_CG_CLR,
	FLD_JTAG_CG_CLR,
	FLD_OCDHALTONRESET,
	FLD_IPU_D_RST,
	FLD_IPU_B_RST,
	FLD_IPU_APB_RST,
	FLD_AXI_M_RST,
	FLD_IPU_HW_RST,
	FLD_CORE_MBIST_MODE,
	FLD_CORE_MBIST_RSTB,
	FLD_CORE_MBIST_BACKGROUND,
	FLD_CORE_MBIST_BSEL,
	FLD_CORE_MBIST_RP_OK_0,
	FLD_CORE_MBIST_RP_OK_1,
	FLD_CORE_MBIST_RP_FAIL_0,
	FLD_CORE_MBIST_RP_FAIL_1,
	FLD_CORE_MBIST_FAIL_0,
	FLD_CORE_MBIST_FAIL_1,
	FLD_CORE_MBIST_DONE,
	FLD_CORE_SRAM_DELSEL_0,
	FLD_CORE_SRAM_DELSEL_1,
	FLD_CORE_RP_RSTB,
	FLD_CORE_RP_CON,
	FLD_CORE_RP_PRE_FUSE,
	FLD_CORE_SRAM_SLEEP_W,
	FLD_CORE_SRAM_SLEEP_R,
	FLD_CORE_SRAM_SLEEP_INV,
	FLD_CORE_SRAM_SLEEP_TEST,
	FLD_CORE_SRAM_HDEN,
	FLD_CORE_SPARE0,
	FLD_CORE_SPARE1,
	FLD_CORE_SPARE2,
	FLD_CORE_SPARE3,
	FLD_PWAITMODE,
	FLD_BREAK_IN_ACK,
	FLD_BREAK_OUT,
	FLD_XOCDMODE,
	FLD_P_DEBUG_ENABLE,
	FLD_STROBE,
	FLD_SRAM_CONFIGURE,
	FLD_PBCLK_ENABLE,
	FLD_RUN_STALL,
	FLD_TRIG_IN_DMA,
	FLD_BREAK_OUT_ACK,
	FLD_BREAK_IN,
	FLD_STATE_VECTOR_SELECT,
	FLD_TIE2APB_GATED_ENABLE,
	FLD_PIF_GATED,
	FLD_PRID,
	FLD_NMI,
	FLD_APMCU_INT,
	FLD_CTL_INT,
	FLD_CTL_INT_CLR,
	FLD_IPU2CAM_INT_MASK,
	FLD_APMCU_INT_MASK,
	FLD_CTL_INT_MASK,
	FLD_ARUSER_8_4,
	FLD_AWUSER_8_4,
	FLD_ARDOMAIN,
	FLD_ARFLUSH,
	FLD_ARULTRA,
	FLD_AWDOMAIN,
	FLD_AWFLUSH,
	FLD_AWULTRA,
	FLD_ARUSER_IDMA_8_4,
	FLD_AWUSER_IDMA_8_4,
	FLD_ARDOMAIN_IDMA,
	FLD_ARFLUSH_IDMA,
	FLD_ARULTRA_IDMA,
	FLD_AWDOMAIN_IDMA,
	FLD_AWFLUSH_IDMA,
	FLD_AWULTRA_IDMA,
	FLD_SPIDEN,
	FLD_SPNIDEN,
	FLD_NIDEN,
	FLD_DBG_EN,
	FLD_CABGEN2TO1_SLICE_O_ARTHRES,
	FLD_CABGEN2TO1_SLICE_O_AWTHRES,
	FLD_CABGEN2TO1_SLICE_MI0_OUTSTANDING_EXTEND_EN,
	FLD_CABGEN2TO1_SLICE_MI0_QOS_ON,
	FLD_CABGEN2TO1_SLICE_CG_DIS,
	FLD_CABGEN2TO1_SLICE_PCLK_EN,
	FLD_XTENSA_INFO00,
	FLD_XTENSA_INFO01,
	FLD_XTENSA_INFO02,
	FLD_XTENSA_INFO03,
	FLD_XTENSA_INFO04,
	FLD_XTENSA_INFO05,
	FLD_XTENSA_INFO06,
	FLD_XTENSA_INFO07,
	FLD_XTENSA_INFO08,
	FLD_XTENSA_INFO09,
	FLD_XTENSA_INFO10,
	FLD_XTENSA_INFO11,
	FLD_XTENSA_INFO12,
	FLD_XTENSA_INFO13,
	FLD_XTENSA_INFO14,
	FLD_XTENSA_INFO15,
	FLD_XTENSA_INFO16,
	FLD_XTENSA_INFO17,
	FLD_XTENSA_INFO18,
	FLD_XTENSA_INFO19,
	FLD_XTENSA_INFO20,
	FLD_XTENSA_INFO21,
	FLD_XTENSA_INFO22,
	FLD_XTENSA_INFO23,
	FLD_XTENSA_INFO24,
	FLD_XTENSA_INFO25,
	FLD_XTENSA_INFO26,
	FLD_XTENSA_INFO27,
	FLD_XTENSA_INFO28,
	FLD_XTENSA_INFO29,
	FLD_XTENSA_INFO30,
	FLD_XTENSA_INFO31,
	FLD_P_DEBUG_DATA,
	FLD_IPU_INFO_01,
	FLD_P_DEBUG_STATUS,
	FLD_P_DEBUG_INB_PIF,
	FLD_P_DEBUG_INST,
	FLD_P_DEBUG_LS0_STAT,
	FLD_P_DEBUG_LS1_STAT,
	FLD_P_DEBUG_PC,
	FLD_IPU_INFO_06,
	FLD_PSLVERR,
	FLD_IRAM0_LOAD_STORE,
	FLD_P_FAULT_INFO_VALID,
	FLD_P_FATAL_ERROR,
	FLD_DOUBLE_EXCEPTION_ERROR,
	FLD_TRIG_OUT_IDMA,
	FLD_P_FAULT_INFO,
	FLD_CORE_XTENSA_ALTRESETVEC,
	FLD_IPU2CAM_INT,
	FLD_IPU2CAM_INT_WRITE_ONE_CLEAR,
	VPU_NUM_REG_FIELDS
};


struct vpu_reg_desc {
	enum vpu_reg reg;
	char *name;
	uint32_t offset;
	uint8_t size;
};


struct vpu_reg_field_desc {
	enum vpu_reg reg;
	enum vpu_reg_field field;
	char *name;
	uint8_t msb;
	uint8_t lsb;
};

extern struct vpu_reg_desc g_vpu_reg_descs[VPU_NUM_REGS];
extern struct vpu_reg_field_desc g_vpu_reg_field_descs[VPU_NUM_REG_FIELDS];

uint32_t vpu_read_field(int core, enum vpu_reg_field f);

void vpu_write_field(int core, enum vpu_reg_field f, uint32_t v);

#endif
