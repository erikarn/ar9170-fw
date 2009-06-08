#include "dt_defs.h"
#include "reg_defs.h"
#include "cam.h"

void zfDisableCamUser(u16_t userId)
{
	if (userId >= MAX_USER)
		return;

	if (userId <= 31) {
		zm_cam_roll_call_tablel_reg &= (~((u32_t) 1 << userId));
	} else if (userId <= 63) {
		zm_cam_roll_call_tableh_reg &= (~((u32_t) 1 << (userId - 32)));
	}
	return;
}

void zfEnableCamUser(u16_t userId)
{
	if (userId >= MAX_USER)
		return;

	if (userId <= 31) {
		zm_cam_roll_call_tablel_reg |= (((u32_t) 1) << userId);
	} else if (userId <= 63) {
		zm_cam_roll_call_tableh_reg |= (((u32_t) 1) << (userId - 32));
	}
	return;
}

void zfWaitForCamReadReady(void)
{
	while (((*(volatile u32_t *)0x1c373c) & 0x40000000) == 0) {
	}
	//zfUartSendStr((u8_t*)"R");
}

void zfWaitForCamWriteReady(void)
{
	while (((*(volatile u32_t *)0x1c373c) & 0x80000000) == 0) {
	}
	//zfUartSendStr((u8_t*)"W");
}

void zfDelayLoop(u32_t n)
{
	u32_t temp;
	for (temp = 0; temp < n;) {
		temp++;
	}
}

void HW_CAM_Avail(void)
{
	u32_t tmpValue;

	tmpValue = zm_cam_mode_reg;
	while (tmpValue & HOST_PEND) {
		//for(ii=0; ii<10000; ii++){}
		tmpValue = zm_cam_mode_reg;
	}

}

void HW_CAM_Write128(const u32_t address, const u32_t * data)
{
	HW_CAM_Avail();

	zm_cam_data0_reg = data[0];
	zm_cam_data1_reg = data[1];
	zm_cam_data2_reg = data[2];
	zm_cam_data3_reg = data[3];

	zm_cam_addr_reg = address | CAM_WRITE;

	zfWaitForCamWriteReady();
}

void HW_CAM_Read128(const u32_t address, u32_t * data)
{

	HW_CAM_Avail();
	zm_cam_addr_reg = address;

	zfWaitForCamReadReady();
	HW_CAM_Avail();
	data[0] = zm_cam_data0_reg;
	data[1] = zm_cam_data1_reg;
	data[2] = zm_cam_data2_reg;
	data[3] = zm_cam_data3_reg;
}
