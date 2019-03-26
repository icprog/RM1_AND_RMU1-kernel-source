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
#include <linux/types.h>
#include <mt-plat/mtk_battery.h>
#include <mt-plat/mtk_boot.h>
#include <mtk_gauge_class.h>
#include <mach/mtk_battery_property.h>
#include <mtk_battery_internal.h>
#if VENDOR_EDIT
//PengNan@BSP.CHG.Basic, 2017/09/07, add for compatabling two fuelgauge.
extern bool oppo_gauge_get_batt_current(void);
extern int oppo_chg_get_soc(void);
extern int oppo_chg_get_ui_soc(void);
extern int oppo_gauge_get_batt_temperature(void);
#endif /*VENDOR_EDIT*/
/************** New Interface *******************/
bool oppo_get_current_status(int *bat_current)
{
	*bat_current = oppo_gauge_get_batt_current() * 10;
	if(*bat_current >= 0){
		return false;
	} else 
		return true;
		
}
bool battery_get_bat_current_sign(void)
{
	int curr_val;

	return gauge_get_current(&curr_val);
}

signed int battery_get_bat_current(void)
{
	int curr_val;

    gauge_get_current(&curr_val);
	return curr_val;
}

signed int battery_get_bat_current_mA(void)
{
	int bat_current;
	int bat_current_sign;

	bat_current_sign = gauge_get_current(&bat_current);
	if (bat_current_sign == 1)
		return bat_current / 10;
	else
		return (0 - bat_current / 10);
}


signed int battery_get_bat_avg_current(void)
{
	bool valid;

	return gauge_get_average_current(&valid);
}

signed int battery_get_bat_voltage(void)
{
	return pmic_get_battery_voltage();
}

signed int battery_get_bat_avg_voltage(void)
{
	/*return battery_get_bat_voltage();*/
	return FG_status.nafg_vbat;
}

signed int battery_get_bat_soc(void)
{
	return FG_status.soc;
}
#ifdef VENDOR_EDIT
/* Qiao.Hu@EXP.BSP.BaseDrv.CHG.Basic, 2017/11/29, Add for mt6771 charger */
extern int fgauge_is_start;
#endif /* VENDOR_EDIT */
signed int battery_get_bat_uisoc(void)
{
	int boot_mode = get_boot_mode();

	if ((boot_mode == META_BOOT) ||
		(boot_mode == ADVMETA_BOOT) ||
		(boot_mode == FACTORY_BOOT) ||
		(boot_mode == ATE_FACTORY_BOOT))
		return 75;

	#ifdef VENDOR_EDIT
	/* Qiao.Hu@EXP.BSP.BaseDrv.CHG.Basic, 2017/11/29, Add for mt 6771 charger */
	if(fgauge_is_start == 1) {
		return FG_status.ui_soc;
	} else {
		return -1;
	}
	#else
	return FG_status.ui_soc;
	#endif /* VENDOR_EDIT */
}

signed int battery_get_bat_temperature(void)
{
	/* TODO */
	if (is_battery_init_done())
		return force_get_tbat(true);
	else
		return -127;
}

signed int battery_get_ibus(void)
{
	return pmic_get_ibus();
}

signed int battery_get_vbus(void)
{
	return pmic_get_vbus();
}

unsigned int battery_get_is_kpoc(void)
{
	return bat_is_kpoc();
}

bool battery_is_battery_exist(void)
{
	return pmic_is_battery_exist();
}

/************** Old Interface *******************/
void wake_up_bat(void)
{

}
EXPORT_SYMBOL(wake_up_bat);

signed int battery_meter_get_battery_temperature(void)
{
	return battery_get_bat_temperature();
}

signed int battery_meter_get_charger_voltage(void)
{
	return battery_get_vbus();
}

unsigned long BAT_Get_Battery_Current(int polling_mode)
{
	return (long)battery_get_bat_avg_current();
}

unsigned long BAT_Get_Battery_Voltage(int polling_mode)
{
	long int ret;

	ret = (long)pmic_get_battery_voltage();
	return ret;
}

unsigned int bat_get_ui_percentage(void)
{
	return battery_get_bat_uisoc();
}

/*user: mtk_pe20_intf.c:  pe20_check_leave_status()*/
int get_soc(void)
{
	return battery_get_bat_soc();
}

/*user: mtk_charger.c: show_Pump_Express()*/
int get_ui_soc(void)
{
	return battery_get_bat_uisoc();
}

signed int battery_meter_get_battery_current(void)
{
	return battery_get_bat_current();
}

bool battery_meter_get_battery_current_sign(void)
{
	return battery_get_bat_current_sign();
}

