/*
* Copyright (C) 2016 MediaTek Inc.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See http://www.gnu.org/licenses/gpl-2.0.html for more details.
*/
#include <linux/kernel.h>
#include "cam_cal_list.h"
#include "kd_imgsensor.h"

/*Common EEPRom Driver*/
#include "common/BRCB032GWZ_3/BRCB032GWZ_3.h"
#include "common/cat24c16/cat24c16.h"
#include "common/GT24c32a/GT24c32a.h"


#define CAM_CAL_DEBUG
#ifdef CAM_CAL_DEBUG
/*#include <linux/log.h>*/
#include <linux/kern_levels.h>
#define PFX "cam_cal_list"

#define CAM_CALINF(format, args...)     pr_info(PFX "[%s] " format, __func__, ##args)
#define CAM_CALDB(format, args...)      pr_debug(PFX "[%s] " format, __func__, ##args)
#define CAM_CALERR(format, args...)     pr_info(format, ##args)
#else
#define CAM_CALINF(x, ...)
#define CAM_CALDB(x, ...)
#define CAM_CALERR(x, ...)
#endif

#ifdef VENDOR_EDIT
/*Feng.Hu@Camera.Driver 20171208 modify for sensor eeprom*/
#define MTK_MAX_CID_NUM 13
#define EEPROM_CHECK_ID_ADDR        0x0006
/*Caohua.Lin@Camera.Driver 20180423 modify for read s5k4h7 lens_id*/
#define EEPROM_CHECK_S5K4H7_ID_ADDR   0x0A0A
#define EEPROM_SENSORID_S5K3P8SX 0x0000003a
#define EEPROM_SENSORID_IMX376     0x0000001B
#define EEPROM_SENSORID_IMX476     0x00000051
#define EEPROM_SENSORID_IMX576     0x00000053
/*Caohua.Lin@Camera.Driver 20180604 add for 18311's imx576*/
#define EEPROM_SENSORID_IMX576_1    0x00000053
#define EEPROM_SENSORID_IMX519     0x00000052
/*Yijun.Tan@Camera.Driver 20180111 add for imx519 version 0.9*/
#define EEPROM_SENSORID_IMX519_09  0x00000054
/*Caohua.Lin@Camera.Driver 20180131 add for ov13855*/
#define EEPROM_SENSORID_OV13855    0x00000028
/*Caohua.Lin@Camera.Driver 20180207 add for imx371*/
#define EEPROM_SENSORID_IMX371     0x0000001B
/*Caohua.Lin@Camera.Driver 20180423 modify for read s5k4h7 lens_id*/
#define EEPROM_SENSORID_S5K4H7     0x00000061
/*Caohua.Lin@Camera.Driver 20180507 add for s5k3p9sx*/
#define EEPROM_SENSORID_S5K3P9SX   0x00000067
/*Caohua.Lin@Camera.Driver 20180706 add for s5k3p9sp*/
#define EEPROM_SENSORID_S5K3P9SP   0x00000062
#define EEPROM_SENSORID_S5K3L6     0x00000064
#define S5K3P9SP_F_SENSOR_ID       0x310A


#else
#define MTK_MAX_CID_NUM 4
#endif

