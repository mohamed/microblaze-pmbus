#ifndef SYSMON_H_
#define SYSMON_H_

#include <xsysmon.h>
#include <xparameters.h>
#include <xiic.h>
#include <xbasic_types.h>

#define SYSMON            XPAR_SYSMON_0_BASEADDR
#define SYSMON_DEVICE_ID  XPAR_SYSMON_0_DEVICE_ID

#define TEMP_ALARM_ON     70.0f         	// The temperature in C when the alarm should be set
#define TEMP_ALARM_OFF    67.0f         	// The temperature in C when the alarm should be cleared
#define R_SHUNT_FACTOR    200.0f	    	// IS 1/Rshunt (5mOhm) - if the system has a shunt (see EN_CURRENT_MEAS)
#define R_SHUNT_CHAN      XSM_VPVN_OFFSET  	// This is the name of the sysmon channel the shunt is connected to
#define V_BOARD_CHAN      XSM_AUX12_OFFSET	// This is the external channel that is connected to the board supply sensor
#define V_BOARD_FS        25.0f         	// This is the full-scale voltage on the board supply sensor
#define I_BOARD_CHAN      XSM_AUX13_OFFSET  // This is the channel for the board current sensor
#define I_BOARD_RSHUNT    1e-3				// This is the value of the Shunt Resistor in mOhms
#define I_BOARD_GAIN      50.0f				// The Board has a gain of 50x at the output of the shunt resistor
#define NUM_DELAY_CYCLES  500000	    	// Delay Period between screen updates
#define S_MAX_LEN         40		    	// Max length of a line

#define SysMon_Set(a,v)   XSysMon_WriteReg(XPAR_SYSMON_0_BASEADDR,a,v)  // alias for the sysmon low level register write driver function
#define SysMon_Get(a)     XSysMon_ReadReg(XPAR_SYSMON_0_BASEADDR,a)     // alias for the sysmon low level register read function

#define TEMP_TX(x)        ((((float)x/65536.0f)/0.00198421639f) - 273.15f)
#define TEMP_ITX(x) 		  ((int)((x + 273.15f)*65536.0f*0.00198421639f ))
#define SPLY_TX(x)  		  (((float)x)*3.0f/65536.0f )
#define SPLY_ITX(x) 	 	  ((int)(x*65536.0f/3.0f))
#define I_PART(f)         ((int)f)
#define F_PART(f,p)       ((int)((f-(float)((int)f))*(p)))


void init_system_monitor(XSysMon_Config *ConfigPtr, XSysMon SysMonInst);
int show_sysmon_data(void);

#endif 
