#include <stdio.h>
#include <xuartlite_l.h>
#include <xparameters.h>
#include <xio.h>
#include <xbasic_types.h>
#include <mb_interface.h>
#include <xsysmon.h>
#include <xiic.h>
#include <math.h>

#include "../include/sysmon.h"
#include "../include/pmbus.h"

float convert_pmbus_reading(u16 pmbus_reading)
{
	u8 x_e_u;
	s8 x_e;
	float x_p;
	s16 x_m;

	/* TODO: Handle negative mantissa:
		It will never happen since current and power can't be negative */

	/* Extract the mantissa and exponent. See Section 7.1 in PMBus Spec. - Part 2*/
	x_m = (pmbus_reading & 0x07FF);
	x_e_u = ((pmbus_reading & 0xF800) >> 11);

	/* Convert from 2's complement */
	if (x_e_u > 15) {
		x_e_u = ((x_e_u ^ 255) + 1) & 0x1F;
		x_e = -x_e_u;
	}
	x_p = pow(2.0, (float)x_e);
	return ((float)x_m)*x_p;
}

int main()
{
	int i;
	u8 c;
	XSysMon_Config *ConfigPtr;
	XSysMon SysMonInst;
	u8 Vout[2];
	u8 Iout[2];
	u8 Pout[2];
	u16 vout_int;
	u16 iout_int;
	u16 pout_int;
	float iout_f;
	float vout_f;
	float pout_f;

	microblaze_disable_interrupts();

	init_system_monitor(ConfigPtr, SysMonInst);
	PMBusInit();

	while (1) {


		PMBus_PEC(READ_VOUT,1,0,Vout);
		PMBus_PEC(READ_IOUT,1,0,Iout);
		PMBus_PEC(READ_POUT,1,0,Pout);
		vout_int = (Vout[1] << 8) | Vout[0];
		iout_int = (Iout[1] << 8) | Iout[0];
		pout_int = (Pout[1] << 8) | Pout[0];
		
		vout_f = ((float)vout_int) * V_SCALE;
		iout_f = convert_pmbus_reading(iout_int);
		pout_f = convert_pmbus_reading(pout_int);
		
		xil_printf("PMBus Measurements\r\n");
		xil_printf("Vout = %0d.%05d V\r\n", I_PART(vout_f), F_PART(vout_f,100000.0f));
		xil_printf("Iout = %0d.%05d A\r\n", I_PART(iout_f), F_PART(iout_f,100000.0f));
		xil_printf("Pout = %0d.%05d W\r\n", I_PART(pout_f), F_PART(pout_f,100000.0f));
		show_sysmon_data();
	}

	/*microblaze_disable_interrupts();*/

	return 0;
}