unsigned int mtkCidList[MTK_MAX_CID_NUM] = {
	#ifdef VENDOR_EDIT
	/*Feng.Hu@Camera.Driver 20171208 modify for sensor eeprom*/
	EEPROM_SENSORID_S5K3P8SX,
	EEPROM_SENSORID_IMX576,
	/*Caohua.Lin@Camera.Driver 20180604 add for 18311's imx576*/
	EEPROM_SENSORID_IMX576_1,
	EEPROM_SENSORID_IMX476,
	EEPROM_SENSORID_IMX519,
	/*Yijun.Tan@Camera.Driver 20180111 add for imx519 version 0.9*/
	EEPROM_SENSORID_IMX519_09,
	EEPROM_SENSORID_IMX376,
	/*Caohua.Lin@Camera.Driver 20180131 add for ov13855*/
	EEPROM_SENSORID_OV13855,
	EEPROM_SENSORID_IMX371,
	/*Caohua.Lin@Camera.Driver 20180423 modify for read s5k4h7 lens_id*/
	EEPROM_SENSORID_S5K4H7,
	/*Caohua.Lin@Camera.Driver 20180507 add for s5k3p9*/
	EEPROM_SENSORID_S5K3P9SX,
	EEPROM_SENSORID_S5K3P9SP,
	EEPROM_SENSORID_S5K3L6,
	#else
	0x010b00ff,/*Single MTK Format*/
	0x020b00ff,/*Double MTK Format in One OTP/EEPRom - Legacy*/
	0x030b00ff,/*Double MTK Format in One OTP/EEPRom*/
	0x040b00ff /*Double MTK Format in One OTP/EEPRom V1.4*/
	#endif
};

struct stCAM_CAL_FUNC_STRUCT g_camCalCMDFunc[] = {
	{CMD_BRCB032GWZ, brcb032gwz_selective_read_region},
	{CMD_CAT24C16, cat24c16_selective_read_region},
	{CMD_GT24C32A, gt24c32a_selective_read_region},

	/*      ADD before this line */
	{0, 0} /*end of list*/
};

