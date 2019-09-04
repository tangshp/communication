#ifndef _USER_PROTCOL_H_
#define _USER_PROTCOL_H_
#include <stdint.h>
/*protocol*/
enum _ptl_type//
{
	val_START=0,
	VAL_POSE,
	VAL_IMU_RAW,
	VAL_VEL,
	VAL_PID,
	VAL_IMU,
	VAL_END,
};
#define _SERIAL_SYN_CODE_START 0xFA
#define _SERIAL_SYN_CODE_CR 0xAF
#define _SERIAL_SYN_CODE_LF 0xFF
#pragma pack(1)//
typedef struct _serial_data
{
	uint8_t syn;
	uint8_t type;
	union{
		struct{
			//float linear[3],angular[3];
			float linear[2];
		}vel;
		/*struct{
			bool rot_ok,acc_ok,mag_ok;
			double rot[3],acc[3],mag[3];
		}imu;
		float pid[3];
		*/
	}dat;
	uint8_t sys_CR;
	uint8_t syn_LF;
}serialData;
#pragma pack()/*_USER_PROTCOL_H_*/
#endif