struct stCAM_CAL_LIST_STRUCT g_camCalList[] = {
	#ifdef VENDOR_EDIT
	/*Feng.Hu@Camera.Driver 20171208 modify for sensor eeprom*/
	{S5K3P8SX_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{IMX576_SENSOR_ID, 0xA2, CMD_AUTO, cam_cal_check_mtk_cid},
	/*Caohua.Lin@Camera.Driver 20180604 add for 18311's imx576*/
	{IMX576_SENSOR_ID, 0xA8, CMD_AUTO, cam_cal_check_mtk_cid},
	{IMX476_SENSOR_ID, 0xA2, CMD_AUTO, cam_cal_check_mtk_cid},
	{IMX519_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	/*Yijun.Tan@Camera.Driver 20180111 add for imx519 version 0.9*/
	{IMX519_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{IMX376_SENSOR_ID, 0xA2, CMD_AUTO, cam_cal_check_mtk_cid},
	/*Caohua.Lin@Camera.Driver 20180131 add for ov13855*/
	{OV13855_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{IMX371_SENSOR_ID, 0xA2, CMD_AUTO, cam_cal_check_mtk_cid},
	/*Caohua.Lin@Camera.Driver 20180423 modify for read s5k4h7 lens_id*/
	{S5K4H7_SENSOR_ID, 0x20, CMD_AUTO, cam_cal_check_mtk_s5k4h7_cid},
	/*Caohua.Lin@Camera.Driver 20180507 add for s5k3p9*/
	{S5K3P9SX_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{S5K3P9SP_F_SENSOR_ID, 0xA2, CMD_AUTO, cam_cal_check_mtk_cid},
	{S5K3L6_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	#else
/*Below is commom sensor */
	{IMX338_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{S5K4E6_SENSOR_ID, 0xA8, CMD_AUTO, cam_cal_check_mtk_cid},
	{IMX386_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{S5K3M3_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{S5K2L7_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{IMX398_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_double_eeprom},
	{IMX318_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{OV8858_SENSOR_ID, 0xA8, CMD_AUTO, cam_cal_check_mtk_cid},
	{IMX350_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{S5K3P8SX_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_double_eeprom},
	{IMX386_MONO_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{IMX576_SENSOR_ID, 0xA2, CMD_AUTO, cam_cal_check_double_eeprom},
/*99 */
	{IMX258_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{IMX258_MONO_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
/*97*/
	{OV23850_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{OV23850_SENSOR_ID, 0xA8, CMD_AUTO, cam_cal_check_mtk_cid},
	{S5K3M2_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
/*39*/
	{OV13870_SENSOR_ID, 0xA8, CMD_AUTO, cam_cal_check_mtk_cid},
	{OV8856_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
/*55*/
	{S5K2P8_SENSOR_ID, 0xA2, CMD_AUTO, cam_cal_check_mtk_cid},
	{S5K2P8_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{OV8858_SENSOR_ID, 0xA2, CMD_AUTO, cam_cal_check_mtk_cid},
/* Others */
	{S5K2X8_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{IMX376_SENSOR_ID, 0xA2, CMD_AUTO, cam_cal_check_mtk_cid},
	{IMX214_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	{IMX214_MONO_SENSOR_ID, 0xA0, CMD_AUTO, cam_cal_check_mtk_cid},
	#endif/*VENDOR_EDIT*/
	/*  ADD before this line */
	{0, 0, CMD_NONE, 0} /*end of list*/
};


unsigned int cam_cal_get_sensor_list(struct stCAM_CAL_LIST_STRUCT **ppCamcalList)

{
	if (ppCamcalList == NULL)
		return 1;

	*ppCamcalList = &g_camCalList[0];
	return 0;
}


unsigned int cam_cal_get_func_list(struct stCAM_CAL_FUNC_STRUCT **ppCamcalFuncList)
{
	if (ppCamcalFuncList == NULL)
		return 1;

	*ppCamcalFuncList = &g_camCalCMDFunc[0];
	return 0;
}

unsigned int cam_cal_check_mtk_cid(struct i2c_client *client, cam_cal_cmd_func readCamCalData)
{
	unsigned int calibrationID = 0, ret = 0;
	int j = 0;

	if (readCamCalData != NULL) {
		#ifdef VENDOR_EDIT
		/*Feng.Hu@Camera.Driver 20171209 modify for sensor eeprom*/
		readCamCalData(client, EEPROM_CHECK_ID_ADDR, (unsigned char *)&calibrationID, 1);
		#else
		readCamCalData(client, 1, (unsigned char *)&calibrationID, 4);
		#endif
		CAM_CALDB("calibrationID = %x\n", calibrationID);
	}

	if (calibrationID != 0)
		for (j = 0; j < MTK_MAX_CID_NUM; j++) {
			CAM_CALDB("mtkCidList[%d] =0x%x, calibrationID = 0x%x\n", j, mtkCidList[j], calibrationID);
			if (mtkCidList[j] == calibrationID) {
				ret = 1;
				break;
			}
		}

	CAM_CALDB("ret =%d\n", ret);
	return ret;
}

#ifdef VENDOR_EDIT
/*Caohua.Lin@Camera.Driver 20180423 modify for read s5k4h7 lens_id*/
unsigned int cam_cal_check_mtk_s5k4h7_cid(struct i2c_client *client, cam_cal_cmd_func readCamCalData)
{
	unsigned int calibrationID = 0, ret = 0;
	int j = 0;

	if (readCamCalData != NULL) {
		readCamCalData(client, EEPROM_CHECK_S5K4H7_ID_ADDR, (unsigned char *)&calibrationID, 1);
		CAM_CALDB("calibrationID = %x\n", calibrationID);
	}

	if (calibrationID != 0)
		for (j = 0; j < MTK_MAX_CID_NUM; j++) {
			CAM_CALDB("mtkCidList[%d] == %x\n", j, calibrationID);
			if (mtkCidList[j] == calibrationID) {
				ret = 1;
				break;
			}
		}

	CAM_CALDB("ret =%d\n", ret);
	return ret;
}
#endif

unsigned int cam_cal_check_double_eeprom(struct i2c_client *client, cam_cal_cmd_func readCamCalData)
{
	unsigned int calibrationID = 0, ret = 1;

	CAM_CALDB("start cam_cal_check_double_eeprom !\n");
	if (readCamCalData != NULL) {
		CAM_CALDB("readCamCalData != NULL !\n");
		readCamCalData(client, 1, (unsigned char *)&calibrationID, 4);
		CAM_CALDB("calibrationID = %x\n", calibrationID);
	}

	return ret;
}



